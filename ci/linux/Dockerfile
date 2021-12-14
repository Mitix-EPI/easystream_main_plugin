FROM ubuntu:latest

RUN ln -snf /usr/share/zoneinfo/$CONTAINER_TIMEZONE /etc/localtime && echo $CONTAINER_TIMEZONE > /etc/timezone

RUN apt-get update && apt-get install -y \
            g++-multilib \
            gcc-multilib \
            git \
            wget \
            software-properties-common

WORKDIR /tmp

COPY *.sh ./

RUN sh -c "cat install-dependencies-ubuntu.sh | sed 's/sudo//g' | sh"

WORKDIR /mnt

CMD ["sh", "-c", "cat ./ci/linux/build-ubuntu.sh | sed 's/sudo//g' | sh"]