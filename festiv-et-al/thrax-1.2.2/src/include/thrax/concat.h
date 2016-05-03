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
//         rws@google.com (Richard Sproat)
//
// Wrapper for the concatenation function, which expands the second argument and
// concatenates into there (destructive-mode) or just uses ConcatFst
// (delayed-mode).

#ifndef THRAX_CONCAT_H_
#define THRAX_CONCAT_H_

#include <iostream>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/concat.h>
#include <fst/fst.h>
#include <fst/symbol-table.h>
#include <fst/vector-fst.h>
#include <thrax/datatype.h>
#include <thrax/function.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.

namespace thrax {
namespace function {

template <typename Arc>
class Concat : public BinaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  Concat() {}
  virtual ~Concat() {}

 protected:
  virtual Transducer* BinaryFstExecute(const Transducer& left,
                                       const Transducer& right,
                                       const vector<DataType*>& args) {
    if (args.size() != 2) {
      std::cout << "Concat: Expected 2 arguments but got " << args.size()
                << std::endl;
      return NULL;
    }

    if (FLAGS_save_symbols) {
      if (!CompatSymbols(left.InputSymbols(), right.InputSymbols())) {
        std::cout << "Concat: input symbol table of 1st argument "
                  << "does not match input symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
      if (!CompatSymbols(left.OutputSymbols(), right.OutputSymbols())) {
        std::cout << "Concat: output symbol table of 1st argument "
                  << "does not match output symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
    }

    MutableTransducer* mutable_right = new MutableTransducer(right);
    fst::Concat(left, mutable_right);
    return mutable_right;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Concat<Arc>);
};

template <typename Arc>
class ConcatDelayed : public BinaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;

  ConcatDelayed() {}
  virtual ~ConcatDelayed() {}

 protected:
  virtual Transducer* BinaryFstExecute(const Transducer& left,
                                       const Transducer& right,
                                       const vector<DataType*>& args) {
    if (args.size() != 2) {
      std::cout << "ConcatDelayed: Expected 2 arguments but got " << args.size()
                << std::endl;
      return NULL;
    }

    if (FLAGS_save_symbols) {
      if (!CompatSymbols(left.InputSymbols(), right.InputSymbols())) {
        std::cout << "ConcatDelayed: input symbol table of 1st argument "
                  << "does not match input symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
      if (!CompatSymbols(left.OutputSymbols(), right.OutputSymbols())) {
        std::cout << "ConcatDelayed: output symbol table of 1st argument "
                  << "does not match output symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
    }

    return new fst::ConcatFst<Arc>(left, right);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ConcatDelayed<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_CONCAT_H_
