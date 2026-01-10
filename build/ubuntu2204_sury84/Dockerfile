ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}

FROM --platform=${PLATFORM} ubuntu:24.04

ENV DEBIAN_FRONTEND="noninteractive"

RUN apt-get update && \
    apt-get install -y \
        "software-properties-common" && \
    add-apt-repository ppa:ondrej/php -y && \
    apt-get update && \
    apt-get install -y \
        "php8.4" "php8.4-dev" "checkinstall"

COPY ./ext /tmp/ext

COPY ./build/ubuntu2204_sury84/build.sh /usr/bin/build

ENTRYPOINT ["/usr/bin/build"]
