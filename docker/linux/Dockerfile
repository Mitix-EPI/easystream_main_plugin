FROM ubuntu:latest

WORKDIR /usr/src/app/ubuntuApp

ARG DEBIAN_FRONTEND=noninteractive

ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

RUN apt-get update && apt-get install -y \
  autoconf \
  automake \
  autopoint \
  build-essential \
  g++-multilib \
  gcc-multilib \
  git \
  wget \
  sudo software-properties-common

COPY ./plugin/ ./

RUN ./ci/linux/install-dependencies-ubuntu.sh

CMD ["./ci/linux/build-ubuntu.sh"]
