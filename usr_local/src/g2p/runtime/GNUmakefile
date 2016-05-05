# festus/runtime/GNUmakefile
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Copyright 2016 Google, Inc.
# Author: mjansche@google.com (Martin Jansche)

# Path to OpenFst installation.
OPENFST=/usr/local

CXX=g++
CXXFLAGS=-std=c++11 -O2
CPPFLAGS=-I$(OPENFST)/include
LDFLAGS=-L$(OPENFST)/lib/fst -L$(OPENFST)/lib
LDLIBS=-lfstngram -lfst -ldl

g2p-lookup:	g2p-lookup.cc

total-weight:	total-weight.cc

clean:
	$(RM) g2p-lookup total-weight
