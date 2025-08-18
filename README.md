# Crypto Price Aggregator Service

A minimal microservice written in C language that fetches live Bitcoin price from Kraken API and serves it over HTTP using **libmicrohttpd**.

## Features
- Written in C (libcurl + cJSON + libmicrohttpd)
- Dockerized microservice
- Unit tests
- Static analysis via `cppcheck`
- CI/CD with GitHub Actions
- Terraform (AWS deploy)

## Build & Run locally

```bash
make run
# Service runs at http://localhost:8080
```

## Run static analysis

```bash
make test
```

## Run unit tests

```bash
make test
```

## Run in Docker (Podman)

```bash
docker build -t crypto-service .
docker run -p 8080:8080 crypto-service
```
