###########################################################################
# Sparrowhawk integration code
#
# Author: Rob Clark
#
# Copyright 2015 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###########################################################################

TOP=../../..
DIRNAME=src/modules/Sparrowhawk

H = Sparrowhawk.h
SRCS = Sparrowhawk.cc

OBJS = $(SRCS:.cc=.o)
FILES = Makefile $(SRCS) $(H)

LOCAL_INCLUDES = -I../include
INLIB = $(TOP)/src/lib/libFestival.a
LOCAL_DEFINES += $(SPARROWHAWK_DEFINES)

ALL = .buildlib

include $(TOP)/config/common_make_rules
