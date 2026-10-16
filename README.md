# Fundamentals of Networking for Effective Backend Design

<p align="left">
  <a href="https://github.com/Sohila-Khaled-Abbas/udemy-networking-fundamentals/commits/main"><img src="https://img.shields.io/github/last-commit/Sohila-Khaled-Abbas/udemy-networking-fundamentals?logo=github" alt="Last commit"></a>
  <a href="https://github.com/Sohila-Khaled-Abbas/udemy-networking-fundamentals/blob/main/LICENSE"><img src="https://img.shields.io/github/license/Sohila-Khaled-Abbas/udemy-networking-fundamentals" alt="License"></a>
  <img src="https://img.shields.io/badge/Go-Networking-00ADD8?logo=go&logoColor=white" alt="Go">
  <img src="https://img.shields.io/badge/Node.js-Networking-339933?logo=node.js&logoColor=white" alt="Node.js">
  <img src="https://img.shields.io/badge/C-UDP%20Sockets-A8B9CC?logo=c&logoColor=black" alt="C UDP sockets">
  <img src="https://img.shields.io/badge/Protocols-TCP%20%7C%20UDP-blue" alt="TCP and UDP">
</p>

Hands-on networking exercises inspired by Hussein Nasser's Udemy course, [Fundamentals of Networking for Effective Backend Design](https://www.udemy.com/course/fundamentals-of-networking-for-effective-backend-design/).

