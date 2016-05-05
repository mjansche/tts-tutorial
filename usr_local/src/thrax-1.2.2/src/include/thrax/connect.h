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
// This function connects an FST, removing all unreachable states and paths.

#ifndef THRAX_CONNECT_H_
#define THRAX_CONNECT_H_

#include <iostream>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/connect.h>
#include <fst/fst.h>
#include <fst/vector-fst.h>
#include <thrax/datatype.h>
#include <thrax/function.h>

namespace thrax {
namespace function {

template <typename Arc>
class Connect : public UnaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  Connect() {}
  virtual ~Connect() {}

 protected:
  virtual Transducer* UnaryFstExecute(const Transducer& fst,
                                      const vector<DataType*>& args) {
    if (args.size() != 1) {
      std::cout << "Connect: Expected 1 argument but got " << args.size()
                << std::endl;
      return NULL;
    }

    MutableTransducer* output = new MutableTransducer(fst);
    fst::Connect(output);
    return output;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Connect<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_CONNECT_H_
