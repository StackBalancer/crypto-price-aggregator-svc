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

## Deployment

### Prerequisites
- AWS credentials with permissions for ECS, ECR, IAM, and VPC.
- Terraform CLI installed.

### Deploying
1. Set environment variables on your Linux terminal or via GitHub secrets:

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
