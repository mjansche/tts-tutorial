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
// Definitions needed by various utilities here.

#ifndef NLP_GRM_LANGUAGE_UTIL_UTILDEFS_H_
#define NLP_GRM_LANGUAGE_UTIL_UTILDEFS_H_

#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/arc.h>
#include <fst/fst.h>
#include <fst/symbol-table.h>
#include <fst/vector-fst.h>
#include <thrax/grm-manager.h>

using fst::StdArc;
using fst::SymbolTable;
using fst::VectorFst;
using thrax::GrmManagerSpec;

typedef VectorFst<StdArc> Transducer;
enum TokenType { SYMBOL = 1, BYTE = 2, UTF8 = 3 };

namespace thrax {

// Helper function for FstToStrings: traverses the (acyclic) acceptor in a
// depth-first search accumulating each path into a string, and accumulating all
// paths into a vector of strings.

bool VisitState(const Transducer& fst,
                TokenType type,
                const SymbolTable* generated_symtab,
                SymbolTable* symtab,
                StdArc::StateId state,
                const string& path,
                float cost,
                vector<std::pair<string, float> >* paths);

// Computes the n-shortest paths and returns a vector of strings, each string
// corresponding to each path. The mapping of labels to strings is controlled by
// the type and the symtab. Elements that are in the generated label set from
// the grammar are output as "[name]" where "name" is the name of the generated
// label.

bool FstToStrings(const Transducer& fst,
                  vector<std::pair<string, float> >* strings,
                  const SymbolTable* generated_symtab,
                  TokenType type = BYTE,
                  SymbolTable* symtab = NULL,
                  size_t n = 1);

// Find the generated labels from the grammar.

const fst::SymbolTable*
GetGeneratedSymbolTable(GrmManagerSpec<StdArc>* grm);


}  // namespace thrax

#endif  // NLP_GRM_LANGUAGE_UTIL_UTILDEFS_H_
