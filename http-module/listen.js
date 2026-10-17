const http = require('http');
const httpServer = http.createServer();
httpServer.on('request', (req, res) => {
    res.end('OK!');
})
httpServer.on('listening', () => {
    console.log(`listening on port: http://localhost:${httpServer.address().port}`)
})
httpServer.listen(8080);