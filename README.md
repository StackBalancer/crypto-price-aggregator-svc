# Crypto Price Aggregator Service

A minimal microservice written in C that fetches live Bitcoin price from the Kraken API and serves it over HTTP. It also exposes a Prometheus `/metrics` endpoint for monitoring.

## Features

- Simple HTTP API (`/`) returning the current BTC/USD price with error handling
- Dockerized microservice
- Unit tests and static analysis via `cppcheck`
- CI/CD pipeline with GitHub Actions
- Terraform-based deployment to AWS ECS
- Prometheus metrics endpoint:
  - `crypto_requests_total` - count of HTTP requests
  - `crypto_btc_usd` - current BTC/USD price (updated on each request)
- Monitoring stack with Prometheus and Grafana

## Changelog

### v0.3.0 (Latest)
- Added monitoring and metrics stack with Prometheus and Grafana
- Added container security improvements (non-root user)
- Added static and dynamic analysis in CI pipeline
- Improved error handling for API failures and null pointer checks
- Fixed robust smoke tests and ECR repository cleanup

### v0.2.0
- Added Prometheus metrics endpoint (`/metrics`)
- Added Docker containerization
- Added AWS ECS deployment with Terraform
- Added CI/CD pipeline with GitHub Actions

### v0.1.0
- Initial basic HTTP server serving BTC price from Kraken API
- Added unit tests and static analysis

## Build and Run

### Local Development

You will need the following dependencies installed on your host:

- `libprom`
- `libpromhttp`
- `libmicrohttpd-dev`
- `libcjson-dev`
- `libcurl4-openssl-dev`
- `build-essential`

Then build and run:

```bash
make run
# Service runs at http://localhost:8080 and http://localhost:8080/metrics
```

Run static analysis:

```bash
make static-check
```

Run unit tests:

```bash
make test
```

### Build and Run in Container (Docker/Podman)

```bash
podman build -t crypto-service .
podman run -p 8080:8080 crypto-service

# Static check
podman build -f Dockerfile.test -t crypto-service:test .
podman run --rm crypto-service:test make static-check

# Unit tests
podman build -f Dockerfile.test -t crypto-service:test .
podman run --rm crypto-service:test make test
```

## Deployment

### Prerequisites

- AWS credentials with permissions for ECS, ECR, IAM, and VPC
- Terraform CLI installed

### Deploying Manually

1. Set environment variables:

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

3. Apply the configuration:

```bash
terraform apply -auto-approve
```

4. Find the service IP in the AWS ECS Console and test:

```bash
curl http://<TASK_PUBLIC_IP>:8080
```

5. Cleanup:

```bash
cd terraform
terraform destroy -auto-approve
```

### CI/CD

Build, test, and deployment to AWS ECS is automated via GitHub Actions. The pipeline definition is in [ci-cd.yml](.github/workflows/ci-cd.yml).

Note: requires an IAM user created in AWS with restricted permissions and credentials added to GitHub secrets.

## Monitoring and Metrics

The service exposes a `/metrics` endpoint in Prometheus format.

### Running the Monitoring Stack

```bash
podman-compose build --no-cache crypto-service
podman-compose up -d

# Test API
curl -s http://localhost:8080/
# {"BTC_USD": 109749.80}

# Test metrics
curl http://localhost:8080/metrics | grep crypto
# crypto_requests_total 1
# crypto_btc_usd 109749.80
```

Check that the crypto-service target is up in the Prometheus UI at http://localhost:9090/targets.

### Grafana Setup

1. Open Grafana at http://localhost:3000 (username: `admin`, password: `admin`)
2. Go to Create > Import and upload [grafana-dashboard.json](grafana-dashboard.json)
3. Select Prometheus as the data source (`http://prometheus:9090`)

Dashboard panels:
- Gauge showing the current BTC/USD price (`crypto_btc_usd`)
- Graph showing total requests over time (`crypto_requests_total`)
