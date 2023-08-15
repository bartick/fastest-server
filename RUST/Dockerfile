FROM rust:1.71.1-buster

COPY . .

EXPOSE 8080

RUN cargo build --release

CMD ["./target/release/rust"]