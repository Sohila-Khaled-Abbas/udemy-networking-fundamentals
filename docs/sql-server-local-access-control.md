# SQL Server 2022 Developer — local-only network access control

This lab documents a **local-PC-only** access policy for SQL Server 2022 Developer on Windows 11. It uses Windows Defender Firewall as the first enforcement layer and preserves local SSMS, Power BI, Python, ETL, and database-tool workflows.

> **Scope:** this guide changes the Windows host firewall only when you explicitly run the implementation steps. It does not expose SQL Server to the LAN or internet, change SQL logins, alter SQL Server network protocols, or assume TCP port 1433. Run PowerShell as Administrator only for the steps that require it.

## Policy and safety goals

- SQL Server is intended for this PC only; remote clients must not reach its database listener.
- Keep local connections working, including Shared Memory and loopback TCP where currently used.
- Do not change the SQL Server port, enable TCP/IP, enable SQL Server Browser, or disable existing firewall rules without evidence that the change is needed.
- Preserve a rollback path and verify connectivity after each change.
- A host firewall rule is network reachability control, **not** full enterprise NAC. It does not assess device posture or replace SQL authentication and database permissions.

## 1. Read-only discovery

Open **PowerShell** (not necessarily elevated) and run:

```powershell
Get-Service | Where-Object { $_.Name -match '^MSSQL(SERVER|\$)' -or $_.Name -match '^SQLBrowser$' } | Select-Object Name, DisplayName, Status
```

This identifies installed Database Engine services and SQL Server Browser without changing them. An instance service commonly appears as `MSSQLSERVER` (default instance) or `MSSQL$InstanceName` (named instance).

In SSMS, connect using your normal local connection and run:

```sql
SELECT
    @@SERVERNAME AS ServerName,
    SERVERPROPERTY('InstanceName') AS InstanceName,
    SERVERPROPERTY('ProductVersion') AS ProductVersion,
    CONNECTIONPROPERTY('net_transport') AS NetTransport,
    CONNECTIONPROPERTY('local_net_address') AS LocalAddress,
    CONNECTIONPROPERTY('local_tcp_port') AS LocalTcpPort;
```

**Interpretation:** `LocalTcpPort` can be `NULL` when the current connection uses Shared Memory or another non-TCP transport. Do not treat that as proof that the server has no TCP listener.

## 2. Discover the actual listener and existing firewall policy

Run the following in **PowerShell as Administrator**. These commands are read-only.

### 2.1 Current TCP listeners

```powershell
Get-NetTCPConnection -State Listen | Sort-Object LocalPort | Select-Object LocalAddress, LocalPort, OwningProcess, @{Name='Process';Expression={(Get-Process -Id $_.OwningProcess -ErrorAction SilentlyContinue).ProcessName}}
```

To map SQL-related processes and their listening ports, first inspect the SQL services from step 1, then use:

```powershell
Get-CimInstance Win32_Service | Where-Object { $_.Name -match '^MSSQL(SERVER|\$)' } | Select-Object Name, State, ProcessId, PathName
```

Compare the service process ID with the `OwningProcess` values in the listener list. Do not assume the default port. If no TCP listener is present, local SSMS may be using Shared Memory; do not enable TCP/IP just to apply a firewall rule.

### 2.2 Existing SQL-related firewall rules

```powershell
Get-NetFirewallRule -PolicyStore ActiveStore | Where-Object { $_.DisplayName -match 'SQL|MSSQL|Database Engine|SQL Server Browser' } | Select-Object DisplayName, Enabled, Direction, Action, Profile, PolicyStoreSourceType
```

Inspect ports and remote-address scopes for each relevant enabled inbound allow rule:

```powershell
Get-NetFirewallRule -PolicyStore ActiveStore -Enabled True -Direction Inbound -Action Allow | Where-Object { $_.DisplayName -match 'SQL|MSSQL|Database Engine|SQL Server Browser' } | Get-NetFirewallPortFilter | Select-Object InstanceID, Protocol, LocalPort, RemotePort
```

```powershell
Get-NetFirewallRule -PolicyStore ActiveStore -Enabled True -Direction Inbound -Action Allow | Where-Object { $_.DisplayName -match 'SQL|MSSQL|Database Engine|SQL Server Browser' } | Get-NetFirewallAddressFilter | Select-Object InstanceID, LocalAddress, RemoteAddress
```

Rule display names are not definitive: a generic rule may target the SQL Server executable without containing “SQL” in its name. Review enabled inbound allow rules that reference the SQL Server executable, and inspect the program filter:

```powershell
Get-NetFirewallRule -PolicyStore ActiveStore -Enabled True -Direction Inbound -Action Allow | Get-NetFirewallApplicationFilter | Where-Object { $_.Program -match 'sqlservr\.exe|sqlbrowser\.exe' } | Select-Object InstanceID, Program
```

Also record the current profile defaults:

```powershell
Get-NetFirewallProfile | Select-Object Name, Enabled, DefaultInboundAction, DefaultOutboundAction
```

**Save the discovery output** before making changes. If the active firewall policy is managed by Group Policy or your organization, stop and ask the administrator before changing it.

## 3. Back up firewall policy before implementation

Run in elevated PowerShell. The backup contains host firewall policy information; keep it in a private folder.

