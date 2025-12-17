FROM ubuntu:noble

# Create a group and non-root user and ensure non-root user has access
RUN groupadd --gid 1183 donpgroup && useradd --uid 1183 --gid donpgroup --shell /bin/bash --create-home donpuser && \
    mkdir /app && \
    chown -R donpuser:donpgroup /app

# Working directory
WORKDIR /app

# Selective copy operations
COPY ./Go /app/Go
COPY ./Python /app/Python
COPY ./dockerBuild.sh /app
COPY ./dockerRemove.sh /app
COPY ./dockerRun.sh /app
COPY ./donp.sh /app
COPY ./modbusAscii.json /app
COPY ./modbusRtu.json /app

# Install dependencies, build applications, set permissions ensuring non-root user access
RUN apt-get update && \
    apt-get install -y \
    golang \
    python3 \
    python3-pip \
    bash \
    bc && \
    cd /app/Go && \
    go mod tidy && \
    go build -o donp ./app/donp.go && \
    cd ../../ && \
    chmod +x /app/donp.sh && \
    touch /app/Python/output.log && \
    chmod g=rx /app/Python/output.log && \
    touch /app/Go/output.log && \
    chmod g=rx /app/Go/output.log && \
    chown -R donpuser:donpgroup /app

# Switch to non-root user
USER donpuser

CMD ["/app/donp.sh"]