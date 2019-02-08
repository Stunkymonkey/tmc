FROM ubuntu:bionic

ARG boost_version=1_69_0
ARG leaflet_version=1.4.0
ARG leaflet_heat_version=0.2.0

RUN export DEBIAN_FRONTEND=noninteractive \
 && apt-get update \
 && apt-get install -y \
      autoconf \
      build-essential \
      bzip2 \
      git \
      libbz2-dev \
      libpqxx-dev \
      libtool \
      postgis \
      postgresql \
      sudo \
      wget \
 && true

RUN wget -q https://dl.bintray.com/boostorg/release/1.69.0/source/boost_${boost_version}.tar.gz -O - \
 | tar xvz \
 && cd boost_${boost_version} \
 && ./bootstrap.sh \
 && ./b2 --with-system --with-iostreams --with-program_options -j$(nproc) \
 && ./b2 --with-system --with-iostreams --with-program_options -j$(nproc) install \
 && cd / \
 && rm -rf boost_${boost_version} \
 && true


RUN git clone https://github.com/stunkymonkey/tmc /srv/tmc

RUN cd /srv/tmc \
 && ./autogen.sh --prefix=/srv/rds/ \
 && make -j \
 && true
