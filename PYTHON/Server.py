# create a http server in python

import http.server

from http import HTTPStatus

# create a handler class
class MyHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=None, **kwargs)
        self.content = ""

    # handle get request
    def do_GET(self):
        self.content = "Hello World\n"
        self.content += "Request: " + self.path + "\n"
        self.send_head()
        self.wfile.write(self.content.encode())
    
    # serve the head
    def do_HEAD(self):
        self.send_head()
    
    # handle head
    def send_head(self):
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-type", "text/plain")
        self.send_header("Content-Length", str(len(self.content)))
        self.end_headers()


# create a http server
server = http.server.HTTPServer(('', 8080), MyHandler)


if __name__ == "__main__":
    # start the server
    try:
        print("Starting server at http://localhost:8080")
        server.serve_forever()
    except KeyboardInterrupt:
        print("Shutting down server")
        server.socket.close()