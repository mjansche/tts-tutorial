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
// Author: rws@google.com (Richard Sproat)
//
// Stand-alone binary to load up a FAR and generate a random set
// of strings from a given rule. Useful for debugging to see the kinds of things
// the grammar rule will accept.

#include <iostream>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/compat/utils.h>
#include <fst/arc.h>
#include <fst/fst.h>
#include <fst/randgen.h>
#include <fst/string.h>
#include <fst/symbol-table.h>
#include <fst/vector-fst.h>
#include <thrax/grm-manager.h>
#include <../bin/utildefs.h>
#include <thrax/symbols.h>

using fst::StdArc;
using fst::SymbolTable;
using fst::VectorFst;
using thrax::FstToStrings;
using thrax::GetGeneratedSymbolTable;
using thrax::GrmManagerSpec;
using thrax::Split;
using thrax::VisitState;
typedef VectorFst<StdArc> Transducer;
typedef StdArc::Label Label;

DEFINE_string(far, "", "Path to the FAR.");
DEFINE_string(rule, "", "Names of the rewrite rule.");
DEFINE_string(input_mode, "byte", "Either \"byte\", \"utf8\", or the path to "
              "a symbol table for input parsing.");
DEFINE_string(output_mode, "byte", "Either \"byte\", \"utf8\", or the path to "
              "a symbol table for input parsing.");
DEFINE_int64(noutput, 1, "Maximum number of output mappings to generate.");

int main(int argc, char** argv) {
  std::set_new_handler(FailedNewHandler);
  SET_FLAGS(argv[0], &argc, &argv, true);

  GrmManagerSpec<StdArc> grm;
  CHECK(grm.LoadArchive(FLAGS_far));
  const SymbolTable* generated_symtab = GetGeneratedSymbolTable(&grm);
  SymbolTable* output_symtab = NULL;
  TokenType type;
  if (FLAGS_output_mode == "byte") {
    type = BYTE;
  } else if (FLAGS_output_mode == "utf8") {
    type = UTF8;
  } else {
    type = SYMBOL;
    output_symtab = SymbolTable::ReadText(FLAGS_output_mode);
    CHECK(output_symtab)
        ;
  }
  SymbolTable* input_symtab = NULL;
  if (FLAGS_input_mode == "byte") {
    type = BYTE;
  } else if (FLAGS_input_mode == "utf8") {
    type = UTF8;
  } else {
    type = SYMBOL;
    input_symtab = SymbolTable::ReadText(FLAGS_input_mode);
    CHECK(input_symtab)
        ;
  }
  if (FLAGS_rule.empty())
    LOG(FATAL) << "--rule must be specified";
  const fst::Fst<StdArc>* fst = grm.GetFst(FLAGS_rule);
  if (!fst) {
    LOG(FATAL) << "grm.GetFst() must be non NULL for rule: "
               << FLAGS_rule;
  }
  // If the exported rule is not optimized, it may have final Infinite
  // costs. This can cause problems with randgen. RmEpsilon has the effect of
  // cleaning this up.
  fst::StdVectorFst cleaned(*fst);
  fst::RmEpsilon(&cleaned);
  vector<std::pair<string, float> > istrings;
  vector<std::pair<string, float> > ostrings;
  fst::UniformArcSelector<StdArc> uniform_selector;
  fst::RandGenOptions< fst::UniformArcSelector<StdArc> >
      opts(uniform_selector, INT_MAX, 1, true, false);
  for (int i = 0; i < FLAGS_noutput; ++i) {
    fst::StdVectorFst ofst;
    fst::RandGen(cleaned, &ofst, opts);
    if (ofst.NumStates() == 0) continue;
    fst::StdVectorFst ifst(ofst);
    fst::Project(&ifst, fst::PROJECT_INPUT);
    fst::Project(&ofst, fst::PROJECT_OUTPUT);
    if (!FstToStrings(ifst, &istrings, generated_symtab, type, input_symtab)) {
      LOG(FATAL) << "Can't generate strings for input side";
    }
    if (!FstToStrings(ofst, &ostrings, generated_symtab, type, output_symtab)) {
      LOG(FATAL) << "Can't generate strings for output side";
    }
  }
  for (int i = 0; i < istrings.size(); ++i) {
    std::cout << "****************************************" << std::endl;
    std::cout << istrings[i].first << std::endl << ostrings[i].first
              << std::endl;
    // TODO(rws): Currently there is an issue that RandGen() removes weights, so
    // we'll never actually see these costs.
    if (istrings[i].second != 0)
      std::cout << " <cost=" << istrings[i].second << ">" << std::endl;
  }
  delete output_symtab;
  return 0;
}
