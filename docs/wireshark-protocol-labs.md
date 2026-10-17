# Wireshark protocol labs: UDP, TCP/HTTP, HTTP/2/TLS, MongoDB, and SSE

These labs use Wireshark to connect application behavior to packets on a Windows 11 workstation. Keep traffic local or use systems and accounts you own. Packet captures can contain sensitive data; save them locally, inspect them before sharing, and do not commit `.pcapng` files, TLS secrets, cookies, tokens, or database credentials.

## Prerequisites and capture setup

- Windows 11 and [Wireshark](https://www.wireshark.org/download.html).
- Install Npcap when prompted. For loopback experiments, enable/install the Npcap loopback adapter if the installer offers it.
- Node.js LTS for the repository HTTP lab and Python 3 for the small UDP/SSE examples below.
- Docker Desktop with Linux containers for the MongoDB lab.
- Optional: a browser that supports exporting TLS session secrets through `SSLKEYLOGFILE` for the HTTP/2 lab.

Open Wireshark and identify the correct interface before starting traffic. For local clients talking to `127.0.0.1`, select the Npcap loopback adapter (often displayed as `Adapter for loopback traffic capture`). For Docker Desktop, capture visibility depends on Docker Desktop's networking backend; if the Windows loopback capture does not show container-internal traffic, capture the host-published endpoint or use the Docker-supported capture approach for your environment.

**Capture filter vs. display filter:** capture filters limit what is recorded; display filters only change which already-captured packets are shown. Start with display filters while learning.

Suggested display filters:
- UDP: `udp`
- TCP: `tcp`
- HTTP/1.x: `http`
- DNS: `dns`
- TLS: `tls`
- HTTP/2: `http2`
- MongoDB wire protocol (when dissectable): `mongodb`
- Server-Sent Events over HTTP: `http` or `tcp.port == 8090`

Wireshark's protocol dissectors and field names vary by version and traffic format. If a filter is not recognized, use the display-filter autocomplete or start with `tcp.port == <port>`.

---

## Lab 1 — Wiresharking UDP

### Goal

Observe UDP datagrams and distinguish UDP's message-oriented transport from TCP's connection-oriented byte stream.

### 1.1 Start a local UDP receiver

Save the following as `udp_receiver.py` in a temporary lab folder:

```python
import socket

HOST = "127.0.0.1"
PORT = 9999

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
    sock.bind((HOST, PORT))
    print(f"UDP receiver listening on {HOST}:{PORT}")
    while True:
        data, address = sock.recvfrom(4096)
        print(f"from={address} bytes={len(data)} payload={data!r}")
```

Run it in PowerShell Terminal 1:

```powershell
python .\udp_receiver.py
```

### 1.2 Send a few datagrams

In Terminal 2, run:

```powershell
python -c "import socket; s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM); [s.sendto(x.encode(),('127.0.0.1',9999)) for x in ['first datagram','second datagram','third datagram']]; s.close()"
```

### 1.3 Capture and inspect

1. Start capturing on the Npcap loopback adapter.
2. Run the sender command again, then stop capture.
3. Apply `udp.port == 9999`.
4. Select a packet and expand **User Datagram Protocol** and **Data**.

Record source/destination ports, UDP length, payload bytes, and how many datagrams you see. The receiver prints each datagram separately. UDP itself does not establish a connection, guarantee delivery, or retransmit lost datagrams.

**Questions:** Does each send correspond to a separate datagram? What does the UDP checksum indicate? Does seeing a packet prove the application processed it?

---

## Lab 2 — Wiresharking TCP and HTTP

### Goal

Follow a TCP connection and inspect an HTTP request/response as application data carried by TCP.

### 2.1 Start the repository HTTP server

In Terminal 1, from the repository root:

```powershell
Set-Location .\http-module
npm start
```

Use the address and port shown by the app. The repository's introductory instructions use `127.0.0.1:8080`.

### 2.2 Capture a request

1. Start a capture on the Npcap loopback adapter.
2. In Terminal 2, send a request:

```powershell
curl.exe -i http://127.0.0.1:8080/
```

3. Stop the capture and try these display filters:

```text
tcp.port == 8080
http
tcp.flags.syn == 1
```

If Wireshark does not automatically dissect the traffic as HTTP, use **Analyze → Decode As…** only for the relevant TCP port and choose HTTP, if appropriate for the captured traffic.

### 2.3 Follow the stream

Right-click a packet in the connection and choose **Follow → TCP Stream**. Inspect:
- the TCP three-way handshake (SYN, SYN/ACK, ACK), if captured from connection start;
- the HTTP request line and headers;
- the HTTP status line, response headers, and body;
- FIN/RST packets and how the connection closes.

A TCP segment is not necessarily one HTTP message: TCP carries a byte stream, so requests and responses may span multiple segments. Packet boundaries are not application-message boundaries.

**Questions:** Which packets establish TCP? Which packet(s) contain the HTTP request? Can you infer application success from the handshake alone? Why can the HTTP body be split across segments?

---

## Lab 3 — Wiresharking HTTP/2 by decrypting TLS

### Goal

Understand why HTTPS normally hides HTTP/2 contents, then decrypt a capture using session secrets generated by your own test browser. Do not use another person's keys, sessions, or traffic.

### 3.1 Start a TLS key log for a dedicated browser session

Close all browser processes for the browser you will use, then open a new PowerShell window. Example for Chrome:

```powershell
$env:SSLKEYLOGFILE = "$env:TEMP\wireshark-tls-keys.log"
Start-Process chrome.exe
```

If the browser is not on PATH, launch it by its full executable path. Support for `SSLKEYLOGFILE` depends on browser and build; verify that the file is created and grows after visiting a test HTTPS site. Do not share or commit this file: it can allow decryption of captured sessions.

In Wireshark, open **Edit → Preferences → Protocols → TLS** and set **(Pre)-Master-Secret log filename** to the exact path printed by:

```powershell
$env:TEMP + "\wireshark-tls-keys.log"
```

Set this preference before capturing, or reload the capture after configuring it.

### 3.2 Capture a test HTTP/2 connection

1. Start Wireshark capture on the active network interface used by the browser (loopback is not normally used for public sites).
2. Visit an HTTPS site you are authorized to inspect that negotiates HTTP/2. You can use your own test service; do not capture private account activity.
3. Stop capture and apply `tls` or `http2`.
4. Confirm the capture has TLS traffic and that the key log contains secrets for the session. If decryption works, HTTP/2 frames and headers may become visible.

HTTP/2 is commonly negotiated inside TLS using ALPN (often `h2`). Without the correct session secrets, Wireshark can generally show network/TLS metadata but not the encrypted HTTP headers and body. A certificate private key alone is not a general substitute for session secrets with modern TLS (for example, TLS 1.3 and ephemeral key exchange).

### 3.3 Validate and clean up

Look for the TLS handshake and ALPN negotiation, then inspect HTTP/2 frames/streams if decrypted. If only TLS records appear, check that the browser generated the key log, the exact file path is configured, and the capture contains the same browser session. Browser support and Wireshark labels may vary by version.

When finished, close the test browser and remove the local key log if you no longer need it:

```powershell
Remove-Item "$env:TEMP\wireshark-tls-keys.log" -ErrorAction SilentlyContinue
```

Also clear the TLS key-log path from Wireshark preferences if you want to stop using it. Keep captures and key logs private; never disable certificate validation to make the lab work.

**Questions:** What can you see before decryption? What changes after Wireshark has session secrets? What does ALPN tell the client and server?

---

## Lab 4 — Wiresharking MongoDB locally

### Goal

Observe a MongoDB client connection and understand that the wire protocol may be visible in a local lab without TLS, while authentication and TLS settings change what a capture reveals.

### 4.1 Start a disposable local MongoDB container

This is a local learning instance, not a production configuration. It has no configured database credentials in this example, so bind it only to loopback and stop/remove it when finished. Do not store real data in it.

```powershell
docker run -d --name wireshark-mongo-lab -p 127.0.0.1:27017:27017 mongo:8
```

If Docker reports that the port is already in use, inspect the conflicting listener/container rather than stopping it blindly. Wait for startup:

```powershell
docker logs wireshark-mongo-lab
```

### 4.2 Generate a few requests

If the image includes `mongosh`, connect inside the container:

```powershell
docker exec -it wireshark-mongo-lab mongosh
```

At the MongoDB shell, run:

```javascript
db.runCommand({ ping: 1 })
db.lab.insertOne({ topic: "wireshark", kind: "demo", n: 1 })
db.lab.find({ topic: "wireshark" })
```

Exit with `exit`. This shell runs inside the container, so it may not generate the same Windows host-to-published-port traffic as a client running on Windows. For a host-side capture, connect from a local MongoDB Compass/mongosh installation to `mongodb://127.0.0.1:27017`, if available, and run the same commands.

### 4.3 Inspect the capture

Capture on the Npcap loopback adapter while the Windows host client connects to `127.0.0.1:27017`. Use:

```text
tcp.port == 27017
mongodb
```

Follow the TCP stream and inspect packet details. Depending on Wireshark version, MongoDB wire-protocol dissection may be available. If not, use the TCP stream and packet lengths as observations rather than claiming that the command contents were decoded.

**Important:** the no-auth container is deliberately limited to a disposable loopback lab. Do not bind it to `0.0.0.0`, use real credentials/data, or expose it to a LAN. Production MongoDB deployments should use appropriate authentication, authorization, network restrictions, and TLS. With TLS enabled, payload contents are encrypted unless you have an authorized decryption setup.

### 4.4 Cleanup

```powershell
docker rm -f wireshark-mongo-lab
```

The image remains cached. Remove it only if no other local work needs it.

**Questions:** Can you identify the TCP connection? Does the dissector recognize MongoDB messages? What is visible when traffic is unencrypted versus TLS-protected?

---

## Lab 5 — Wiresharking Server-Sent Events (SSE)

### Goal

Inspect an HTTP response that remains open and delivers event data over time. SSE is a text-based application protocol carried over HTTP; it is not a separate transport protocol.

### 5.1 Start a local SSE server

Save as `sse_server.py` in a temporary lab folder:

```python
from http.server import BaseHTTPRequestHandler, HTTPServer
import time

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path != "/events":
            self.send_error(404)
            return
        self.send_response(200)
        self.send_header("Content-Type", "text/event-stream")
        self.send_header("Cache-Control", "no-cache")
        self.send_header("Connection", "keep-alive")
        self.end_headers()
        try:
            for number in range(1, 11):
                payload = f"id: {number}\\nevent: tick\\ndata: message {number}\\n\\n"
                self.wfile.write(payload.encode("utf-8"))
                self.wfile.flush()
                time.sleep(1)
        except (BrokenPipeError, ConnectionResetError):
            pass

    def log_message(self, fmt, *args):
        print(fmt % args)

HTTPServer(("127.0.0.1", 8090), Handler).serve_forever()
```

Run in Terminal 1:

```powershell
python .\sse_server.py
```

### 5.2 Connect and capture

Start a Wireshark capture on the Npcap loopback adapter. In Terminal 2, run:

```powershell
curl.exe -N -i http://127.0.0.1:8090/events
```

The `-N` option disables curl's output buffering. The server sends ten events one second apart, then closes the response. Apply `tcp.port == 8090` and, if Wireshark dissects it as HTTP, `http`. Follow the TCP stream to inspect the response headers and event records.

An SSE stream uses `Content-Type: text/event-stream`. Blank lines delimit events; fields such as `id:`, `event:`, and `data:` have defined meanings. The server may write each event separately, but TCP segmentation and Wireshark packet boundaries are not guaranteed to align with event boundaries.

**Questions:** Does the HTTP response finish immediately? How are events delimited? What changes if you stop curl early? How would TLS affect visibility for an HTTPS SSE stream?

---

## Troubleshooting

| Symptom | What to check |
| --- | --- |
| No localhost packets | Confirm the Npcap loopback adapter is installed and selected; send traffic while capture is running. |
| TCP works but no HTTP dissection | Confirm the port/protocol; use Follow TCP Stream or Decode As for known plaintext HTTP. |
| HTTP/2 remains encrypted | Confirm the browser supports `SSLKEYLOGFILE`, the log contains session secrets, and Wireshark points to that exact file. |
| MongoDB filter is unavailable | Use `tcp.port == 27017` and Follow TCP Stream; dissector support varies. |
| SSE appears as one long response | This is expected for a streaming response; inspect the stream and timing rather than expecting one response per event. |
| Docker host capture is confusing | Capture the Windows host-published loopback endpoint; container-internal traffic may not appear on the same Windows interface. |

## Acceptance checklist

- [ ] Captured and identified UDP datagrams on port 9999.
- [ ] Followed a TCP stream and identified an HTTP request/response.
- [ ] Explained what TLS hides and what a TLS session key log enables for HTTP/2.
- [ ] Inspected a local MongoDB connection without exposing it beyond loopback.
- [ ] Observed SSE response headers and multiple events in one streaming response.
- [ ] Removed the disposable MongoDB container and protected/deleted TLS key logs.
- [ ] Kept packet captures and secrets out of Git.

## References

- [Wireshark User's Guide](https://www.wireshark.org/docs/wsug_html_chunked/)
- [Wireshark display filter reference](https://www.wireshark.org/docs/dfref/)
- [Npcap](https://npcap.com/)
- [Wireshark TLS decryption documentation](https://wiki.wireshark.org/TLS)
- [MongoDB wire protocol](https://www.mongodb.com/docs/manual/reference/mongodb-wire-protocol/)
- [Server-Sent Events standard](https://html.spec.whatwg.org/multipage/server-sent-events.html)
- [Docker port publishing](https://docs.docker.com/engine/network/port-publishing/)
