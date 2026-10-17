# Section 6 — Networking Protocols, DNS, TLS, HTTPS, Keys, and Certificates

This section turns lessons 33–36 into small, repeatable experiments using Node.js and PowerShell on Windows.

## Learning outcomes

By the end, you should be able to explain the role of each protocol, perform DNS lookups, inspect a TLS handshake and server certificate, create a local development CA and server certificate, and make an HTTPS request that validates the certificate instead of disabling security checks.

## 33. Networking Protocols Introduction

A simplified web request travels through several layers:

1. **DNS** resolves a host name such as `example.com` to one or more IP addresses.
2. **IP** routes packets to the destination IP.
3. **TCP** establishes an ordered, reliable byte stream between IP addresses and ports.
4. **TLS** negotiates encryption, integrity protection, and server authentication using certificates.
5. **HTTP** defines requests and responses (methods, paths, headers, and bodies).
6. **HTTPS** is HTTP carried over TLS.

These are layers, not competing alternatives: HTTPS uses HTTP semantics over a TLS-protected transport connection. Standard HTTPS commonly uses TCP port 443; HTTP/3 instead uses QUIC over UDP. This lab focuses on TLS over TCP.

A DNS lookup doesn't make a TCP connection by itself. A successful TCP connection doesn't prove a TLS handshake or HTTP request will succeed. Test each layer separately.

## 34. DNS — resolve names and inspect record types

No third-party npm packages are needed; DNS methods are built into Node.js.

From PowerShell, run this from the repository root:

```powershell
Set-Location .\\section-06-protocols
node .\\dns-lookup.mjs example.com
```

These relative commands work regardless of where your local clone is stored. If you are already inside `section-06-protocols`, skip the `Set-Location` command.

The program compares:

- **OS lookup** (`dns.lookup`): uses the operating system's name-resolution facilities, which can include a hosts file as well as DNS.
- **A** records: IPv4 addresses, with TTL values.
- **AAAA** records: IPv6 addresses.
- **MX** records: mail exchangers for the domain.
- Configured DNS resolver addresses.

Try another name:

```powershell
node .\dns-lookup.mjs nodejs.org
```

You can compare it with Windows tools:

```powershell
Resolve-DnsName example.com
nslookup example.com
ipconfig /displaydns
```

An AAAA or MX lookup can legitimately return no records. A timeout or `ENOTFOUND` can also indicate connectivity/resolver trouble, so read the error code rather than assuming the domain never existed.

## 35. TLS — inspect a real handshake

This exercise opens a TLS connection to a public TLS endpoint, requires normal certificate validation, and prints the negotiated TLS version, cipher, and peer certificate details.

```powershell
node .\tls-inspect.mjs example.com 443
```

Try another hostname:

```powershell
node .\tls-inspect.mjs nodejs.org 443
```

Look at:

- **Authorized** — whether Node accepted the certificate chain and host identity.
- **Protocol** — the negotiated TLS version, for example TLSv1.3 or TLSv1.2.
- **Cipher** — the negotiated cipher suite.
- **Subject / SAN** — the names the certificate represents. Host name checks rely on the certificate's Subject Alternative Name (SAN).
- **Issuer** — the certificate authority that signed the certificate.
- **Validity dates and fingerprint** — useful identity and expiry details.

If this fails, check the Internet connection, system clock, DNS, and whether a corporate/security proxy intercepts TLS. Do not work around a certificate failure by permanently disabling `rejectUnauthorized`.

You can also inspect a connection with OpenSSL if it is installed:

```powershell
openssl s_client -connect example.com:443 -servername example.com -brief
```

## 36. HTTPS, TLS, Keys, and Certificates — local lab

This lab creates a local development certificate authority (CA) and a server certificate for `localhost` and `127.0.0.1`. The HTTPS client explicitly trusts that local CA. It does **not** disable certificate validation.

### A. Check prerequisites and generate credentials

You need Node.js and OpenSSL. Check both from PowerShell:

```powershell
node --version
npm --version
openssl version
```

Generate the local-only test credentials:

```powershell
.\generate-dev-certs.ps1
```

If PowerShell cannot find OpenSSL, install OpenSSL or add the folder containing `openssl.exe` to your Windows `PATH`, then close and reopen PowerShell. If PowerShell blocks the script because of execution policy, run `powershell -NoProfile -ExecutionPolicy Bypass -File .\\generate-dev-certs.ps1`; this uses a process-specific bypass rather than changing your persistent system policy.

