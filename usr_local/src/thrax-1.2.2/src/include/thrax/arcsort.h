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
// Arc-sorts the single FST argument.

#ifndef THRAX_ARCSORT_H_
#define THRAX_ARCSORT_H_

#include <iostream>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/arcsort.h>
#include <fst/fst.h>
#include <thrax/datatype.h>
#include <thrax/function.h>

namespace thrax {
namespace function {

template <typename Arc>
class ArcSort : public UnaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  ArcSort() {}
  virtual ~ArcSort() {}

 protected:
  virtual Transducer* UnaryFstExecute(const Transducer& fst,
                                      const vector<DataType*>& args) {
    if (args.size() != 2) {
      std::cout << "ArcSort: Expected 2 arguments but received " << args.size()
                << std::endl;
      return NULL;
    }
    if (!args[1]->is<string>()) {
      std::cout << "ArcSort: Expected string for argument 2" << std::endl;
      return NULL;
    }

    const string& sort = *args[1]->get<string>();
    if (sort == "input") {
      return new fst::ArcSortFst<Arc, fst::ILabelCompare<Arc> >(
          fst, fst::ILabelCompare<Arc>());
    } else if (sort == "output") {
      return new fst::ArcSortFst<Arc, fst::OLabelCompare<Arc> >(
          fst, fst::OLabelCompare<Arc>());
    } else {
      std::cout << "ArcSort: Invalid sort parameter: " << sort
                << " (should be 'input' or 'output')" << std::endl;
      return NULL;
    }
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ArcSort<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_ARCSORT_H_
