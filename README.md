# Protobuf/GRPC with CMake Playground

This is a Protobuf playground 
Including CMake together with gRPC in C++

## Docker
docker build -t cpp-grpc-dev:latest .
docker run -d --name grpc_playground -v /mnt/c/Users/are/source/repos/K2ePlayground:/src cpp-grpc-dev:latest "local"

## Build
within container

make target