```powershell
$Backup = Join-Path $env:USERPROFILE ("Desktop\Firewall-Backup-" + (Get-Date -Format 'yyyyMMdd-HHmmss')); New-Item -ItemType Directory -Path $Backup | Out-Null; netsh advfirewall export (Join-Path $Backup 'firewall-policy.wfw'); Get-NetFirewallRule -PolicyStore ActiveStore | Export-Clixml (Join-Path $Backup 'firewall-rules.xml'); Write-Host "Backup folder: $Backup"
```

If export fails, do not continue until you have a usable backup or have confirmed that you can restore the existing policy through your normal administrative process.

## 4. Apply a local-only inbound policy

### Preferred case: SQL Server has no inbound allow rule

If the firewall is enabled, inbound traffic is blocked by default, and no applicable inbound allow rule permits the SQL listener, **do not add an allow rule**. That is the least-change outcome for a local-only server.

Verify that SQL Server Browser is not required for local connections. Do not stop or disable it blindly if other local named-instance workflows depend on it; first confirm the current usage. SQL Browser is not needed merely for a local connection that explicitly specifies the instance/port.

### If an existing rule permits remote SQL access

First confirm that the rule applies to the actual Database Engine listener and is not managed by Group Policy. Prefer editing the specific existing rule to disable its remote access, or disable only that verified rule after recording its exact name and settings. Do **not** disable every rule containing “SQL” by pattern; this could affect another instance or tool.

For a clear, independently reversible safeguard when you have confirmed the actual TCP listener port, you may add a specifically named inbound **block** rule for that port on all profiles. Replace `<PORT>` with the verified numeric listener port; never type the angle brackets literally:

```powershell
New-NetFirewallRule -DisplayName 'LocalOnly-SQL2022-Block-Remote-TCP' -Direction Inbound -Action Block -Protocol TCP -LocalPort <PORT> -Profile Any -Enabled True -Description 'Local-PC-only SQL Server policy; blocks inbound TCP to the verified SQL listener port.'
```

This block rule is intentionally broad for that local TCP port and should be used only after verifying that the port belongs to the intended SQL Server instance and is not shared by an unrelated service. Windows loopback behavior can vary by traffic path and policy; test your normal local TCP connection after applying it. If a local TCP connection unexpectedly fails, remove **only this rule** using the rollback command below and reassess the approach. Shared Memory connections do not use the TCP listener.

Do not create an inbound allow rule for `1433`, `UDP 1434`, or any other port unless the verified listener and an explicit, changed access requirement justify it. Do not create router port forwards or internet-facing rules.

## 5. Verify local tooling

After any change:

1. Reconnect in SSMS using the same server name/instance and authentication as before.
2. If Power BI, Python, ETL jobs, or other database tools use SQL Server, run a small existing local query or a harmless connection test with each relevant tool.
3. Re-run the SSMS diagnostic query above. If the client uses TCP, confirm the local TCP port matches the discovered listener.
4. Confirm the new/edited firewall rule has the intended port, profile, direction, and action:

```powershell
Get-NetFirewallRule -DisplayName 'LocalOnly-SQL2022-Block-Remote-TCP' | Format-List DisplayName, Enabled, Direction, Action, Profile, Description; Get-NetFirewallPortFilter -AssociatedNetFirewallRule (Get-NetFirewallRule -DisplayName 'LocalOnly-SQL2022-Block-Remote-TCP') | Format-List Protocol, LocalPort
```

5. If you have another device on your LAN, do not use it to attempt access unless you own/administer it and are conducting an authorized test. The intended outcome is that remote TCP access is blocked while local workflows continue to work.

If a tool breaks, capture the exact error and connection method before changing SQL Server protocols, ports, authentication, or firewall profiles.

## 6. Roll back

To remove only the optional block rule created by this guide:

```powershell
Remove-NetFirewallRule -DisplayName 'LocalOnly-SQL2022-Block-Remote-TCP'
```

If you edited or disabled a pre-existing rule, restore that exact rule's prior settings from your recorded discovery output. Do not import the entire firewall backup unless a broader restore is necessary: importing it can overwrite firewall changes made after the backup.

## Acceptance checklist

- [ ] SQL Server service and instance identified.
- [ ] Actual listener/process/port verified—or confirmed no TCP listener is present.
- [ ] Existing inbound allow rules and their port/address/application filters reviewed.
- [ ] Firewall policy backup created.
- [ ] No unverified SQL port or SQL Browser assumption was used.
- [ ] Local SSMS connection still works.
- [ ] Power BI, Python, ETL, and other local database tools still work.
- [ ] No router forwarding or internet exposure was introduced.
- [ ] Rollback command and backup location recorded.

## Microsoft references

- [Configure the Windows Firewall to Allow SQL Server Access](https://learn.microsoft.com/en-us/sql/sql-server/install/configure-the-windows-firewall-to-allow-sql-server-access)
- [Configure Windows Firewall for Database Engine Access](https://learn.microsoft.com/en-us/sql/database-engine/configure-windows/configure-a-windows-firewall-for-database-engine-access)
- [Windows Defender Firewall with Advanced Security](https://learn.microsoft.com/en-us/windows/security/operating-system-security/network-security/windows-firewall/)
