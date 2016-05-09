# tts-tutorial

Software for the [Text-to-Speech tutorial at SLTU 2016](http://mica.edu.vn/sltu2016/index.php?pid=23). Examples of how to use some of these tools can be found in the Lab Session section of the [slide deck](https://goo.gl/BjmmHt) for the tutorial.

The [Dockerfile](Dockerfile) here is used to automatically build the image
[mjansche/tts-tutorial](https://hub.docker.com/r/mjansche/tts-tutorial/) and as
a basis for the larger image
[mjansche/tts-tutorial-sltu2016](https://hub.docker.com/r/mjansche/tts-tutorial-sltu2016/)
on Docker Hub.

It is based on Ubuntu 16.04 LTS ("Xenial Xerus") and contains the following
software:

* [Festival](http://www.cstr.ed.ac.uk/projects/festival/) 2.4
* [Edinburgh Speech Tools](http://www.cstr.ed.ac.uk/projects/speech_tools/) 2.4
* [FestVox](http://festvox.org/) 2.7
* [SPTK](http://sp-tk.sourceforge.net/) 3.6
* [OpenFst](http://openfst.org/) 1.5.2
* [Thrax](http://www.opengrm.org/Thrax) 1.2.2
* [Pynini](http://opengrm.org/Pynini) 0.9
* [Sparrowhawk](https://github.com/google/sparrowhawk) 0.1
* [Protocol Buffers](https://github.com/google/protobuf)
* [RE2](https://github.com/google/re2)
* [NCHLT-inlang Afrikaans Pronunciation Dictionary](http://rma.nwu.ac.za/index.php/resource-catalogue/nchlt-inlang-dictionaries.html)
* [Afrikaans Language Resources](https://github.com/googlei18n/language-resources/tree/master/af)
* [Festus G2P runtime and Afrikaans models](https://github.com/googlei18n/language-resources/tree/release)

To use this Docker image, you need to have a working version of Docker; on
Windows and Mac OS you will also need a way to run a Linux kernel in a virtual
machine. Visit [docs.docker.com](https://docs.docker.com/) and follow the "Get
Started" instructions for your platform.

Minimally you want to be able to run the official Ubuntu image, like this:

```
docker run --rm -it ubuntu:xenial /bin/bash
```

Once that is working you can try out synthesizing with Festival's KAL diphone
voice:

```
echo Hello world |
docker run --rm -i mjansche/tts-tutorial-sltu2016 festival/bin/text2wave \
  > hello.wav
```

On many versions of GNU/Linux, the following also works:

```
echo Hello world |
docker run --rm -i mjansche/tts-tutorial-sltu2016 festival/bin/text2wave |
aplay
```

The software shipped with this image can also be used to build a new voice from
scratch. This will be covered during the tutorial at SLTU.
