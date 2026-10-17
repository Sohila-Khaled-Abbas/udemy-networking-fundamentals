param(
    [switch]$SkipGo,
    [switch]$SkipC
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location $repoRoot

try {
    Write-Host ''
    Write-Host 'Networking Fundamentals — repository validation' -ForegroundColor Cyan
    Write-Host "Root: $repoRoot"
    Write-Host ''

    $node = Get-Command node -ErrorAction SilentlyContinue
    if ($node) {
        $nodeFiles = Get-ChildItem -Path $repoRoot -Recurse -File |
            Where-Object {
                $_.Extension -in @('.js', '.mjs') -and
                $_.FullName -notmatch '[\\/](node_modules|\.git)[\\/]'
            }

        foreach ($file in $nodeFiles) {
            Write-Host "[Node] $($file.FullName.Substring($repoRoot.Length + 1))"
            & node --check $file.FullName
            if ($LASTEXITCODE -ne 0) {
                throw "Node.js syntax check failed: $($file.FullName)"
            }
        }

        Write-Host "PASS: checked $($nodeFiles.Count) JavaScript file(s)." -ForegroundColor Green
    }
    else {
        Write-Warning 'Node.js not found; skipping JavaScript syntax checks.'
    }

    if (-not $SkipGo) {
        $go = Get-Command go -ErrorAction SilentlyContinue
        if ($go) {
            $goFiles = Get-ChildItem -Path $repoRoot -Recurse -Filter '*.go' -File |
                Where-Object { $_.FullName -notmatch '[\\/]vendor[\\/]' }

            $extension = if ($env:OS -eq 'Windows_NT') { '.exe' } else { '' }
            $built = 0

            foreach ($file in $goFiles) {
                $outputPath = Join-Path ([System.IO.Path]::GetTempPath()) (
                    'networking-lab-' + [guid]::NewGuid().ToString('N') + $extension
                )

                Write-Host "[Go]   $($file.FullName.Substring($repoRoot.Length + 1))"
                & go build -o $outputPath $file.FullName
                if ($LASTEXITCODE -ne 0) {
                    throw "Go build failed: $($file.FullName)"
                }

                Remove-Item -LiteralPath $outputPath -Force -ErrorAction SilentlyContinue
                $built++
            }

            Write-Host "PASS: built $built standalone Go source file(s)." -ForegroundColor Green
        }
        else {
            Write-Warning 'Go not found; skipping Go build checks.'
        }
    }

    if (-not $SkipC) {
        $gcc = Get-Command gcc -ErrorAction SilentlyContinue
        if ($gcc) {
            if ($env:OS -eq 'Windows_NT') {
                $cFiles = @(
                    '.\tcpserver-c\main-windows.c',
                    '.\udpserver-c\main-windows.c'
                )
            }
            else {
                $cFiles = @(
                    './tcpserver-c/main.c',
                    './udpserver-c/main.c'
                )
            }

            Write-Host "[C]    Syntax-checking $($cFiles -join ', ')"
            & gcc -Wall -Wextra -fsyntax-only @cFiles
            if ($LASTEXITCODE -ne 0) {
                throw 'C syntax check failed. Confirm that the correct platform-specific source files are selected.'
            }

            Write-Host 'PASS: C syntax checks completed.' -ForegroundColor Green
        }
        else {
            Write-Warning 'GCC not found; skipping C syntax checks.'
        }
    }

    $git = Get-Command git -ErrorAction SilentlyContinue
    if ($git) {
        $tracked = & git ls-files
        $generated = $tracked | Select-String -Pattern '(^|/)(a\.out|[^/]+\.exe|[^/]+\.pcap(ng)?)$|(^|/)(dev-ca-key|localhost-key)\.pem$'
        if ($generated) {
            $generated | ForEach-Object { Write-Host "Unexpected generated file: $($_.Line)" -ForegroundColor Red }
            throw 'Generated binary, packet capture, or private key is tracked by Git.'
        }

        Write-Host 'PASS: no known generated binaries, packet captures, or lab private keys are tracked.' -ForegroundColor Green
    }

    Write-Host ''
    Write-Host 'Validation finished. Exercise each lab manually before relying on its runtime behavior.' -ForegroundColor Cyan
}
catch {
    Write-Error $_.Exception.Message
    exit 1
}
finally {
    Pop-Location
}
