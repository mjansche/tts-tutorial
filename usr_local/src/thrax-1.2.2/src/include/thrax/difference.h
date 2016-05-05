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
// Takes the difference of two FSTs.  This function may expand the second of the
// FSTs so that it can be optimized (determinized and epsilon-removed) if
// necessary.

#ifndef THRAX_DIFFERENCE_H_
#define THRAX_DIFFERENCE_H_

#include <iostream>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/difference.h>
#include <fst/fst.h>
#include <fst/properties.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/optimize.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.

namespace thrax {
namespace function {

const uint64 kRightProps = fst::kNoEpsilons | fst::kIDeterministic;

template <typename Arc>
class Difference : public BinaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;

  Difference() {}
  virtual ~Difference() {}

 protected:
  virtual Transducer* BinaryFstExecute(const Transducer& left,
                                       const Transducer& right,
                                       const vector<DataType*>& args) {
    if (args.size() != 2) {
      std::cout << "Difference: Expected 2 arguments but got " << args.size()
                << std::endl;
      return NULL;
    }

    if (FLAGS_save_symbols) {
      if (!CompatSymbols(left.InputSymbols(), right.InputSymbols())) {
        std::cout << "Difference: input symbol table of 1st argument "
                  << "does not match input symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
      if (!CompatSymbols(left.OutputSymbols(), right.OutputSymbols())) {
        std::cout << "Difference: output symbol table of 1st argument "
                  << "does not match output symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
    }

    if (right.Properties(kRightProps, false) == kRightProps) {
      return new fst::DifferenceFst<Arc>(left, right);
    } else {
      Transducer* optimized_right = Optimize<Arc>::ActuallyOptimize(right);
      Transducer* return_fst =
          new fst::DifferenceFst<Arc>(left, *optimized_right);
      delete optimized_right;
      return return_fst;
    }
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Difference<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_DIFFERENCE_H_
