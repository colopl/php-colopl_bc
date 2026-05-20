ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}

FROM --platform=${PLATFORM} ubuntu:26.04

ENV DEBIAN_FRONTEND="noninteractive"

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        "build-essential" \
        "debhelper" \
        "dpkg-dev" \
        "php8.5-cli" \
        "php8.5-dev" && \
    rm -rf /var/lib/apt/lists/*

COPY ./ext /tmp/src/ext
COPY ./build/ubuntu2604/debian /tmp/build/ubuntu2604/debian

COPY ./build/ubuntu2604/build.sh /usr/bin/build

ENTRYPOINT ["/usr/bin/build"]
