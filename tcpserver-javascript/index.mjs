import net from 'net';

const HOST = '127.0.0.1';
const PORT = 5500;

const server = net.createServer((socket) => {
  console.log(`TCP connection from ${socket.remoteAddress}:${socket.remotePort}`);
  socket.write('Hello client!\n');

  socket.on('data', (data) => {
    console.log('Received:', data.toString().trim());
  });

  socket.on('error', (error) => {
    console.error('Client socket error:', error.message);
  });
});

server.on('error', (error) => {
  console.error('TCP server error:', error.message);
  if (error.code === 'EADDRINUSE') {
    console.error(`Port ${PORT} is already in use. Stop the other server or choose another port.`);
  }
});

server.listen(PORT, HOST, () => {
  console.log(`TCP server listening on ${HOST}:${PORT}`);
});
