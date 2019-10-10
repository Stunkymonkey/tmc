FROM ubuntu:bionic

ARG boost_version=1.71.0
ARG boost_version_u=1_71_0
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
      libtool \
      sudo \
      wget \
 && true

RUN wget -q https://dl.bintray.com/boostorg/release/${boost_version}/source/boost_${boost_version_u}.tar.gz -O - \
 | tar xvz \
 && cd boost_${boost_version_u} \
 && ./bootstrap.sh \
 && ./b2 --with-system --with-iostreams --with-program_options --with-serialization -j$(nproc) \
 && ./b2 --with-system --with-iostreams --with-program_options --with-serialization -j$(nproc) install \
 && cd / \
 && rm -rf boost_${boost_version} \
 && true


RUN git clone https://github.com/stunkymonkey/tmc /srv/tmc

RUN cd /srv/tmc \
 && ./autogen.sh --prefix=/srv/rds/ \
 && make -j \
 && true
