ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}

FROM --platform=${PLATFORM} ubuntu:26.04

ENV DEBIAN_FRONTEND="noninteractive"

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        "build-essential" \
        "debhelper" \
        "dpkg-dev" \
        "php8.1-cli" \
        "php8.1-dev" && \
    rm -rf /var/lib/apt/lists/*

COPY ./ext /tmp/src/ext
COPY ./build/ubuntu2204/debian /tmp/build/ubuntu2204/debian

COPY ./build/ubuntu2204/build.sh /usr/bin/build

ENTRYPOINT ["/usr/bin/build"]
