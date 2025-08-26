FROM debian:stable-slim

# Install dependencies
RUN apt-get update && apt-get install -y \
    gcc make cmake curl libcurl4-openssl-dev \
    libcjson-dev libmicrohttpd-dev build-essential \
    git cppcheck \
    && rm -rf /var/lib/apt/lists/*

# Build libprom
RUN git clone https://github.com/digitalocean/prometheus-client-c.git /tmp/libprom \
    && cd /tmp/libprom/prom && \
    mkdir build && cd build && \
    cmake .. \
    make && \
    make install && \
    cd && \
    rm -rf /tmp/libprom && \
    ldconfig

# Build libpromhttp
RUN git clone https://github.com/digitalocean/prometheus-client-c.git /tmp/libpromhttp \
    && cd /tmp/libpromhttp/promhttp && \
    # Patch promhttp_handler signature
    sed -i 's/^int promhttp_handler(/enum MHD_Result promhttp_handler(/' src/promhttp.c && \
    mkdir build && cd build && \
    cmake .. && \
    make && \
    make install && \
    cd && \
    rm -rf /tmp/libpromhttp && \
    ldconfig

WORKDIR /app

COPY Makefile .
COPY src/ src/

RUN make

# Create container user
RUN useradd -r -s /bin/false crypto-svc
USER crypto-svc

EXPOSE 8080
CMD ["./crypto_service"]
