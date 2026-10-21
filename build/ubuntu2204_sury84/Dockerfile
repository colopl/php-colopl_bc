ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}

FROM --platform=${PLATFORM} ubuntu:22.04

ENV DEBIAN_FRONTEND="noninteractive"

# ppa.launchpadcontent.net keeps serving the index but answers 503 for pool/ under load
# (e.g. right after a PHP security release); the legacy ppa.launchpad.net host serves the
# same archive. It is HTTP only, but apt still verifies the signed InRelease and hashes.
RUN echo 'Acquire::Retries "10";' > /etc/apt/apt.conf.d/80-retries && \
    apt-get update && \
    apt-get install -y \
        "software-properties-common" && \
    add-apt-repository ppa:ondrej/php -y && \
    sed -i 's#https://ppa.launchpadcontent.net/#http://ppa.launchpad.net/#' /etc/apt/sources.list.d/ondrej-ubuntu-php-*.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        "build-essential" \
        "debhelper" \
        "dpkg-dev" \
        "php8.4-cli" \
        "php8.4-dev" && \
    rm -rf /var/lib/apt/lists/*

COPY ./ext /tmp/src/ext

COPY ./build/ubuntu2204_sury84/debian /tmp/build/ubuntu2204_sury84/debian

COPY ./build/ubuntu2204_sury84/build.sh /usr/bin/build

ENTRYPOINT ["/usr/bin/build"]
