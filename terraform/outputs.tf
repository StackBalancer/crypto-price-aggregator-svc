output "repository_url" {
  description = "ECR repository URL for the crypto service container images"
  value       = aws_ecr_repository.crypto_service.repository_url
}