This repository is based on the original project by [André Brandão](https://github.com/andrenbrandao/udemy-networking-fundamentals). It explores TCP, UDP, socket programming, concurrent connections, and TCP latency behavior through small examples in Go and Node.js, with additional C and JavaScript UDP exercises.

## What I Added to the Original Repository

The original course examples are retained. These are the additional exercises in this version:

- **C UDP server — `udpserver-c/main.c`:** a minimal UDP socket example using POSIX sockets. It binds to `127.0.0.1:5501` and receives a datagram, providing a C implementation to compare with the Go and Node.js labs. The source references [UDP Client-Server Program in C](https://github.com/nikhilroxtomar/UDP-Client-Server-Program-in-C).
- **Separate JavaScript UDP lab — `udpserver-javascript/index.mjs`:** a Node.js UDP listener using the built-in `dgram` module. It listens on `127.0.0.1:5500` and logs the received datagram and sender address.
- **Dedicated Node.js project files:** `udpserver-javascript/package.json` and `package-lock.json` organize the JavaScript exercise as its own folder.
- **Windows-oriented run instructions:** this README includes file-specific Go commands and PowerShell testing steps alongside the original networking references.

> **Note:** Node.js already includes the `dgram` module, so the JavaScript UDP example does not need a third-party package to run. Start it with `node .\index.mjs`.

## Repository Contents

| Folder | Language | What it demonstrates |
| --- | --- | --- |
| [`tcpserver-go/`](tcpserver-go/) | Go | TCP server, interactive client, and a concurrent server |
| [`tcpserver-nodejs/`](tcpserver-nodejs/) | Node.js | TCP server using the built-in `net` module |
| [`udpserver-go/`](udpserver-go/) | Go | UDP server and client |
| [`udpserver-nodejs/`](udpserver-nodejs/) | Node.js | UDP listener using `dgram` |
| [`udpserver-javascript/`](udpserver-javascript/) | JavaScript | Additional standalone UDP listener |
| [`udpserver-c/`](udpserver-c/) | C | Minimal POSIX UDP socket example |
| [`tcp-nodelay-go/`](tcp-nodelay-go/) | Go | TCP_NODELAY and Nagle's algorithm experiment |

## Requirements

- [Go](https://go.dev/dl/) for the Go examples.
- [Node.js LTS](https://nodejs.org/en/download) for the Node.js examples.
- A C compiler such as GCC, plus a POSIX-compatible environment (Linux, macOS, or WSL) for `udpserver-c`. The C source uses POSIX socket headers and is not directly compatible with native MSVC as written.

The examples use loopback addresses for local testing. You do not need to open router ports or expose these practice servers to your network.

## Getting Started

Clone the repository:

```powershell
git clone https://github.com/Sohila-Khaled-Abbas/udemy-networking-fundamentals.git
Set-Location .\udemy-networking-fundamentals
```

### 1. Go TCP server

Open **Terminal 1**:

```powershell
Set-Location .\tcpserver-go
go run .\server.go
```

Open **Terminal 2** in the repository folder:

```powershell
Set-Location .\tcpserver-go
go run .\client.go 127.0.0.1:5500
```

Type a message and press Enter. The server prints the incoming message and sends a numbered response. Type `exit` to close the client session.

The folder contains more than one Go program, so run the specific source file shown above rather than `go run .`.

### 2. Go UDP server

Open **Terminal 1**:

```powershell
Set-Location .\udpserver-go
go run .\server.go
```

Open **Terminal 2** in the repository folder:

```powershell
Set-Location .\udpserver-go
go run .\client.go 127.0.0.1:5500
```

Send a message from the client prompt. The client prints the reply received from the server. Type `exit` when finished.

### 3. Node.js TCP server

In **Terminal 1**:

```powershell
Set-Location .\tcpserver-nodejs
node .\index.mjs
```

In **Terminal 2**, check the TCP listener:

```powershell
Test-NetConnection 127.0.0.1 -Port 5500
```

A successful test reports `TcpTestSucceeded : True`. The server logs when a client connects. Stop the server with `Ctrl+C` when you are done.

### 4. Node.js UDP server

Run either the original Node.js UDP example or the additional JavaScript example. **Do not start both at once** because both bind to `127.0.0.1:5500`.

For the additional example:

```powershell
Set-Location .\udpserver-javascript
node .\index.mjs
```

In a second PowerShell window, send a test datagram:

```powershell
$udp = [System.Net.Sockets.UdpClient]::new()
$bytes = [System.Text.Encoding]::UTF8.GetBytes('Hello UDP')
[void]$udp.Send($bytes, $bytes.Length, '127.0.0.1', 5500)
$udp.Dispose()
```

The server should print the message and the sender address/port. UDP is connectionless, so this test sends a datagram rather than establishing a TCP connection.

### 5. C UDP server (Linux, macOS, or WSL)

The C example uses POSIX socket headers. Run it from a POSIX-compatible shell, such as Ubuntu under WSL.

In **Terminal 1**, compile and run it:

```bash
cd udpserver-c
gcc main.c -o udpserver
./udpserver
```

In **Terminal 2**, send one datagram if Netcat is available:

```bash
printf 'Hello from the client' | nc -u -w 1 127.0.0.1 5501
```

This is a deliberately minimal learning example: it receives one datagram and then exits. The code is not intended as a production-ready UDP server.

## TCP_NODELAY and Nagle's Algorithm

The `tcp-nodelay-go/` folder contains a Go TCP client and server for exploring how Nagle's algorithm can affect small writes.

Start the server in one terminal:

```powershell
Set-Location .\tcp-nodelay-go
go run .\server.go
```

Run the client in a second terminal:

```powershell
Set-Location .\tcp-nodelay-go
go run .\client.go
```

For local-only testing, set the server listener in `server.go` to `127.0.0.1:8000` before running it. In `client.go`, the line below can be enabled to disable TCP_NODELAY and observe the effect of Nagle's algorithm:

```go
conn.SetNoDelay(false)
```

Packet-capture command examples vary by operating system. On Linux, `tcpdump` commonly uses the loopback interface `lo`; on Windows, Wireshark can be used to inspect loopback traffic when loopback capture is available.

Further reading:
- [Control packet flow with TCP_NODELAY](https://blog.gopheracademy.com/advent-2019/control-packetflow-tcp-nodelay/)
- [Nagle, delayed acknowledgements, and TCP performance](https://www.extrahop.com/company/blog/2016/tcp-nodelay-nagle-quickack-best-practices/)
- [Developing UDP and TCP clients and servers in Go](https://www.linode.com/docs/guides/developing-udp-and-tcp-clients-and-servers-in-go/)

## Further Reading

- [Build a TCP Connection Pool From Scratch With Go](https://betterprogramming.pub/build-a-tcp-connection-pool-from-scratch-with-go-d7747023fe14)

## Attribution and License

The original networking examples and MIT license are by André Brandão. The course material is by Hussein Nasser. Additional C and JavaScript UDP exercises are documented above.

See [LICENSE](LICENSE) for the license terms.
