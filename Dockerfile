FROM ubuntu:noble

WORKDIR /app

COPY . /app

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
    chmod +x /app/donp.sh

CMD ["/app/donp.sh"]