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
// Composes two FSTs together.  This function leaves its arguments unexpanded
// (if they weren't expanded to begin with) and creates an on-the-fly
// ComposeFst.
//
// If only two arguments are provided, then we will arcsort neither of the input
// FSTs.  Alternatively, the third argument can be a string flag to control the
// sorting:
//   - 'left' will arcsort the left FST on its output tape.
//   - 'right' will arcsort the right FST on its input tape.
//   - 'both' will arcsort both the FSTs as above.

#ifndef THRAX_COMPOSE_H_
#define THRAX_COMPOSE_H_

#include <iostream>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/fstlib.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/compat/stlfunctions.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.

namespace thrax {
namespace function {

template <typename Arc>
class Compose : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;

  Compose() {}
  virtual ~Compose() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() != 2 && args.size() != 3) {
      std::cout << "Compose: Expected 2 or 3 arguments but got " << args.size()
                << std::endl;
      return NULL;
    }

    if (!args[0]->is<Transducer*>() || !args[1]->is<Transducer*>()) {
      std::cout << "Compose: First two arguments should be FSTs" << std::endl;
      return NULL;
    }
    const Transducer* left = *args[0]->get<Transducer*>();
    const Transducer* right = *args[1]->get<Transducer*>();
    bool delete_left = false, delete_right = false;

    if (FLAGS_save_symbols) {
      if (!CompatSymbols(left->OutputSymbols(), right->InputSymbols())) {
        std::cout << "Compose: output symbol table of 1st argument "
                  << "does not match input symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
    }

    if (args.size() == 3) {
      if (!args[2]->is<string>()) {
        std::cout << "Compose: Expected string for argument 3" << std::endl;
        return NULL;
      }
      const string& sort_mode = *args[2]->get<string>();
      if (sort_mode != "left" && sort_mode != "right" && sort_mode != "both") {
        std::cout
            << "Compose: Expected 'left', 'right', or 'both' for argument 3"
            << std::endl;
        return NULL;
      }

      if (sort_mode != "right") {
        left = new fst::ArcSortFst<Arc, fst::OLabelCompare<Arc> >(
            *left, ocomp);
        delete_left = true;
      }
      if (sort_mode != "left") {
        right = new fst::ArcSortFst<Arc, fst::ILabelCompare<Arc> >(
            *right, icomp);
        delete_right = true;
      }
    }

    Transducer* output = new fst::ComposeFst<Arc>(*left, *right);

    if (delete_left)
      delete left;
    if (delete_right)
      delete right;

    return new DataType(output);
  }

 private:
  fst::ILabelCompare<Arc> icomp;
  fst::OLabelCompare<Arc> ocomp;

  DISALLOW_COPY_AND_ASSIGN(Compose<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_COMPOSE_H_
