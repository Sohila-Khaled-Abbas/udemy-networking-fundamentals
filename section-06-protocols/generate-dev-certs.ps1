$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot

if (-not (Get-Command openssl -ErrorAction SilentlyContinue)) {
    Write-Error 'OpenSSL was not found on PATH. Try running this script from Git Bash, or install OpenSSL and reopen the terminal.'
    exit 1
}

New-Item -ItemType Directory -Force -Path '.\certs' | Out-Null

if (Test-Path '.\certs\dev-ca-key.pem') {
    Write-Error 'A development CA key already exists in certs. To regenerate everything, back up any files you need and remove the certs folder first.'
    exit 1
}

Write-Host '1/3 Creating a local development CA...'
& openssl req -x509 -newkey rsa:2048 -nodes -days 3650 -sha256 -config '.\openssl\ca.cnf' -keyout '.\certs\dev-ca-key.pem' -out '.\certs\dev-ca-cert.pem'
if ($LASTEXITCODE -ne 0) { throw 'OpenSSL failed while generating the local CA.' }

Write-Host '2/3 Creating the localhost private key and certificate request...'
& openssl req -new -newkey rsa:2048 -nodes -config '.\openssl\localhost.cnf' -keyout '.\certs\localhost-key.pem' -out '.\certs\localhost.csr'
if ($LASTEXITCODE -ne 0) { throw 'OpenSSL failed while generating the localhost key/CSR.' }

Write-Host '3/3 Signing the localhost certificate with the local development CA...'
& openssl x509 -req -in '.\certs\localhost.csr' -CA '.\certs\dev-ca-cert.pem' -CAkey '.\certs\dev-ca-key.pem' -CAcreateserial -out '.\certs\localhost-cert.pem' -days 365 -sha256 -extfile '.\openssl\localhost.cnf' -extensions server_cert
if ($LASTEXITCODE -ne 0) { throw 'OpenSSL failed while signing the localhost certificate.' }

Write-Host ''
Write-Host 'Generated local-only test credentials in certs.'
Write-Host 'IMPORTANT: Never commit these keys. The certs folder is listed in the repository .gitignore.'
Write-Host 'This development CA is only for this lab and must not be used for production.'
