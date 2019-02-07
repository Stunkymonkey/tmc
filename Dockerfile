FROM ubuntu:cosmic

RUN export DEBIAN_FRONTEND=noninteractive \
 && apt-get update \
 && apt-get install -y \
      autoconf \
      build-essential \
      git \
      libboost-all-dev \
      libboost-iostreams-dev \
      libpqxx-dev \
      libtool \
      postgis \
      postgresql \
 && true
      #libboost1.68-dev \
      #libboost-iostreams-dev \

RUN git clone https://github.com/stunkymonkey/tmc /srv/tmc

ENV LD_LIBRARY_PATH=/usr/local/lib

RUN true \
 && cd /srv/tmc \
 && ./autogen.sh --prefix=/srv/rds/ \
 && make -j \
 && true
