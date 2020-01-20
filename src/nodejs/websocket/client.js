const WebSocket = require('ws');
const iconv = require('iconv-lite');

const ws = new WebSocket('ws://localhost:18888/');

ws.on('open', function () {
    const s = JSON.stringify({name: '小菜'});
    const b = iconv.encode(s, 'gbk');
    ws.send(b);
});
