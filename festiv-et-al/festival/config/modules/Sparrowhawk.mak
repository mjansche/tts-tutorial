 ###########################################################################
 ##                                                                       ##
 ##                 Author: Rob Clark                                     ##
 ##                   Date: November 2015                                 ##
 ## --------------------------------------------------------------------  ##
 ## Sparrowhawk support                                                   ##
 ##                                                                       ##
 ###########################################################################

INCLUDE_SPARROWHAWK=1

MOD_DESC_SPARROWHAWK=USE Sparrowhawk Textnormalisation

MODULE_LIBS += -lsparrowhawk
MODULE_EXTRA_LIBS += -L$(FST_LIB) -lthrax -lprotobuf -lfstscript -lfstfar -lfst -lre2  -Wl,--no-as-needed
SPARROWHAWK_DEFINES = -DSUPPORT_SPARROWHAWK
CXXFLAGS += -std=c++11
