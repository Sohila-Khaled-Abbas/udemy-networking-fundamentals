const http = require('node:http');

const HOST = '127.0.0.1';
const PORT = 8080;

const httpServer = http.createServer();

httpServer.on('request', (req, res) => {
  console.log(`[${new Date().toISOString()}] ${req.method} ${req.url}`);

  res.writeHead(200, {
    'Content-Type': 'text/plain; charset=utf-8',
    'Cache-Control': 'no-store',
  });

  res.end('OK!\n');
});

httpServer.on('listening', () => {
  const address = httpServer.address();

  console.log(`HTTP server listening at http://${HOST}:${address.port}`);
  console.log('Press Ctrl+C to stop the server.');
});

httpServer.on('error', (error) => {
  console.error('HTTP server error:', error.message);

  if (error.code === 'EADDRINUSE') {
    console.error(`Port ${PORT} is already in use. Stop the other process or change PORT.`);
  }

  process.exitCode = 1;
});

// Listen locally for this practice lab.
httpServer.listen(PORT, HOST);