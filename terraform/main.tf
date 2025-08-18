provider "aws" {
  region = var.aws_region
}

# Create ECR repo to store Docker images
resource "aws_ecr_repository" "crypto_service" {
  name = "crypto-service"
}

# Create ECS cluster
resource "aws_ecs_cluster" "crypto_service" {
  name = "crypto-service-cluster"
}

# IAM role for ECS task execution
resource "aws_iam_role" "ecs_task_execution_role" {
  name = "ecsTaskExecutionRole"

  assume_role_policy = jsonencode({
    Version = "2012-10-17"
    Statement = [{
      Action    = "sts:AssumeRole"
      Effect    = "Allow"
      Principal = {
        Service = "ecs-tasks.amazonaws.com"
      }
    }]
  })
}

resource "aws_iam_role_policy_attachment" "ecs_task_execution_policy" {
  role       = aws_iam_role.ecs_task_execution_role.name
  policy_arn = "arn:aws:iam::aws:policy/service-role/AmazonECSTaskExecutionRolePolicy"
}

# ECS Task Definition
resource "aws_ecs_task_definition" "crypto_service" {
  family                   = "crypto-service"
  network_mode             = "awsvpc"
  requires_compatibilities = ["FARGATE"]
  cpu                      = "256"
  memory                   = "512"

  execution_role_arn = aws_iam_role.ecs_task_execution_role.arn

  container_definitions = jsonencode([
    {
      name      = "crypto-service"
      image     = "${aws_ecr_repository.crypto_service.repository_url}:latest"
      essential = true
      portMappings = [{
        containerPort = 8080
        hostPort      = 8080
      }]
    }
  ])
}

# ECS Service (runs the task)
resource "aws_ecs_service" "crypto_service" {
  name            = "crypto-service"
  cluster         = aws_ecs_cluster.crypto_service.id
  task_definition = aws_ecs_task_definition.crypto_service.arn
  desired_count   = 1
  launch_type     = "FARGATE"

  network_configuration {
    subnets         = var.subnets
    security_groups = [var.security_group]
    assign_public_ip = true
  }
}
