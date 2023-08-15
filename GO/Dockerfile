FROM golang:1.20-alpine3.17

WORKDIR /app

COPY . .

EXPOSE 8080

RUN go build -o Server .

CMD ["go", "run", "."]