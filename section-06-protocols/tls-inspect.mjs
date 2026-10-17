import tls from 'node:tls';
import { isIP } from 'node:net';

const host = process.argv[2] ?? 'example.com';
const port = Number(process.argv[3] ?? 443);

if (!Number.isInteger(port) || port < 1 || port > 65535) {
  console.error('Usage: node tls-inspect.mjs [hostname] [port]');
  process.exit(1);
}

console.log(`Connecting to ${host}:${port} with certificate validation enabled...`);

const options = {
  host,
  port,
  rejectUnauthorized: true,
  ...(isIP(host) ? {} : { servername: host }),
};

const socket = tls.connect(options);

socket.setTimeout(10000, () => {
  console.error('TLS connection timed out after 10 seconds.');
  socket.destroy(new Error('TLS connection timeout'));
});

socket.once('secureConnect', () => {
  const certificate = socket.getPeerCertificate();

  console.log('\nTLS handshake successful');
  console.log('Authorized:', socket.authorized);
  console.log('Protocol:', socket.getProtocol());
  console.log('Cipher:', socket.getCipher());
  console.log('Remote endpoint:', `${socket.remoteAddress}:${socket.remotePort}`);
  console.log('Certificate subject:', certificate.subject);
  console.log('Certificate issuer:', certificate.issuer);
  console.log('Valid from:', certificate.valid_from);
  console.log('Valid to:', certificate.valid_to);
  console.log('Subject Alternative Name:', certificate.subjectaltname);
  console.log('SHA-256 fingerprint:', certificate.fingerprint256);

  socket.end();
});

socket.once('error', (error) => {
  console.error('\nTLS connection failed:', error.message);
  if (error.code) console.error('Error code:', error.code);
  process.exitCode = 1;
});
