# Networking Fundamentals — Hands-on Lab Notebook

<p align="center">
  <strong>Learn the network by running it, observing it, and explaining what happened.</strong><br/>
  <sub>A Windows-first, PowerShell-friendly lab collection by Sohila Khaled Abbas.</sub>
</p>

<p align="center">
  <a href="https://github.com/Sohila-Khaled-Abbas/udemy-networking-fundamentals/actions/workflows/validate.yml"><img alt="Validation" src="https://github.com/Sohila-Khaled-Abbas/udemy-networking-fundamentals/actions/workflows/validate.yml/badge.svg"></a>
  <a href="LICENSE"><img alt="MIT license" src="https://img.shields.io/badge/license-MIT-blue.svg"></a>
  <a href="https://nodejs.org/"><img alt="Node.js" src="https://img.shields.io/badge/Node.js-LTS-339933?logo=node.js&logoColor=white"></a>
  <a href="https://go.dev/"><img alt="Go" src="https://img.shields.io/badge/Go-labs-00ADD8?logo=go&logoColor=white"></a>
</p>

> **Why I’m building this:** I’m strengthening my backend and systems fundamentals alongside my path into Data Engineering and BI. Networking knowledge helps me reason about APIs, database connections, service-to-service communication, latency, TLS, and the failures that can sit underneath a data pipeline.

