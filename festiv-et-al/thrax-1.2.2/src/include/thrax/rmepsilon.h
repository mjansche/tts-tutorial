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
// This function removes epsilon arcs from an FST.  The argument FST will be
// expanded, and the epsilon arcs removed in place.

#ifndef THRAX_RMEPSILON_H_
#define THRAX_RMEPSILON_H_

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
class RmEpsilon : public UnaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  RmEpsilon() {}
  virtual ~RmEpsilon() {}

 protected:
  virtual Transducer* UnaryFstExecute(const Transducer& fst,
                                      const vector<DataType*>& args) {
    if (args.size() != 1) {
      std::cout << "RmEpsilon: Expected 1 argument but got " << args.size()
                << std::endl;
      return NULL;
    }

    MutableTransducer* output = new MutableTransducer(fst);
    fst::RmEpsilon(output);
    return output;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(RmEpsilon<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_RMEPSILON_H_
