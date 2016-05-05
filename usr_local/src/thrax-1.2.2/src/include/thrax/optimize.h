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
// Author: allauzen@google.com (Cyril Allauzen)
//         ttai@google.com (Terry Tai)
//
// This function does a cleaning up of an FST by determinizing and minimizing
// it.  If it's a transducer, we'll encode the arcs beforehand.
//
// This code is taken from the Optimize() function in
// nlp/fst_grammar/context_dependent_rewrite/context_dependent_rewrite.h.

#ifndef THRAX_OPTIMIZE_H_
#define THRAX_OPTIMIZE_H_

#include <iostream>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/fstlib.h>
#include <thrax/algo/optimize.h>
#include <thrax/datatype.h>
#include <thrax/function.h>

namespace thrax {
namespace function {

// We don't need to encode weights if the FST has no weighted cycles, which
// is true if kUnweightedCycles, and is implied by kAcyclic and kUnweighted.
const uint64 kDoNotEncodeWeights = (fst::kAcyclic | fst::kUnweighted |
                                    fst::kUnweightedCycles);

template <typename Arc>
class Optimize : public UnaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  Optimize() {}
  virtual ~Optimize() {}

  // This function is public since other functions sometimes like to call on
  // this without having to go through the whole Function registration
  // rigmarole.
  static Transducer* ActuallyOptimize(const Transducer& fst) {
    MutableTransducer* output = new MutableTransducer(fst);
    fst::Optimize(output);
    return output;
  }

 protected:
  virtual Transducer* UnaryFstExecute(const Transducer& fst,
                                      const vector<DataType*>& args) {
    if (args.size() != 1) {
      std::cout << "Optimize: Expected 1 argument but got " << args.size()
                << std::endl;
      return NULL;
    }

    return ActuallyOptimize(fst);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Optimize<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_OPTIMIZE_H_
