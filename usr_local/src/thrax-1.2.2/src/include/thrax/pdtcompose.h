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
// Composes two FSTs together, where one is a pushdown transducer
// (nlp/fst/extensions/pdt). The PDT may be either be the first or second
// transducer, as controlled by the fourth (string) argument to the
// function. The third argument is an FST that defines the alphabet of paired
// left and right parenthesis. For example, the following is a valid
// specification:
//
// 0  1  (  )
// 0  1  [  ]
// 0  1  <  >
// 1
//
// Parenthesis pairs can of course involve any labels one chooses, and the
// left/right pairs must be defined by the grammar writer. E.g.:
//
// parens = ("(" : ")") | ("[" : "]") | ("<" : ">");
//
// This function leaves its arguments unexpanded (if they weren't expanded to
// begin with) and creates an on-the-fly ComposeFst.
//
// If up to four arguments are provided, then we will arcsort neither of the
// input FSTs.  Alternatively, the fifth argument can be a string flag to
// control the sorting: - 'left' will arcsort the left FST on its output tape.
// - 'right' will arcsort the right FST on its input tape.  - 'both' will
// arcsort both the FSTs as above.

#ifndef THRAX_PDTCOMPOSE_H_
#define THRAX_PDTCOMPOSE_H_

#include <iostream>
#include <set>
#include <string>
#include <vector>
using std::vector;

#include <fst/extensions/pdt/compose.h>
#include <fst/extensions/pdt/pdt.h>
#include <fst/fstlib.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/make-parens-pair-vector.h>
#include <thrax/compat/stlfunctions.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.

namespace thrax {
namespace function {

template <typename Arc>
class PdtCompose : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;
  typedef typename Arc::Label Label;

  PdtCompose() {}
  virtual ~PdtCompose() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() < 3 || args.size() > 5) {
      std::cout << "PdtCompose: Expected 3-5 arguments but got " << args.size()
                << std::endl;
      return NULL;
    }

    if (!args[0]->is<Transducer*>()
        || !args[1]->is<Transducer*>()
        || !args[2]->is<Transducer*>()) {
      std::cout << "PdtCompose: First three arguments should be FSTs"
                << std::endl;
      return NULL;
    }
    const Transducer* left = *args[0]->get<Transducer*>();
    const Transducer* right = *args[1]->get<Transducer*>();

    if (FLAGS_save_symbols) {
      if (!CompatSymbols(left->OutputSymbols(), right->InputSymbols())) {
        std::cout << "PdtCompose: output symbol table of 1st argument "
                  << "does not match input symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
    }

    MutableTransducer parens_transducer(**args[2]->get<Transducer*>());
    vector<std::pair<Label, Label> > parens;
    MakeParensPairVector(parens_transducer, &parens);

    bool left_pdt = false;
    if (args.size() > 3) {
      if (!args[3]->is<string>()) {
        std::cout << "PdtCompose: Expected string for argument 4" << std::endl;
        return NULL;
      }
      const string& pdt_direction = *args[3]->get<string>();
      if (pdt_direction != "left_pdt" && pdt_direction != "right_pdt") {
        std::cout
            << "PdtCompose: Expected 'left_pdt' or 'right_pdt' for argument 4"
            << std::endl;
        return NULL;
      }
      if (pdt_direction == "left_pdt") left_pdt = true;
    }

    bool delete_left = false, delete_right = false;
    if (args.size() == 5) {
      if (!args[4]->is<string>()) {
        std::cout << "PdtCompose: Expected string for argument 5" << std::endl;
        return NULL;
      }
      const string& sort_mode = *args[4]->get<string>();
      if (sort_mode != "left" && sort_mode != "right" && sort_mode != "both") {
        std::cout
            << "PdtCompose: Expected 'left', 'right', or 'both' for argument 5"
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

    MutableTransducer* output = new MutableTransducer();
    fst::PdtComposeOptions opts = fst::PdtComposeOptions();
    opts.connect = false;
    if (left_pdt) {
      fst::Compose(*left, parens, *right, output, opts);
    } else {
      fst::Compose(*left, *right, parens, output, opts);
    }

    if (delete_left)
      delete left;
    if (delete_right)
      delete right;

    return new DataType(output);
  }

 private:
  fst::ILabelCompare<Arc> icomp;
  fst::OLabelCompare<Arc> ocomp;

  DISALLOW_COPY_AND_ASSIGN(PdtCompose<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_PDTCOMPOSE_H_
