const WebSocket = require('ws');

const server = new WebSocket.Server({port: 18888});

server.on('connection', function (ws) {
    ws.on('messge', function (msg) {
        console.log('received', msg)
    });
});
