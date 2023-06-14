ARG IMAGE=php
ARG TAG=8.2-cli

FROM ${IMAGE}:${TAG}

COPY --from=composer:2 /usr/bin/composer /usr/bin/composer

RUN apt-get update && apt-get install -y "git" "unzip"

COPY ./build/library/test.sh /usr/bin/test

ENTRYPOINT ["/usr/bin/test"]

WORKDIR "/workspace"
