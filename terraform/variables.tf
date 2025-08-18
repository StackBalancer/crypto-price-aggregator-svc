variable "aws_region" {
  default = "us-east-1"
}

variable "subnets" {
  type = list(string)
}

variable "security_group" {
  type = string
}

variable "subnets" {
  default = [aws_subnet.public.id]
}

variable "security_group" {
  default = aws_security_group.crypto_service_sg.id
}
