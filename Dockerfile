FROM ubuntu:noble

WORKDIR /app

COPY ./Go /app/Go
COPY ./Python /app/Python
COPY ./dockerBuild.sh /app
COPY ./dockerRemove.sh /app
COPY ./dockerRun.sh /app
COPY ./donp.sh /app
COPY ./modbusAscii.json /app
COPY ./modbusRtu.json /app


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