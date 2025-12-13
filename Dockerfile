FROM ubuntu:latest 

WORKDIR /app

COPY . /app

RUN apt-get update && apt-get install -y golang python3 python3-pip bash bc

RUN cd /app/Go && go mod tidy

RUN cd /app/Go && go build -o donp ./app/donp.go

RUN cd ../../

RUN chmod +x /app/donp.sh

CMD ["/app/donp.sh"]