This repository accompanies [Fundamentals of Networking for Effective Backend Design on Udemy](https://www.udemy.com/course/fundamentals-of-networking-for-effective-backend-design/). It combines the course's core ideas with small experiments, Windows-friendly runbooks, packet inspection, and a few implementation comparisons across Go, Node.js, and C.

**Learning style:** small experiments, visible output, explicit assumptions, repeatable commands, and notes about what each result does—and does not—prove.

---

## Contents

- [Learning path](#learning-path)
- [SQL Server local-only access control](docs/sql-server-local-access-control.md)
- [Routing and Docker httpd networking labs](docs/routing-and-docker-httpd-labs.md)
- [Lab catalog](#lab-catalog)
- [Environment setup](#environment-setup)
- [Run your first lab](#run-your-first-lab)
- [How to work through an experiment](#how-to-work-through-an-experiment)
- [Engineering and safety principles](#engineering-and-safety-principles)
- [Repository structure](#repository-structure)
- [References and attribution](#references-and-attribution)

## Learning path

Follow this sequence so each layer gives context to the next:

| Stage | Question to answer | Labs |
| --- | --- | --- |
| 1 · Sockets | What does it mean for a process to listen on an IP address and port? | TCP and UDP servers in Go, Node.js, and C |
| 2 · HTTP | What changes when bytes follow HTTP request/response rules? | [Node.js HTTP module](http-module/README.md) |
| 3 · Name resolution | How does a hostname become an address? | [DNS lookup lab](section-06-protocols/README.md#34-dns--resolve-names-and-inspect-record-types) |
| 4 · Transport security | What does a TLS client verify before trusting a server? | [TLS inspection](section-06-protocols/README.md#35-tls--inspect-a-real-handshake) |
| 5 · HTTPS and identity | How do certificates, keys, and trust fit together? | [Local HTTPS lab](section-06-protocols/README.md#36-https-tls-keys-and-certificates--local-lab) |
| 6 · Observability | What can a packet capture show that application logs cannot? | [TCP packet-capture lab](docs/tcpdump-capture-lab.md) |
| 7 · Performance | How can small TCP writes interact with Nagle's algorithm? | [TCP_NODELAY experiment](tcp-nodelay-go/) |
| 8 · Database access control | How can a local SQL Server listener be kept off the network without breaking local BI/ETL tools? | [SQL Server local-only firewall lab](docs/sql-server-local-access-control.md) |
| 9 · Routing and containers | How do host routes, Docker bridge networks, container IPs, and published ports fit together? | [Routing + Docker httpd labs](docs/routing-and-docker-httpd-labs.md) |

## Lab catalog

| Experiment | Implementation | Start here |
| --- | --- | --- |
| TCP request/response | Go | [tcpserver-go](tcpserver-go/) |
| TCP listener | Node.js `net` | [tcpserver-nodejs](tcpserver-nodejs/) |
| Additional TCP listener | JavaScript / Node.js | [tcpserver-javascript](tcpserver-javascript/) |
| TCP server with POSIX sockets and Winsock versions | C | [tcpserver-c](tcpserver-c/) |
| UDP client/server | Go | [udpserver-go](udpserver-go/) |
| UDP listener | Node.js `dgram` | [udpserver-nodejs](udpserver-nodejs/) |
| Additional UDP listener | JavaScript / Node.js | [udpserver-javascript](udpserver-javascript/) |
| UDP receive-once server | C: POSIX and Winsock | [udpserver-c](udpserver-c/) |
| HTTP request/response | Node.js `http` | [http-module](http-module/README.md) |
| DNS, TLS, certificates, verified HTTPS | Node.js + OpenSSL | [section-06-protocols](section-06-protocols/README.md) |
| TCP capture and filters | `tcpdump` in WSL | [capture guide](docs/tcpdump-capture-lab.md) |
| Small-write behavior | Go TCP client/server | [tcp-nodelay-go](tcp-nodelay-go/) |
| SQL Server local-only access control | Windows Defender Firewall + PowerShell discovery | [SQL Server firewall lab](docs/sql-server-local-access-control.md) |
| Host routing + Docker HTTP | Windows routing table, Docker bridge, official Apache `httpd:2.4` image | [Routing and Docker httpd labs](docs/routing-and-docker-httpd-labs.md) |

The exercises are intentionally small. They are for learning protocol behavior—not production servers, load testing, or security testing against systems you do not own.

## Environment setup

Core tools:

- **Git** — clone and track your experiments.
- **Node.js LTS** — for the Node.js labs; the examples use built-in modules and generally need no third-party packages.
- **Go** — for the Go TCP, UDP, and TCP_NODELAY examples.
- **A C compiler** — use [w64devkit](https://github.com/skeeto/w64devkit/releases) for the Windows Winsock files, or GCC in Linux/WSL for the POSIX files.
- **OpenSSL** — needed only for generating the local test credentials in Section 6.
- **WSL + tcpdump** — optional, for the packet-capture lab.
- **Docker Desktop** — optional, for the routing and Docker `httpd` labs; use Linux containers and keep published lab ports bound to `127.0.0.1`.

Check what is available in PowerShell:

```powershell
git --version
node --version
npm --version
go version
openssl version
```

A missing optional tool does not block unrelated labs. For example, you can complete the HTTP and DNS exercises without a C compiler.

## Run your first lab

### 1. Clone the repository

```powershell
git clone https://github.com/Sohila-Khaled-Abbas/udemy-networking-fundamentals.git
Set-Location .\udemy-networking-fundamentals
```

If you already cloned the repository, use `Set-Location` to enter your existing local clone instead.

### 2. Start the Node.js HTTP server

**Terminal 1:**

```powershell
Set-Location .\http-module
npm start
```

The server binds to `127.0.0.1:8080`. Keep this terminal open.

**Terminal 2:**

```powershell
Test-NetConnection 127.0.0.1 -Port 8080
$response = Invoke-WebRequest -Uri 'http://127.0.0.1:8080/'
$response.StatusCode
$response.Content
```

Expected result: `TcpTestSucceeded : True`, HTTP status `200`, and body `OK!`. The TCP check confirms a listener is reachable; the HTTP request confirms the application response. Those are different checks.

For more examples, expected output, and troubleshooting, use the [HTTP lab guide](http-module/README.md).

## How to work through an experiment

For each lab, record five things in your own notes:

1. **Question** — what are you trying to prove?
2. **Prediction** — what do you expect the client, server, or packet capture to show?
3. **Experiment** — exact commands, environment, address, and port.
4. **Observation** — actual output or a small sanitized capture excerpt.
5. **Conclusion** — what the evidence supports, what remains uncertain, and what you want to test next.

That habit transfers directly to debugging ingestion jobs, API integrations, database connectivity, and latency issues in data platforms.

## Engineering and safety principles

- **Loopback first:** examples bind to `127.0.0.1` for local practice. Do not change them to `0.0.0.0` or expose ports unless you understand the consequences.
- **Validate one layer at a time:** DNS success is not TCP success; TCP success is not TLS success; TLS success is not proof that an application request is correct.
- **Protect credentials:** the Section 6 CA and server private keys are generated locally under `section-06-protocols/certs/`, which is ignored by Git. Never commit private keys or install the demo CA as a general-purpose trust root.
- **Keep generated artifacts out of version control:** binaries, packet captures, dependency folders, and local environment files do not belong in source history.
- **Prefer reproducible checks:** use the validation script and GitHub Actions workflow before treating a change as ready.
- **Be explicit about limits:** these are educational examples, not hardened services. They do not provide production-grade authentication, authorization, rate limiting, monitoring, or operational controls.

## Repository structure

```text
.
├── .github/workflows/validate.yml  # automated syntax/build checks
├── docs/                           # packet capture, SQL Server firewall, routing, Docker, and learning notes
├── http-module/                    # built-in Node.js HTTP lab
├── section-06-protocols/            # DNS, TLS, certificates, HTTPS
├── tcp-nodelay-go/                  # TCP_NODELAY / Nagle experiment
├── tcpserver-c/                     # POSIX sockets + Windows Winsock
├── tcpserver-go/                    # TCP client/server examples
├── tcpserver-javascript/            # additional Node.js TCP example
├── tcpserver-nodejs/                # Node.js net example
├── udpserver-c/                     # POSIX sockets + Windows Winsock
├── udpserver-go/                    # UDP client/server examples
├── udpserver-javascript/            # additional Node.js UDP example
├── udpserver-nodejs/                # Node.js dgram example
├── scripts/                         # local validation helpers
├── CONTRIBUTING.md
└── SECURITY.md
```

## References and attribution

- Course: [Fundamentals of Networking for Effective Backend Design](https://www.udemy.com/course/fundamentals-of-networking-for-effective-backend-design/)
- Original upstream repository: [andrenbrandao/udemy-networking-fundamentals](https://github.com/andrenbrandao/udemy-networking-fundamentals)
- Node.js documentation: [DNS](https://nodejs.org/api/dns.html), [HTTP](https://nodejs.org/api/http.html), [TLS](https://nodejs.org/api/tls.html), [HTTPS](https://nodejs.org/api/https.html)
- Go networking documentation: [net package](https://pkg.go.dev/net)
- Microsoft WSL documentation: [Install WSL](https://learn.microsoft.com/windows/wsl/install)

Original networking examples and the MIT license are by André Brandão. Course material is by Hussein Nasser. Additional notes, Windows-specific instructions, and extensions are maintained in this learning repository. See [LICENSE](LICENSE) for terms.
