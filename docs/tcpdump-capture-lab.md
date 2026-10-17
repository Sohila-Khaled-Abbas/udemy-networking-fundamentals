# Capturing TCP Segments with tcpdump

This lab captures a local TCP connection to the C server in `tcpserver-c/`, then explains the TCP flags and how to save a packet capture.

## Why use WSL for this lab?

On Windows, the native server `main-windows.c` listens on the Windows loopback interface. Linux `tcpdump` inside WSL captures Linux/WSL interfaces, not Windows host-loopback traffic. To use the actual `tcpdump` command and reliably see every packet in this lab, run the POSIX `main.c` server and its client inside the same Ubuntu/WSL environment.

If WSL is not installed, open PowerShell as Administrator and run:

```powershell
wsl --install -d Ubuntu
```

Restart if prompted, launch Ubuntu, and create your Linux username/password. Microsoft's [official WSL installation guide](https://learn.microsoft.com/en-us/windows/wsl/install) has details.

## 1. Install the tools

In Ubuntu/WSL:

```bash
sudo apt update
sudo apt install -y build-essential tcpdump netcat-openbsd
```

## 2. Compile and start the C TCP server

Clone the repository inside WSL if you do not already have a Linux-side checkout:

```bash
git clone https://github.com/Sohila-Khaled-Abbas/udemy-networking-fundamentals.git
cd udemy-networking-fundamentals/tcpserver-c
```

Compile the POSIX source file (not `main-windows.c`):

```bash
gcc -Wall -Wextra main.c -o tcpserver
```

**Terminal 1 — start the server:**

```bash
./tcpserver
```

Expected startup:

```text
TCP server listening on 127.0.0.1:8801
Press Ctrl+C to stop the server.
```

Leave this terminal running.

## 3. Capture the TCP packets live

Open **Terminal 2** in Ubuntu/WSL and run:

```bash
sudo tcpdump -i lo -nn -tttt -S -vvv 'tcp port 8801'
```

- `-i lo`: capture on Linux loopback because both endpoints use `127.0.0.1`.
- `-nn`: keep addresses and ports numeric; skip name/service lookups.
- `-tttt`: print readable timestamps.
- `-S`: show absolute TCP sequence numbers.
- `-vvv`: verbose packet details.
- `tcp port 8801`: capture filter for this lab's TCP port only.

Keep tcpdump running while you create a connection.

## 4. Generate traffic

Open **Terminal 3** in Ubuntu/WSL and send a short message:

```bash
printf 'Hello from netcat\n' | nc -q 1 127.0.0.1 8801
```

You should receive the server's `Hello from C TCP server!` greeting from the TCP client, and Terminal 1 should log `Received: Hello from netcat`. Terminal 2 should display packets for the connection.

Press **Ctrl+C** in Terminal 2 to stop the capture.

## 5. Understand the output

Look for the connection setup (the three-way handshake):

1. Client → server: `Flags [S]` — SYN, asks to start a TCP connection.
2. Server → client: `Flags [S.]` — SYN-ACK, acknowledges the SYN and sends its own SYN.
3. Client → server: `Flags [.] ` — ACK, acknowledges the server's SYN.

Then look for data packets, usually marked with flags such as `[P.]` (PSH + ACK) or `[.]` (ACK). The exact packet count and display can vary by kernel, timing, and TCP implementation. When the client closes, you may see `[F.]` packets for orderly shutdown.

TCP flags to recognize:

| Flag | Meaning |
| --- | --- |
| `[S]` | SYN — start connection |
| `[S.]` | SYN + ACK — acknowledge and synchronize |
| `[.]` | ACK — acknowledge received bytes |
| `[P.]` | PSH + ACK — commonly seen on data segments |
| `[F.]` | FIN + ACK — close a direction of the connection |
| `[R]` or `[R.]` | RST — reset/reject a connection |

TCP is a byte stream: one application write does not guarantee exactly one TCP segment. Do not assume every call to `send` corresponds to one packet.

## 6. Save the capture to a PCAP file

Stop any live capture with Ctrl+C. Start the server in Terminal 1 again if it has stopped. In Terminal 2:

```bash
sudo tcpdump -i lo -nn -s 0 -w tcp-8801.pcap 'tcp port 8801'
```

While it is running, repeat the client command from Terminal 3. Then press Ctrl+C in Terminal 2. The capture is saved as `tcp-8801.pcap` in the current directory.

Read it back in the terminal:

```bash
tcpdump -nn -tttt -S -vvv -r tcp-8801.pcap
```

You can also open the PCAP in Wireshark for a visual packet list and expanded TCP header fields. Use `-w` to save packets; do not use `-w` and `-r` together for this simple exercise.

## 7. Useful filters

Only packets with a TCP SYN flag:

```bash
sudo tcpdump -i lo -nn 'tcp port 8801 and (tcp[tcpflags] & tcp-syn != 0)'
```

Only packets sent from or to port 8801 with ASCII payload display enabled:

```bash
sudo tcpdump -i lo -nn -A 'tcp port 8801'
```

Stop live capture with Ctrl+C. Keep captures focused on traffic you own or are authorized to inspect.
