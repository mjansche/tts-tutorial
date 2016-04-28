# tts-tutorial

Software for the Text-to-Speech tutorial at SLTU 2016.

The [Dockerfile](Dockerfile) here is used to build the image
[mjansche/tts-tutorial](https://hub.docker.com/r/mjansche/tts-tutorial/) on
Docker Hub. It is based on Ubuntu 16.04 LTS ("Xenial Xerus") and contains the
following software:

* Festival, Festvox, and related tools
* OpenFst
* Thrax
* Sparrowhawk
* Google Protocol Buffers
* RE2

To use this Docker image, you need to have a working version of Docker; on
Windows and Mac OS you will also need a way to run a Linux kernel in a virtual
machine. Visit [docker.com](https://www.docker.com/) and follow their Getting
Started instructions on how to set up Docker.

Minimally you want to be able to run the official Ubuntu image, like this:

```
docker run --rm -it ubuntu:xenial /bin/bash
```

Once that is working you can try out synthesizing with Festival's KAL diphone
voice:

```
echo Hello world |
docker run --rm -i mjansche/tts-tutorial /usr/local/src/festival/bin/text2wave \
  > hello.wav
```

On many versions of GNU/Linux, the following also works:

```
echo Hello world |
docker run --rm -i mjansche/tts-tutorial /usr/local/src/festival/bin/text2wave |
aplay
```

The software shipped with this image can also be used to build a new voice from
scratch. This will be covered during the tutorial at SLTU.
