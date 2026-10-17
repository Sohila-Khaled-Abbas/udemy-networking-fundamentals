# Node.js HTTP Module Lab

This lab demonstrates the built-in Node.js `http` module. It creates an HTTP server, listens for incoming requests, returns a plain-text `OK!` response, and logs each request method and URL.

## Requirements

- Node.js (no third-party npm dependencies)
- PowerShell or another terminal

Verify Node.js is installed:

```powershell
node --version
npm --version
```

## Run the server on Windows

From the repository root:

```powershell
Set-Location .\http-module
npm start
```

You can also run it directly without npm:

```powershell
node .\listen.js
```

Expected startup output:

```text
HTTP server listening at http://127.0.0.1:8080
Press Ctrl+C to stop the server.
```

Leave this terminal open while testing.

## Send an HTTP request

Open a second PowerShell window. Test the TCP listener:

```powershell
Test-NetConnection 127.0.0.1 -Port 8080
```

Look for `TcpTestSucceeded : True`. This checks TCP connectivity only, not the HTTP response.

Send a GET request and inspect the status and body:

```powershell
$response = Invoke-WebRequest -Uri 'http://127.0.0.1:8080/'
$response.StatusCode
$response.Content
```

Expected output:

```text
200
OK!
```

Or inspect the response headers and body using curl:

```powershell
curl.exe -i http://127.0.0.1:8080/
```

The server returns the same `200 OK` plain-text response for any request path or method. For example:

```powershell
Invoke-WebRequest -Method Post -Uri 'http://127.0.0.1:8080/demo' -Body 'sample body'
```

The server log in Terminal 1 will show the method and path, such as `POST /demo`.

## Understand the implementation

In `listen.js`:

- `require('node:http')` imports Node.js's built-in HTTP module; no package installation is needed.
- `http.createServer()` creates the server.
- The `request` event fires when an HTTP request is received. The callback can inspect `req.method` and `req.url`.
- `res.writeHead(200, ...)` sets the status and headers.
- `res.end('OK!\\n')` completes the response. Calling `end()` is important: otherwise the client may keep waiting for the response to finish.
- The `listening` event confirms that the server has successfully bound to its address and port.
- The `error` handler gives a useful message if the port is already occupied.

## HTTP is not HTTPS

This example uses ordinary HTTP: the response is not encrypted and there is no TLS certificate. Use it to learn request/response behavior; use the separate [Section 6 HTTPS lab](../section-06-protocols/README.md) to study TLS, certificates, and verified HTTPS.

## Why bind to 127.0.0.1?

The server explicitly binds to `127.0.0.1`, so it accepts local connections on this computer rather than listening on every network interface. For this lab, use `http://127.0.0.1:8080/`.

## Troubleshooting

- **`EADDRINUSE` / port already in use:** close the other program listening on port 8080, or change `PORT` in `listen.js` and update every test command to match.
- **`TcpTestSucceeded : False`:** check that `npm start` is still running in Terminal 1 and that the test uses port 8080.
- **Browser or PowerShell can't connect:** make sure the URL is `http://127.0.0.1:8080/` (not `https://`) and that the server startup message appeared.
- Stop the server with **Ctrl+C** in Terminal 1.

Keep this server for local practice. Do not treat this minimal example as a production-ready web service.
