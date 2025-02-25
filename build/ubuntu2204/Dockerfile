ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}

FROM --platform=${PLATFORM} ubuntu:22.04

RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y "php" "php-dev" "checkinstall"

COPY ./ext /tmp/ext

COPY ./build/ubuntu2204/build.sh /usr/bin/build

ENTRYPOINT ["/usr/bin/build"]
