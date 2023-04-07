# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Update package lists and install necessary dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    cmake \
    curl \
    git \
    g++ \
    make \
    pkg-config \
    unzip \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /workspace

# Fetch the latest release tag, clone and build gRPC
RUN LATEST_GRPC_RELEASE_TAG=$(curl --silent "https://api.github.com/repos/grpc/grpc/releases/latest" | grep -Po '"tag_name": "\K.*?(?=")') && \
    git clone --depth 1 -b ${LATEST_GRPC_RELEASE_TAG} https://github.com/grpc/grpc && \
    cd grpc && \
    git submodule update --init && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
          -DgRPC_BUILD_TESTS=OFF \
          -DBUILD_SHARED_LIBS=ON \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          ../.. && \
    make -j$(nproc) && \
    make install && \
    cd /workspace && \
    rm -rf grpc

RUN ldconfig

# create entrypoint
COPY docker-entrypoint.sh /home/dev/docker-entrypoint.sh
RUN chmod +x /home/dev/docker-entrypoint.sh

ENTRYPOINT ["/home/dev/docker-entrypoint.sh"]

# default is bash anyway
CMD ["bash"]
