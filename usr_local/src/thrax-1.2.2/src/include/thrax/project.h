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
// Projects the fst onto the input or output dimension.

#ifndef THRAX_PROJECT_H_
#define THRAX_PROJECT_H_

#include <iostream>
#include <string>
#include <vector>
using std::vector;

#include <fst/fstlib.h>
#include <thrax/datatype.h>
#include <thrax/function.h>

namespace thrax {
namespace function {

template <typename Arc>
class Project : public UnaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  Project() {}
  virtual ~Project() {}

 protected:
  virtual Transducer* UnaryFstExecute(const Transducer& fst,
                                      const vector<DataType*>& args) {
    if (args.size() != 2) {
      std::cout << "Project: Expected 2 arguments but received " << args.size()
                << std::endl;
      return NULL;
    }
    if (!args[1]->is<string>()) {
      std::cout << "Project: Expected string for argument 2" << std::endl;
      return NULL;
    }

    const string& project = *args[1]->get<string>();
    if (project == "input") {
      return new fst::ProjectFst<Arc>(fst, fst::PROJECT_INPUT);
    } else if (project == "output") {
      return new fst::ProjectFst<Arc>(fst, fst::PROJECT_OUTPUT);
    } else {
      std::cout << "Project: Invalid projection parameter: " << project
                << " (should be 'input' or 'output')" << std::endl;
      return NULL;
    }
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Project<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_PROJECT_H_
