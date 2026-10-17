import { readFileSync } from 'node:fs';
import https from 'node:https';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const caPath = join(here, 'certs', 'dev-ca-cert.pem');

let ca;
try {
  ca = readFileSync(caPath);
} catch {
  console.error('Local CA certificate not found.');
  console.error('Run .\\generate-dev-certs.ps1 before running this client.');
  process.exit(1);
}

const request = https.get(
  {
    hostname: '127.0.0.1',
    port: 8443,
    path: '/',
    method: 'GET',
    ca,
    // Send localhost as SNI; the cert also includes 127.0.0.1 as an IP SAN.
    servername: 'localhost',
    timeout: 5000,
  },
  (response) => {
    console.log('HTTP status:', response.statusCode);
    console.log('HTTP version:', response.httpVersion);
    console.log('TLS version:', response.socket.getProtocol());
    console.log('Cipher:', response.socket.getCipher()?.name);

    response.setEncoding('utf8');
    let body = '';
    response.on('data', (chunk) => { body += chunk; });
    response.on('end', () => {
      console.log('\nResponse body:');
      console.log(body);
    });
  },
);

request.on('timeout', () => {
  request.destroy(new Error('HTTPS request timed out. Is the server running?'));
});

request.on('error', (error) => {
  console.error('HTTPS request failed:', error.message);
  console.error('Ensure https-server.mjs is running and the client trusts certs/dev-ca-cert.pem.');
  process.exitCode = 1;
});
