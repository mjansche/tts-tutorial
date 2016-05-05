// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2005-2011 Google, Inc.
// Author: wojciech@google.com (Wojciech Skut)
//         ttai@google.com (Terry Tai)
//
// Main compiler entry point.  Compiles a grm source file to the FST archive.
// Returns zero on success and non-zero on failure.

#include <thrax/compiler.h>
#include <thrax/compat/utils.h>
#include <thrax/grm-compiler.h>
#include <thrax/grm-manager.h>
#include <thrax/function.h>

DEFINE_string(input_grammar, "", "Name of the grammar file.");
DEFINE_string(output_far, "", "Location to write the FST archive.");
DEFINE_string(arc_type, "standard", "Arc type for compiled fsts");
DEFINE_bool(emit_ast_only,
            false,
            "Parse the input, write its AST to stdout and exit without "
            "writing an FST archive.");

using thrax::CompileGrammar;
using thrax::GrmCompilerSpec;
using thrax::GrmManagerSpec;

int main(int argc, char **argv) {
  std::set_new_handler(FailedNewHandler);
  SET_FLAGS(argv[0], &argc, &argv, true);

  thrax::function::RegisterFunctions();
  if (FLAGS_arc_type == "standard") {
    if (CompileGrammar<fst::StdArc>(FLAGS_input_grammar, FLAGS_output_far,
                                        FLAGS_emit_ast_only,
                                        FLAGS_line_numbers_in_ast))
      return 0;
  } else if (FLAGS_arc_type == "log") {
    if (CompileGrammar<fst::LogArc>(FLAGS_input_grammar, FLAGS_output_far,
                                        FLAGS_emit_ast_only,
                                        FLAGS_line_numbers_in_ast))
      return 0;
  } else if (FLAGS_arc_type == "log64") {
    if (CompileGrammar<fst::Log64Arc>(FLAGS_input_grammar, FLAGS_output_far,
                                          FLAGS_emit_ast_only,
                                          FLAGS_line_numbers_in_ast))
      return 0;
  } else {
    LOG(FATAL) << "Unsupported arc type: " << FLAGS_arc_type;
  }
  return 1;
}
