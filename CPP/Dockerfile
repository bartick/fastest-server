FROM gcc:13.2.0

COPY . .

EXPOSE 8080

RUN g++ server.cpp -o server

CMD ["./server"]