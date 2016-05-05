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
// This function takes an FST and expands it to a VectorFst.  This function is
// cheap if the input is already an expanded VectorFst.

#ifndef THRAX_EXPAND_H_
#define THRAX_EXPAND_H_

#include <iostream>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/fstlib.h>
#include <thrax/datatype.h>
#include <thrax/function.h>

namespace thrax {
namespace function {

template <typename Arc>
class Expand : public UnaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  Expand() {}
  virtual ~Expand() {}

 protected:
  virtual Transducer* UnaryFstExecute(const Transducer& fst,
                                      const vector<DataType*>& args) {
    if (args.size() != 1) {
      std::cout << "Expand: Expected 1 argument but got " << args.size()
                << std::endl;
      return NULL;
    }
    return new MutableTransducer(fst);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Expand<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_EXPAND_H_
