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
// Author: ttai@google.com (Terry Tai)
//
// Loads up the appropriate symbol table given the string.

#ifndef THRAX_SYMBOLTABLE_H_
#define THRAX_SYMBOLTABLE_H_

#include <iostream>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/compat/utils.h>
#include <fst/symbol-table.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/compat/stlfunctions.h>

DECLARE_string(indir);  // From util/flags.cc.

namespace thrax {
namespace function {

template <typename Arc>
class SymbolTable : public Function<Arc> {
 public:
  SymbolTable() {}
  virtual ~SymbolTable() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() != 1) {
      std::cout << "SymbolTable: Expected 1 argument but got " << args.size()
                << std::endl;
      return NULL;
    }

    if (!args[0]->is<string>()) {
      std::cout << "SymbolTable: Expected string (path) for argument 1"
                << std::endl;
      return NULL;
    }
    const string& file =
        JoinPath(FLAGS_indir, *args[0]->get<string>());

    VLOG(2) << "Loading symbol table: " << file;
    fst::SymbolTable* symtab(fst::SymbolTable::ReadText(file));
    if (!symtab) {
      std::cout << "SymbolTable: Unable to load symbol table file: " << file
                << std::endl;
      return NULL;
    }

    DataType* output = new DataType(*symtab);
    delete symtab;
    return output;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(SymbolTable<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_SYMBOLTABLE_H_
