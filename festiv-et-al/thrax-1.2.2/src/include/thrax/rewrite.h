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
// Wrapper for the rewrite function, where we have two FSTs and rewrite the
// the first into the second.  This function creates only on-the-fly
// transducers.

#ifndef THRAX_REWRITE_H_
#define THRAX_REWRITE_H_

#include <iostream>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/types.h>
#include <fst/fstlib.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/compat/stlfunctions.h>
#include <thrax/compat/registry.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.

namespace thrax {
namespace function {

// A Mapper that turns either all input or all output labels into epsilons.
template <typename Arc>
class EpsilonMapper {
 public:
  enum Type {
    EPSILON_INPUTS,
    EPSILON_OUTPUTS,
  };

  explicit EpsilonMapper(Type type)
      : type_(type) {}

  Arc operator()(const Arc& arc) {
    if (type_ == EPSILON_INPUTS)
      return Arc(0, arc.olabel, arc.weight, arc.nextstate);
    else /* if (type_ == EPSILON_OUTPUTS) */
      return Arc(arc.ilabel, 0, arc.weight, arc.nextstate);
  }

  fst::MapFinalAction FinalAction() const {
    return fst::MAP_NO_SUPERFINAL;
  }

  // Always clear the symbols on both sides. If the FLAGS_save_symbols is set,
  // then Rewrite will take care of passing the appropriate symbol tables
  // through.
  fst::MapSymbolsAction InputSymbolsAction() const {
    return fst::MAP_CLEAR_SYMBOLS;
  }

  fst::MapSymbolsAction OutputSymbolsAction() const {
    return fst::MAP_CLEAR_SYMBOLS;
  }

  uint64 Properties(uint64 in_properties) const {
    uint64 out_properties = 0;
    if (type_ == EPSILON_INPUTS) {
      out_properties |= fst::kILabelInvariantProperties & in_properties;
      out_properties |= fst::kILabelSorted;
    } else /* if (type_ == EPSILON_OUTPUTS) */ {
      out_properties |= fst::kOLabelInvariantProperties & in_properties;
      out_properties |= fst::kOLabelSorted;
    }
    return out_properties;
  }

 private:
  Type type_;
};

template <typename Arc>
class Rewrite : public BinaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;

  Rewrite() {}
  virtual ~Rewrite() {}

 protected:
  virtual Transducer* BinaryFstExecute(const Transducer& left,
                                       const Transducer& right,
                                       const vector<DataType*>& args) {
    if (args.size() != 2) {
      std::cout << "Rewrite: Expected 2 arguments but got " << args.size()
                << std::endl;
      return NULL;
    }

    // If we keep the symbol tables and if either the input or the output is not
    // an acceptor, then the output symbols of the left and the output symbols
    // of the right must match.
    if (FLAGS_save_symbols &&
        (!left.Properties(fst::kAcceptor, true) ||
         !right.Properties(fst::kAcceptor, true))) {
      if (!CompatSymbols(left.OutputSymbols(), right.InputSymbols())) {
        std::cout << "Rewrite: output symbol table of 1st argument "
                  << "does not match input symbol table of 2nd argument "
                  << "and at least one is not an acceptor" << std::endl;
        return NULL;
      }
    }

    fst::MapFst<Arc, Arc, EpsilonMapper<Arc> > left_rmep(
        left, EpsilonMapper<Arc>(EpsilonMapper<Arc>::EPSILON_OUTPUTS));
    fst::MapFst<Arc, Arc, EpsilonMapper<Arc> > right_rmep(
        right, EpsilonMapper<Arc>(EpsilonMapper<Arc>::EPSILON_INPUTS));

    const fst::SymbolTable* input_symbols = left.InputSymbols();
    const fst::SymbolTable* output_symbols = right.OutputSymbols();
    fst::ConcatFst<Arc> concat(left_rmep, right_rmep);
    // First RmEpsilon is to remove epsilons added by Concat
    fst::RmEpsilonFst<Arc> rmeps(concat);
    fst::VectorFst<Arc>* output = new fst::VectorFst<Arc>;
    fst::Push<Arc, fst::REWEIGHT_TO_INITIAL>(rmeps, output,
                                                     fst::kPushLabels);
    // Second RmEpsilon removes the epsilons left over after the label pushing
    fst::RmEpsilon(output);
    // The following is only functional if FLAGS_save_symbols is set
    output->SetInputSymbols(input_symbols);
    output->SetOutputSymbols(output_symbols);
    return output;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Rewrite<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_REWRITE_H_
