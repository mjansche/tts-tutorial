FROM ubuntu:xenial
MAINTAINER Martin Jansche <mjansche@google.com>

#ADD goog_af_unison_wav_22k.tar /usr/local/src/

RUN apt-get update && apt-get install -y \
      automake \
      bc \
      curl \
      g++ \
      git \
      libc-dev \
      libreadline-dev \
      libtool \
      make \
      ncurses-dev \
      nvi \
      pkg-config \
      python \
      python-dev \
      python-setuptools \
      unzip \
      wavpack \
      wget \
      zip \
      zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

ENV LC_ALL C.UTF-8
ENV OPENFST openfst-1.5.2
ENV THRAX thrax-1.2.2

# Fetch, build, and install the Protocol Buffers package
WORKDIR /usr/local/src
RUN git clone https://github.com/google/protobuf.git
WORKDIR /usr/local/src/protobuf
RUN git reset --hard ca9bbd71d547a05604e8c2bddda66cdba5abe0c4 && \
    ./autogen.sh && \
    ./configure \
      --disable-static \
    && make && make install && make distclean && ldconfig

# Fetch, build, and install the re2 regular expression package
WORKDIR /usr/local/src
RUN git clone https://github.com/google/re2.git
WORKDIR /usr/local/src/re2
RUN git reset --hard 4744450c4880b9445c57d9224495f0e8e29f1c4c && \
    make && make install && make distclean && ldconfig

# Fetch, build, and install the OpenFst Library
WORKDIR /usr/local/src
RUN curl -L http://openfst.org/twiki/pub/FST/FstDownload/$OPENFST.tar.gz | \
    tar xz --no-same-owner --no-same-permissions
WORKDIR /usr/local/src/$OPENFST
RUN ./configure \
      --enable-bin \
      --enable-far \
      --enable-mpdt \
      --enable-ngram-fsts \
      --enable-pdt \
      --enable-python \
    && make && make install && make distclean && ldconfig
ENV LD_LIBRARY_PATH /usr/local/lib/fst:$LD_LIBRARY_PATH

# Fetch, build, and install Pynini
WORKDIR /usr/local/src
RUN curl -L http://openfst.org/twiki/pub/GRM/PyniniDownload/pynini-0.9.tar.gz | \
    tar xz --no-same-owner --no-same-permissions
WORKDIR /usr/local/src/pynini-0.9
RUN python setup.py install && python setup.py test

COPY usr_local /usr/local/

# Build and install the Thrax Grammar Development Tools
WORKDIR /usr/local/src/$THRAX
RUN ./configure \
      --enable-bin \
      --enable-readline \
    && make && make install && make distclean && ldconfig

# Build and install Sparrowhawk
WORKDIR /usr/local/src/sparrowhawk
RUN autoreconf && ./configure && \
    make && make install && make distclean && ldconfig

# Fetch and prepare Festival & friends
WORKDIR /usr/local/src
RUN curl -L http://festvox.org/packed/festival/2.4/festlex_CMU.tar.gz | \
    tar xz --no-same-owner --no-same-permissions && \
    curl -L http://festvox.org/packed/festival/2.4/festlex_POSLEX.tar.gz | \
    tar xz --no-same-owner --no-same-permissions && \
    curl -L http://festvox.org/packed/festival/2.4/voices/festvox_kallpc16k.tar.gz | \
    tar xz --no-same-owner --no-same-permissions && \
    curl -L http://festvox.org/festvox-2.7/festvox-2.7.0-release.tar.gz | \
    tar xz --no-same-owner --no-same-permissions && \
    curl -L http://tts.speech.cs.cmu.edu/awb/sptk/SPTK-3.6.tar.gz | \
    tar xz --no-same-owner --no-same-permissions && \
    patch -p0 < festvox/src/clustergen/SPTK-3.6.patch
ENV ESTDIR /usr/local/src/speech_tools
ENV FESTVOXDIR /usr/local/src/festvox
ENV SPTKDIR /usr/local

# Build and install SPTK
WORKDIR /usr/local/src/SPTK-3.6
RUN ./configure --prefix=$SPTKDIR && make && make install && make distclean

# Build the Edinburgh Speech Tools
WORKDIR /usr/local/src/speech_tools
RUN ./configure && make

# Build Festival
WORKDIR /usr/local/src/festival
RUN ./configure && make

# Build Festvox
WORKDIR /usr/local/src/festvox
RUN ./configure && make

WORKDIR /usr/local/src
RUN rm -fr protobuf re2 $OPENFST pynini-0.9 $THRAX sparrowhawk SPTK-3.6
