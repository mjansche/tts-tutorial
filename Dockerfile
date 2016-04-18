FROM ubuntu:trusty
MAINTAINER Martin Jansche <mjansche@google.com>
RUN apt-get update && apt-get install -y \
     autoconf \
     cpp \
     curl \
     g++ \
     gcc \
     git \
     libc-dev \
     libreadline-dev \
     libtool \
     make \
     ncurses-dev \
     python \
     python-dev \
     unzip \
     wget \
     zlib1g-dev
RUN mkdir /usr/local/tts

# Fetch libraries that will be installed in /usr/local
WORKDIR /usr/local/src
RUN wget http://openfst.org/twiki/pub/FST/FstDownload/openfst-1.5.2.tar.gz
RUN wget http://openfst.cs.nyu.edu/twiki/pub/GRM/ThraxDownload/thrax-1.2.2.tar.gz
RUN tar x --no-same-owner --no-same-permissions -zf openfst-1.5.2.tar.gz
RUN tar x --no-same-owner --no-same-permissions -zf thrax-1.2.2.tar.gz
RUN git clone https://github.com/google/protobuf.git
RUN git clone https://github.com/google/re2.git

# Build and install the OpenFst Library
WORKDIR /usr/local/src/openfst-1.5.2
RUN ./configure \
    --prefix=/usr/local \
    --with-gnu-ld \
    --enable-static \
    --enable-bin \
    --enable-compact-fsts \
    --enable-compress \
    --enable-const-fsts \
    --enable-far \
    --enable-mpdt \
    --enable-ngram-fsts \
    --enable-pdt \
    --enable-python
RUN make && make install
RUN ldconfig

# Build and install the Thrax Grammar Development Tools
WORKDIR /usr/local/src/thrax-1.2.2
RUN ./configure \
    --prefix=/usr/local \
    --with-gnu-ld \
    --enable-bin \
    --enable-readline
RUN make && make install
RUN ldconfig

# Build and install protobuf (Protocol Buffers)
WORKDIR /usr/local/src/protobuf
RUN git reset --hard ca9bbd71d547a05604e8c2bddda66cdba5abe0c4
RUN ./autogen.sh
RUN ./configure --prefix=/usr/local --with-gnu-ld
RUN make && make install
RUN ldconfig

# Bulid and install the re2 regular expression package
WORKDIR /usr/local/src/re2
RUN git reset --hard 4744450c4880b9445c57d9224495f0e8e29f1c4c
RUN make && make install
RUN ldconfig

# Download and prepare Festival & friends
WORKDIR /usr/local/tts
RUN wget http://festvox.org/packed/festival/2.4/festival-2.4-release.tar.gz
RUN wget http://festvox.org/packed/festival/2.4/speech_tools-2.4-release.tar.gz 
RUN wget http://festvox.org/festvox-2.7/festvox-2.7.0-release.tar.gz
RUN wget http://tts.speech.cs.cmu.edu/awb/sptk/SPTK-3.6.tar.gz
RUN tar x --no-same-owner --no-same-permissions -zf festival-2.4-release.tar.gz
RUN tar x --no-same-owner --no-same-permissions -zf speech_tools-2.4-release.tar.gz
RUN tar x --no-same-owner --no-same-permissions -zf festvox-2.7.0-release.tar.gz
RUN tar x --no-same-owner --no-same-permissions -zf SPTK-3.6.tar.gz
RUN patch -p0 < festvox/src/clustergen/SPTK-3.6.patch
ENV ESTDIR /usr/local/tts/speech_tools
ENV FESTVOXDIR /usr/local/tts/festvox
ENV SPTKDIR /usr/local

# Build and install SP-TK
WORKDIR /usr/local/tts/SPTK-3.6
RUN ./configure --prefix=$SPTKDIR
RUN make && make install

# Build the Edinburgh Speech Tools
WORKDIR /usr/local/tts/speech_tools
RUN ./configure
RUN make

# Build Festival
WORKDIR /usr/local/tts/festival
RUN ./configure
RUN make

# Build Festvox
WORKDIR /usr/local/tts/festvox
RUN ./configure
RUN make

WORKDIR /usr/local/tts
