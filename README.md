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

- **C TCP server — `tcpserver-c/main.c` and `tcpserver-c/main-windows.c`:** a loopback-only TCP server for POSIX systems and native Windows, respectively. Both bind to `127.0.0.1:8801`, send a greeting to each client, and log received text.
- **C UDP server — `udpserver-c/main.c`:** a minimal UDP socket example using POSIX sockets for Linux, macOS, or WSL. It binds to `127.0.0.1:5501` and receives one datagram. The source references [UDP Client-Server Program in C](https://github.com/nikhilroxtomar/UDP-Client-Server-Program-in-C).
- **Native Windows C UDP server — `udpserver-c/main-windows.c`:** a Windows-compatible version implemented with Winsock. It uses the same loopback address and port, so you can compile it with GCC from [w64devkit](https://github.com/skeeto/w64devkit/releases/tag/v2.10.1) without converting the POSIX socket code yourself.
- **Separate JavaScript UDP lab — `udpserver-javascript/index.mjs`:** a Node.js UDP listener using the built-in `dgram` module. It listens on `127.0.0.1:5500` and logs the received datagram and sender address.
- **Dedicated Node.js project files:** `udpserver-javascript/package.json` and `package-lock.json` organize the JavaScript exercise as its own folder.
- **Windows-oriented run instructions:** this README includes file-specific Go commands and PowerShell testing steps alongside the original networking references.
- **TCP packet-capture lab — [`docs/tcpdump-capture-lab.md`](docs/tcpdump-capture-lab.md):** step-by-step `tcpdump` commands in WSL to capture the C TCP server's handshake, data, and teardown, with filters and PCAP export.

> **Note:** Node.js already includes the `dgram` module, so the JavaScript UDP example does not need a third-party package to run. Start it with `node .\index.mjs`.

## Repository Contents

| Folder | Language | What it demonstrates |
| --- | --- | --- |
| [`tcpserver-go/`](tcpserver-go/) | Go | TCP server, interactive client, and a concurrent server |
| [`tcpserver-nodejs/`](tcpserver-nodejs/) | Node.js | TCP server using the built-in `net` module |
| [`tcpserver-c/`](tcpserver-c/) | C | TCP server: POSIX source plus native Windows Winsock version |
| [`docs/tcpdump-capture-lab.md`](docs/tcpdump-capture-lab.md) | Lab guide | Capture and interpret TCP segments with tcpdump in WSL |
| [`udpserver-go/`](udpserver-go/) | Go | UDP server and client |
| [`udpserver-nodejs/`](udpserver-nodejs/) | Node.js | UDP listener using `dgram` |
| [`udpserver-javascript/`](udpserver-javascript/) | JavaScript | Additional standalone UDP listener |
| [`udpserver-c/`](udpserver-c/) | C | POSIX UDP server for Linux/macOS/WSL plus a native Windows Winsock version |
| [`tcp-nodelay-go/`](tcp-nodelay-go/) | Go | TCP_NODELAY and Nagle's algorithm experiment |

## Requirements

- [Go](https://go.dev/dl/) for the Go examples.
- [Node.js LTS](https://nodejs.org/en/download) for the Node.js examples.
- **GCC for the C exercises.** On Windows, download the portable [w64devkit v2.10.1 release](https://github.com/skeeto/w64devkit/releases/tag/v2.10.1). Choose `w64devkit-x64-2.10.1.7z.exe`, extract it, launch the included `w64devkit.exe` terminal, and verify the compiler with `gcc --version`. See the [w64devkit usage notes](https://github.com/skeeto/w64devkit#usage). This toolchain is portable and does not require a system-wide installation.
- The C TCP and UDP examples provide POSIX source files (`main.c`) for Linux, macOS, or WSL and native Windows versions (`main-windows.c`) for w64devkit. Build the source file inside the relevant `tcpserver-c` or `udpserver-c` folder for your environment. For WSL/Ubuntu, install the POSIX compiler with `sudo apt update && sudo apt install build-essential`.

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

### 4. C TCP server (Linux/macOS/WSL or native Windows)

The C TCP server listens on `127.0.0.1:8801`, sends a greeting to each client, and logs any text the client sends. It keeps listening until you stop it with `Ctrl+C`.

**Native Windows with w64devkit**

Open the w64devkit terminal and change to the repository's `tcpserver-c` folder. If you are at the repository root, run:

```sh
cd tcpserver-c
gcc main-windows.c -o tcpserver-windows.exe -lws2_32
./tcpserver-windows.exe
```

Expected startup message: `TCP server listening on 127.0.0.1:8801`. The `-lws2_32` flag links the Windows Winsock library. Do not compile `main.c` with native Windows w64devkit because it uses POSIX headers.

**Linux, macOS, or WSL**

From the repository root, run:

```bash
cd tcpserver-c
gcc -Wall -Wextra main.c -o tcpserver
./tcpserver
```

**Test from Windows PowerShell**

Keep the server running. In a separate PowerShell window, check the TCP port:

```powershell
Test-NetConnection 127.0.0.1 -Port 8801
```

Look for `TcpTestSucceeded : True`. Then send a message and read the greeting:

```powershell
$client = [System.Net.Sockets.TcpClient]::new()
$client.Connect('127.0.0.1', 8801)
$stream = $client.GetStream()
$reader = [System.IO.StreamReader]::new($stream)
$writer = [System.IO.StreamWriter]::new($stream)
$writer.AutoFlush = $true
$greeting = $reader.ReadLine()
$greeting
$writer.WriteLine('Hello from PowerShell')
$client.Dispose()
```

PowerShell should print `Hello from C TCP server!`, and the server window should log `Received: Hello from PowerShell`. The port check creates a brief test connection of its own; the server continues listening afterward. These examples bind to loopback only, so they do not expose the service to your network.

### 5. Standalone JavaScript TCP server

This additional TCP exercise lives in `tcpserver-javascript/`. It uses Node.js's built-in `net` module and listens on `127.0.0.1:5500`.

In **Terminal 1**, from the repository root:

```powershell
Set-Location .\\tcpserver-javascript
npm start
```

Leave the server running. It should print `TCP server listening on 127.0.0.1:5500`.

In **Terminal 2**, test the TCP port:

```powershell
Test-NetConnection 127.0.0.1 -Port 5500
```

Look for `TcpTestSucceeded : True`. To connect and send a message (also from Terminal 2), run:

```powershell
$client = [System.Net.Sockets.TcpClient]::new()
$client.Connect('127.0.0.1', 5500)
$stream = $client.GetStream()
$reader = [System.IO.StreamReader]::new($stream)
$writer = [System.IO.StreamWriter]::new($stream)
$writer.AutoFlush = $true
$reader.ReadLine()
$writer.WriteLine('Hello from PowerShell')
$client.Dispose()
```

The client should read the server greeting, and Terminal 1 should log the message. Stop the server with `Ctrl+C`.

If the test fails, confirm the server is still running and that both commands use port `5500`. If Node.js reports `EADDRINUSE`, another process is already listening on that port; stop that process or change the port in both the server and test.

### 6. Node.js UDP server

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

### 7. C UDP server (Linux, macOS, or WSL)

The C example uses **POSIX socket APIs**, so run it from a POSIX-compatible environment such as Linux, macOS, or Ubuntu under WSL. On Ubuntu/WSL, install GCC and the standard build tools once:

```bash
sudo apt update
sudo apt install build-essential
gcc --version
```

In **Terminal 1**, compile and run the server:

```bash
cd udpserver-c
gcc main.c -o udpserver
./udpserver
```

In **Terminal 2**, send one datagram if Netcat is available:

```bash
printf 'Hello from the client' | nc -u -w 1 127.0.0.1 5501
```

**Windows note:** you can also install the portable [w64devkit GCC toolchain (v2.10.1)](https://github.com/skeeto/w64devkit/releases/tag/v2.10.1) for native Windows C/C++ development. However, this repository's current `main.c` uses POSIX networking headers and will not compile with the Windows toolchain unchanged; use WSL for this version or port the code to Winsock.

This is a deliberately minimal learning example: it receives one datagram and then exits. The code is not intended as a production-ready UDP server.

### 8. C UDP server on Windows (w64devkit + Winsock)

Use this version for a native Windows build. The POSIX `main.c` uses headers that are not available in this form on Windows, so compile `main-windows.c` instead.

1. Download the x64 [w64devkit v2.10.1 release](https://github.com/skeeto/w64devkit/releases/tag/v2.10.1), extract the self-extracting archive, and start `w64devkit.exe` from the extracted folder.
2. In the w64devkit terminal, verify GCC is available:

   ```sh
   gcc --version
   ```

3. Change directory to your cloned repository's `udpserver-c` folder, then compile and run:

   ```sh
   gcc main-windows.c -o udpserver-windows.exe -lws2_32
   ./udpserver-windows.exe
   ```

   The `-lws2_32` linker flag links the Windows Winsock library. The server binds to `127.0.0.1:5501`, receives one UDP datagram, prints it, and exits.

4. While the server is waiting, open a **separate PowerShell window** and send a test datagram:

   ```powershell
   $udp = [System.Net.Sockets.UdpClient]::new()
   $bytes = [System.Text.Encoding]::UTF8.GetBytes('Hello from Windows')
   [void]$udp.Send($bytes, $bytes.Length, '127.0.0.1', 5501)
   $udp.Dispose()
   ```

   The w64devkit terminal should display the received message and the sender address/port.

Both C versions are for learning and receive a single datagram per run. They bind to the loopback address only, so you do not need to expose a server to your local network.

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
