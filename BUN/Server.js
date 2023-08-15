Bun.serve({
    port: 8080,
    fetch(req) {
        const url = new URL(req.url);
        const path = url.pathname;
        return new Response(`Hello World\nRequest: ${path}`, {
            status: 200,
            headers: {
                'Content-Type': 'text/plain',
            },
        });
    },
})

console.log('Listening on port 8080');