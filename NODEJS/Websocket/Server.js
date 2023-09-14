const http = require('http');
const WebSocket = require('ws');

// Create an HTTP server
const httpServer = http.createServer((_req, res) => {
    res.write('Hello World\n'); //write a response to the client
    res.write('Request: ' + _req.url + '\n')
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end(); //end the response
});

// Create a WebSocket server by passing the HTTP server instance
const wss = new WebSocket.Server({ server: httpServer });

// WebSocket server event handling
wss.on('connection', (ws) => {
  console.log('WebSocket client connected');

  // subscribe to some topic (for example purposes)
  ws.send('sub:topic1');

  // Handle WebSocket messages
  ws.on('message', (message) => {
    console.log(`Received: ${message}`);
    // You can send messages back to the WebSocket client here
    // For example: ws.send('You said: ' + message);
  });

  // Handle WebSocket close event
  ws.on('close', () => {
    console.log('WebSocket client disconnected');
  });
});

wss.on('error', (err) => {
    console.log('WebSocket server error: ', err);
});

const port = 8080;
const host = 'localhost';

httpServer.listen(port, host, () => {
  console.log(`HTTP server is running at http://${host}:${port}/`);
});

function sendToAll(message) {
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(message);
    }
  });
}

process.on('SIGINT', () => {
    console.log('Received SIGINT signal. Stopping the application gracefully...');

    
    // You can perform cleanup or other actions here
    
    // Exit the application
    process.exit(0);
});

// call sendToAll every minute
setInterval(() => {
    sendToAll(Date.now());
}, 6000);
