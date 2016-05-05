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
// Composes two FSTs together, where one is a multistack pushdown transducer
// (nlp/fst/extensions/mpdt). The MPDT may be either be the first or second
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
// The fourth argument defines the assignments of the *left* paren to stacks.
// Currently we only expose two stacks. So we might have:
//
// 0  1  (   1
// 0  1  [   1
// 0  1  <   2
// 1
//
// This function leaves its arguments unexpanded (if they weren't expanded to
// begin with) and creates an on-the-fly ComposeFst.
//
// If up to four arguments are provided, then we will arcsort neither of the
// input FSTs.  Alternatively, the fifth argument can be a string flag to
// control the sorting: - 'left' will arcsort the left FST on its output tape.
// - 'right' will arcsort the right FST on its input tape.  - 'both' will
// arcsort both the FSTs as above.

#ifndef THRAX_MPDTCOMPOSE_H_
#define THRAX_MPDTCOMPOSE_H_

#include <iostream>
#include <set>
#include <string>
#include <vector>
using std::vector;

#include <fst/extensions/mpdt/compose.h>
#include <fst/extensions/mpdt/mpdt.h>
#include <fst/fstlib.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/make-parens-pair-vector.h>
#include <thrax/compat/stlfunctions.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.

namespace thrax {
namespace function {

template <typename Arc>
class MPdtCompose : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;
  typedef typename Arc::Label Label;

  MPdtCompose() {}
  virtual ~MPdtCompose() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() < 4 || args.size() > 6) {
      std::cout << "MPdtCompose: Expected 4-6 arguments but got " << args.size()
                << std::endl;
      return NULL;
    }

    if (!args[0]->is<Transducer*>()
        || !args[1]->is<Transducer*>()
        || !args[2]->is<Transducer*>()
        || !args[3]->is<Transducer*>()) {
      std::cout << "MPdtCompose: First four arguments should be FSTs"
                << std::endl;
      return NULL;
    }
    const Transducer* left = *args[0]->get<Transducer*>();
    const Transducer* right = *args[1]->get<Transducer*>();

    if (FLAGS_save_symbols) {
      if (!CompatSymbols(left->OutputSymbols(), right->InputSymbols())) {
        std::cout << "MPdtCompose: output symbol table of 1st argument "
                  << "does not match input symbol table of 2nd argument"
                  << std::endl;
        return NULL;
      }
    }

    MutableTransducer parens_transducer(**args[2]->get<Transducer*>());
    vector<std::pair<Label, Label> > parens;
    MakeParensPairVector(parens_transducer, &parens);

    MutableTransducer assignments_transducer(**args[3]->get<Transducer*>());
    vector<Label> assignments;
    MakeAssignmentsVector(assignments_transducer, parens, &assignments);
    // NB: In the underlying nlp/fst/extensions/mpdt library we actually just
    // use left_pdt/right_pdt, but to keep things a little more clear for the
    // grammar writer, here we use left_mpdt/right_mpdt
    bool left_mpdt = false;
    if (args.size() > 4) {
      if (!args[4]->is<string>()) {
        std::cout << "MPdtCompose: Expected string for argument 5" << std::endl;
        return NULL;
      }
      const string& mpdt_direction = *args[4]->get<string>();
      if (mpdt_direction != "left_mpdt" && mpdt_direction != "right_mpdt") {
        std::cout << "MPdtCompose: Expected"
                  << " 'left_mpdt' or 'right_mpdt' for argument 5" << std::endl;
        return NULL;
      }
      if (mpdt_direction == "left_mpdt") left_mpdt = true;
    }

    bool delete_left = false, delete_right = false;
    if (args.size() == 6) {
      if (!args[5]->is<string>()) {
        std::cout << "MPdtCompose: Expected string for argument 6" << std::endl;
        return NULL;
      }
      const string& sort_mode = *args[5]->get<string>();
      if (sort_mode != "left" && sort_mode != "right" && sort_mode != "both") {
        std::cout << "MPdtCompose: Expected 'left', 'right', or 'both'"
                  << " for argument 6" << std::endl;
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
    fst::MPdtComposeOptions opts = fst::MPdtComposeOptions();
    opts.connect = false;
    if (left_mpdt) {
      fst::Compose(*left, parens, assignments, *right, output, opts);
    } else {
      fst::Compose(*left, *right, parens, assignments, output, opts);
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

  DISALLOW_COPY_AND_ASSIGN(MPdtCompose<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_MPDTCOMPOSE_H_
