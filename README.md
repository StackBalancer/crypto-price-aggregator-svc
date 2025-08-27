# Crypto Price Aggregator Service

A minimal microservice written in C language that fetches live Bitcoin price from Kraken API and serves it over HTTP API. It also provides a Prometheus /metrics endpoint for monitoring.

## Features

- Simple HTTP API (/) → returns BTC price.
- Dockerized microservice
- Unit tests
- Static analysis via `cppcheck`
- CI/CD with GitHub Actions
- Terraform (AWS deploy)
- Prometheus metrics endpoint:
  - requests_total → count of HTTP requests.
  - btc_price_usd → current BTC price (updated on request).

## Build & Run

### Local Development

To build and run locally, you need the following dependencies installed on your host:

- `libprom`
- `libpromhttpd`
- `libmicrohttpd-dev`
- `libcjson-dev`
- `libcurl4-openssl-dev`
- `build-essential`

Then you can build and run:

```bash
make run
./crypto_service
# Service runs at http://localhost:8080 and http://localhost:8080/metrics
```

### Run static analysis

```bash
make static-check
```

### Run unit tests

```bash
make test
```

### Buid & run in container (Docker/Podman)

```bash
podman build -t crypto-service .
podman run -p 8080:8080 crypto-service

# Static check
podman build -f Dockerfile.test -t crypto-service:test .
podman run --rm crypto-service:test make static-check

# Unit tests
podman build -f Dockerfile.test -t crypto-service:test .
podman run --rm crypto-service-sqr:test make test
```

## Deployment

### Prerequisites
- AWS credentials with permissions for ECS, ECR, IAM, and VPC.
- Terraform CLI installed.

### Deploying manually
1. Set environment variables on your Linux terminal or via GitHub secrets (CI/CD):

```bash
export AWS_ACCESS_KEY_ID="your_access_key"
export AWS_SECRET_ACCESS_KEY="your_secret_key"
export AWS_REGION="us-east-1"
```

2. Initialize Terraform:
```bash
cd terraform
terraform init
```

3. Apply Terraform configuration:
```bash
terraform apply -auto-approve
```

3. Once deployed, find the service IP in AWS ECS Console and test::
```bash
curl http://<TASK_PUBLIC_IP>:8080
```

4. Cleanup:
```bash
cd terraform
terraform destroy -auto-approve
```

### CI/CD
CI/CD microservice build, test and deployment to AWS ECS is automated via GitHub Actions with pipeline definition stored in [ci-cd.yml](.github/workflows/ci-cd.yml) file.

> [!NOTE]  
> Requires an IAM User created in AWS for CI/CD with restricted permissions and AWS Credentials added to GitHub.

## 🔍 Monitoring & Metrics

This project includes a `/metrics` endpoint (Prometheus format) exposed by the crypto service.

### Run Monitoring Stack

```bash
# Build service (Podman or Docker)
podman-compose build --no-cache crypto-service
podman-compose up -d

# Test API
curl -s http://localhost:8080/
# {"BTC_USD": 109749.80}

# Test Metrics
curl http://localhost:8080/metrics | grep crypto
# crypto_requests_total 1
# crypto_btc_usd 109749.80

# Go to Prometheus UI → http://localhost:9090/targets and check that crypto-service target is UP.
```

### Grafana Setup

* Open Grafana (http://localhost:3000):

  - Username: \<username\>

  - Password: \<password\>

* Click Create → Import.

* Upload [grafana-dashboard.json](grafana-dashboard.json).

* Select Prometheus as the data source (http://prometheus:9090).

* Dashboard panels:

  - Gauge: Current BTC/USD price (crypto_btc_usd)

  - Graph: Total requests over time (crypto_requests_total)
