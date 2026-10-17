import { readFileSync } from 'node:fs';
import { createServer } from 'node:https';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const certDir = join(here, 'certs');

let key;
let cert;

try {
  key = readFileSync(join(certDir, 'localhost-key.pem'));
  cert = readFileSync(join(certDir, 'localhost-cert.pem'));
} catch {
  console.error('Local TLS files are missing.');
  console.error('Run .\\generate-dev-certs.ps1 from PowerShell in this folder first.');
  process.exit(1);
}

const HOST = '127.0.0.1';
const PORT = 8443;

const server = createServer({ key, cert }, (req, res) => {
  const body = {
    message: 'Hello over HTTPS!',
    method: req.method,
    path: req.url,
    httpVersion: req.httpVersion,
    tlsProtocol: req.socket.getProtocol(),
    cipher: req.socket.getCipher()?.name,
    note: 'HTTP request data arrived inside a validated TLS connection.',
  };

  console.log(`${req.method} ${req.url} from ${req.socket.remoteAddress}`);
  res.writeHead(200, {
    'content-type': 'application/json; charset=utf-8',
    'cache-control': 'no-store',
    'x-content-type-options': 'nosniff',
  });
  res.end(JSON.stringify(body, null, 2));
});

server.on('error', (error) => {
  console.error('HTTPS server error:', error.message);
  if (error.code === 'EADDRINUSE') {
    console.error(`Port ${PORT} is already in use. Stop the other process or change PORT.`);
  }
  process.exitCode = 1;
});

server.listen(PORT, HOST, () => {
  console.log(`HTTPS server listening on https://127.0.0.1:${PORT}`);
  console.log('Certificate validation remains enabled in the client; use the local CA for trust.');
  console.log('Press Ctrl+C to stop the server.');
});
