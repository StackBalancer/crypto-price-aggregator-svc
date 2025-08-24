FROM debian:stable-slim

# Install dependencies
RUN apt-get update && apt-get install -y \
    gcc make curl libcurl4-openssl-dev \
    libcjson-dev libmicrohttpd-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY Makefile .
COPY src/ src/

RUN make

# Create container user
RUN useradd -r -s /bin/false crypto-svc
USER crypto-svc

EXPOSE 8080
CMD ["./crypto_service"]