Generated files will appear in `certs\`:

| File | Meaning | Keep secret? |
| --- | --- | --- |
| `dev-ca-key.pem` | Private signing key for this lab's local CA | **Yes** |
| `dev-ca-cert.pem` | Public certificate for the local CA; trusted by the demo client | No, but don't install it as a general system trust root |
| `localhost-key.pem` | Private key used by the HTTPS server | **Yes** |
| `localhost-cert.pem` | Public server certificate signed by the local CA | No |
| `localhost.csr` | Certificate signing request | No |
| `dev-ca-cert.srl` | OpenSSL serial-number bookkeeping | No |

Private keys remain local: the repository ignores the generated `certs\` directory. Never commit private keys to GitHub and never use this lab CA for production systems.

The server certificate includes Subject Alternative Name (SAN) entries for `localhost` and `127.0.0.1`. Modern clients validate the requested host against the certificate's SAN; a Common Name alone is not enough.

### B. Start the HTTPS server

**Terminal 1:**

```powershell
npm run https:server
```

Expected output:

```text
HTTPS server listening on https://127.0.0.1:8443
```

Keep it running.

### C. Make a verified HTTPS request

**Terminal 2:**

```powershell
npm run https:client
```

Expected response includes `HTTP status: 200`, a negotiated TLS version/cipher, and JSON containing `Hello over HTTPS!`. Because the client trusts `certs/dev-ca-cert.pem` and the certificate matches the local address, validation should pass without `curl -k` or `rejectUnauthorized: false`.

You can also check the open TCP port in another PowerShell window:

```powershell
Test-NetConnection 127.0.0.1 -Port 8443
```

This checks the TCP listener only. It doesn't by itself validate TLS or HTTP.

### D. Inspect the certificate and key relationship

View the certificate:

```powershell
openssl x509 -in .\certs\localhost-cert.pem -noout -subject -issuer -dates -fingerprint -sha256 -ext subjectAltName
```

Inspect the public key inside the certificate:

```powershell
openssl x509 -in .\certs\localhost-cert.pem -pubkey -noout
```

Inspect the private key locally (never paste its output into a chat or commit it):

```powershell
openssl pkey -in .\certs\localhost-key.pem -check -noout
```

Conceptually:

- The **private key** stays with the server and proves possession during TLS authentication; protect it.
- The **public key** can be shared and is included in the certificate.
- The **certificate** binds a public key to an identity and is signed by an issuer.
- The **CA certificate** is the trust anchor the client has explicitly chosen for this lab.
- **TLS** negotiates session keys and protects application bytes in transit. A certificate does not encrypt all HTTP data by itself.

### E. See certificate validation fail (optional)

As a controlled experiment, try accessing the endpoint without providing the local CA to a trusted client. A browser or a client without the local CA will normally reject the lab certificate because it isn't issued by a public CA. That's expected.

Do not add the private CA key to Windows' general trust store. For this exercise the Node.js client trusts the CA only for this request.

Stop the HTTPS server with Ctrl+C.

## Troubleshooting

- **`ENOENT` for a PEM file:** run `.\generate-dev-certs.ps1` from this folder and confirm `certs\localhost-key.pem`, `localhost-cert.pem`, and `dev-ca-cert.pem` exist.
- **OpenSSL not recognized:** use a terminal where OpenSSL is available, or install OpenSSL. The Node.js scripts themselves do not need packages from npm.
- **`EADDRINUSE` on 8443:** another process is using the port; stop it or change `PORT` in `https-server.mjs` and the client port in `https-client.mjs`.
- **Certificate is not trusted / host mismatch:** regenerate the files using the supplied script/config files, run the client as documented, and check that the CA file comes from this same `certs\` folder.
- **DNS/TLS internet test fails:** local HTTPS can still work without a public internet connection once the local credentials are generated.

## Further reading

- [Node.js DNS API](https://nodejs.org/api/dns.html)
- [Node.js TLS API](https://nodejs.org/api/tls.html)
- [Node.js HTTPS API](https://nodejs.org/api/https.html)
- [OpenSSL certificate request utility](https://docs.openssl.org/3.4/man1/openssl-req/)
- [OpenSSL X.509 extensions / Subject Alternative Name](https://docs.openssl.org/3.4/man5/x509v3_config/)
