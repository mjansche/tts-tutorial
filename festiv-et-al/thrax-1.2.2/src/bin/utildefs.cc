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

#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/arc.h>
#include <fst/fst.h>
#include <fst/string.h>
#include <fst/symbol-table.h>
#include <fst/vector-fst.h>
#include <thrax/grm-manager.h>
#include <../bin/utildefs.h>

namespace thrax {

bool VisitState(const Transducer& fst,
                TokenType type,
                const SymbolTable* generated_symtab,
                SymbolTable* symtab,
                StdArc::StateId state,
                const string& path,
                float cost,
                vector<std::pair<string, float> >* paths) {
  if (fst.Final(state) != StdArc::Weight::Zero()) {
    paths->push_back(std::make_pair(path, cost + fst.Final(state).Value()));
  }
  fst::ArcIterator<Transducer> aiter(fst, state);
  for (; !aiter.Done(); aiter.Next()) {
    const StdArc &arc = aiter.Value();
    string newpath = path;
    if (arc.olabel != 0) {
      // Check first to see if this label is in the generated symbol set. Note
      // that this should not conflict with a user-provided symbol table since
      // the parser used by GrmCompiler doesn't generate extra labels if a
      // string is parsed using a user-provided symbol table.
      if (generated_symtab &&
          !generated_symtab->Find(arc.olabel).empty()) {
        string sym = generated_symtab->Find(arc.olabel);
        newpath += "[" + sym + "]";
      } else if (type == SYMBOL) {
        string sym = symtab->Find(arc.olabel);
        if (sym == "") {
          LOG(ERROR) << "Missing symbol in symbol table for id: " << arc.olabel;
          return false;
        }
        newpath += sym;
      } else if (type == BYTE) {
        newpath.push_back(arc.olabel);
      } else if (type == UTF8) {
        string utf8_string;
        vector<int> labels;
        labels.push_back(arc.olabel);
        if (!fst::LabelsToUTF8String(labels, &utf8_string)) {
          LOG(ERROR) << "LabelsToUTF8String: Bad code point: "
                     << arc.olabel;
          return false;
        }
        newpath += utf8_string;
      }
    }
    if (!VisitState(fst, type, generated_symtab,
                    symtab, arc.nextstate, newpath,
                    cost + arc.weight.Value(), paths)) {
      return false;
    }
  }
  return true;
}

bool FstToStrings(const Transducer& fst,
                  vector<std::pair<string, float> >* strings,
                  const SymbolTable* generated_symtab,
                  TokenType type,
                  SymbolTable* symtab,
                  size_t n) {
  Transducer temp;
  fst::ShortestPath(fst, &temp, n);
  fst::Project(&temp, fst::PROJECT_OUTPUT);
  fst::RmEpsilon(&temp);
  if (temp.Start() == fst::kNoStateId) {
    return false;
  }
  return VisitState(temp, type, generated_symtab, symtab,
                    temp.Start(), string(), 0, strings);
}

const fst::SymbolTable*
GetGeneratedSymbolTable(GrmManagerSpec<StdArc>* grm) {
  const fst::Fst<StdArc>* symbolfst = grm->GetFst("*StringFstSymbolTable");
  if (symbolfst) {
    Transducer mutable_symbolfst(*symbolfst);
    return mutable_symbolfst.InputSymbols()->Copy();
  }
  return NULL;
}

}  // namespace thrax
