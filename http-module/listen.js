const http = require('http');
const httpServer = http.createServer();
httpServer.on('request', (req, res) => {
    res.end('OK!');
})
httpServer.on('listening', () => {
    console.log(`listening on port http://localhost:${httpServer.address().port} ${httpServer.address().address}`);
})

// :: ipv6 ==> 0.0.0.0 ipv4
// unspecific address
//invalid address
httpServer.listen(0, 'localhost' );