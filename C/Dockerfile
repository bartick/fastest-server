FROM gcc:13.2.0

COPY . .

EXPOSE 8080

RUN gcc server.c -o server

CMD ["./server"]