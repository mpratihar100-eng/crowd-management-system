FROM ubuntu:22.04
RUN apt-get update && apt-get install -y build-essential cmake
WORKDIR /app
COPY . .
RUN cmake . && make
CMD ["./cv_worker"]
