# Use multi-stage build to optimize final image size
# First stage: Build Rust application
FROM rust:slim-bookworm AS builder
WORKDIR /app/Rust
COPY ./Rust /app/Rust
RUN cargo build --release

# Second stage: Final image setup
FROM ubuntu:noble AS donpimage

# Create a group and non-root user and ensure non-root user has access
RUN groupadd --gid 1183 donpgroup && useradd --uid 1183 --gid donpgroup --shell /bin/bash --create-home donpuser && \
    mkdir /app && \
    chown -R donpuser:donpgroup /app

# Turn interactive prompts off
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages including language runtimes and build tools
RUN apt-get update && \
    apt-get install -y \
    unzip \
    curl \
    golang \
    python3 \
    python3-pip && \
    rm -rf /var/lib/apt/lists/ && \
    apt-get clean

# Set Working Directory
WORKDIR /app   

# Selective copy operations
COPY ./C++ /app/C++
COPY --from=builder /app/Rust/target/release /app/Rust/target/release
COPY ./Go /app/Go
COPY ./Python /app/Python
COPY ./dockerBuild.sh /app
COPY ./dockerRemove.sh /app
COPY ./dockerRun.sh /app
COPY ./donp.sh /app
COPY ./modbusAscii.json /app
COPY ./modbusRtu.json /app

# Build remaining applications, file preparation, and permission settings
RUN cd /app/C++ && \
    ./build.sh && \
    cd ../../ && \
    cd /app/Go && \
    go mod tidy && \
    go build -o donp ./app/donp.go && \
    cd ../../ && \
    chmod +x /app/donp.sh && \
    touch /app/Python/output.log && \
    chmod g=rx /app/Python/output.log && \
    touch /app/Go/output.log && \
    chmod g=rx /app/Go/output.log && \
    touch /app/Rust/output.log && \
    chmod g=rx /app/Rust/output.log && \
    touch /app/C++/output.log && \
    chmod g=rx /app/C++/output.log && \
    chown -R donpuser:donpgroup /app

# Switch to non-root user
USER donpuser

CMD ["/app/donp.sh"]