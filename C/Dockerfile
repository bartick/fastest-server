FROM gcc:13.2.0

COPY . .

RUN gcc server.c -o server

CMD ["./server"]