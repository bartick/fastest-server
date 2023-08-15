const http = require('http');

http.createServer(function (_req, res) {
    res.write('Hello World\n'); //write a response to the client
    res.write('Request: ' + _req.url + '\n')
    res.end(); //end the response
}).listen(8080, () => {
    console.log('Server running at port 8080');
}); //the server object listens on port 8080