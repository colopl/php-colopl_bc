ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}

FROM --platform=${PLATFORM} ubuntu:22.04

ENV DEBIAN_FRONTEND="noninteractive"

RUN apt-get update && \
    apt-get install -y \
        "software-properties-common" && \
    add-apt-repository ppa:ondrej/php -y && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        "build-essential" \
        "debhelper" \
        "dpkg-dev" \
        "php8.4-cli" \
        "php8.4-dev" && \
    rm -rf /var/lib/apt/lists/*

COPY ./ext /tmp/src/ext
COPY ./LICENSE /tmp/src/LICENSE
COPY ./README.md /tmp/src/README.md
COPY ./build/ubuntu2204_sury84/debian /tmp/build/ubuntu2204_sury84/debian

COPY ./build/ubuntu2204_sury84/build.sh /usr/bin/build

ENTRYPOINT ["/usr/bin/build"]
