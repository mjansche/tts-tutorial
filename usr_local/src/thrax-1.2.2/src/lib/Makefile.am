AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(srcdir)/../include $(ICU_CPPFLAGS)

lib_LTLIBRARIES = libthrax.la
libthrax_la_SOURCES = ast/collection-node.cc ast/grammar-node.cc ast/fst-node.cc \
                      ast/function-node.cc ast/identifier-node.cc ast/import-node.cc \
                      ast/node.cc ast/return-node.cc ast/rule-node.cc ast/statement-node.cc \
                      ast/string-node.cc \
                      flags/flags.cc \
                      main/grm-compiler.cc main/lexer.cc main/parser.yy main/grm-manager.cc \
		      main/compiler-stdarc.cc main/compiler-log.cc main/compiler-log64.cc \
                      util/utils.cc \
                      walker/evaluator-specializations.cc walker/identifier-counter.cc \
		      walker/loader.cc walker/namespace.cc walker/printer.cc \
                      walker/stringfst.cc walker/symbols.cc walker/walker.cc
libthrax_la_LDFLAGS = -version-info 0:0:0
