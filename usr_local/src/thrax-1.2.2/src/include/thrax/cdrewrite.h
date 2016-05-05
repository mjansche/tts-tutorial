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
// Wrapper for the context dependent rewriter.  Arguments should be, in order,
// 1.) The rewrite transducer (tau).
// 2.) The left context acceptor (lambda).
// 3.) The right context acceptor (rho).
// 4.) The minimized alphabet closure acceptor (sigma*).
// 5.) The string 'ltr', 'rtl', or 'sim' for the direction of rewrite. [opt]
// 6.) The string 'obl' or 'opt' for the rewrite mode. [opt]
//
// If arguments 5 and 6 are omitted, we'll perform a left-to-right and
// obligatory rewrite by default.

#ifndef THRAX_CDREWRITE_H_
#define THRAX_CDREWRITE_H_

#include <iostream>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/fstlib.h>
#include <thrax/algo/cdrewrite.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/compat/stlfunctions.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.
DECLARE_int64(initial_boundary_marker);
DECLARE_int64(final_boundary_marker);

namespace thrax {
namespace function {

template <typename Arc>
class CDRewrite : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  CDRewrite() : Function<Arc>() {}
  virtual ~CDRewrite() {}

  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() != 4 && args.size() != 6) {
      std::cout << "CDRewrite: Expected 4 or 6 arguments but received "
                << args.size() << std::endl;
      return NULL;
    }
    for (int i = 0; i < 4; ++i) {
      if (!args[i]->is<Transducer*>()) {
        std::cout << "CDRewrite: Expect FST for argument " << i + 1
                  << std::endl;
        return NULL;
      }
    }

    MutableTransducer tau(**args[0]->get<Transducer*>());
    MutableTransducer lambda(**args[1]->get<Transducer*>());
    MutableTransducer rho(**args[2]->get<Transducer*>());
    MutableTransducer sigma(**args[3]->get<Transducer*>());

    // All symbol tables for all transducers must match, and all input symbols
    // must match all output symbols. It makes no sense if they don't. So we
    // perform all these checks.
    //
    // Then, internal to ContextDependentRewrite, we need symbol-table-free
    // versions, since that algorithm constructs a large number of intermediate
    // filters that need to be composed with the pieces that are passed through
    // here. The choices to deal with that are:
    //
    // 1) Change  to add symbol tables to all of the
    //    intermediate filters. This seems messy.
    // 2) Toggle FLAGS_fst_compat_symbols prior to passing this in. This is not
    //    thread-safe.
    // 3) Null out the symbol tables of all of the pieces, pass them in and then
    //    put the symbol table on the result when we are done. This seems the
    //    simplest and safest.
    const fst::SymbolTable* symbols = NULL;
    if (FLAGS_save_symbols) {
      if (!CompatSymbols(tau.InputSymbols(), tau.OutputSymbols())) {
        std::cout
            << "CDRewrite: input symbols and output symbols must match for tau"
            << std::endl;
        return NULL;
      }
      if (!CompatSymbols(lambda.InputSymbols(), lambda.OutputSymbols())) {
        std::cout << "CDRewrite: input symbols and output symbols must match "
                     "for lambda" << std::endl;
        return NULL;
      }
      if (!CompatSymbols(rho.InputSymbols(), rho.OutputSymbols())) {
        std::cout
            << "CDRewrite: input symbols and output symbols must match for rho"
            << std::endl;
        return NULL;
      }
      if (!CompatSymbols(sigma.InputSymbols(), sigma.OutputSymbols())) {
        std::cout << "CDRewrite: input symbols and output symbols must match "
                     "for sigma" << std::endl;
        return NULL;
      }
      if (!CompatSymbols(tau.InputSymbols(), lambda.InputSymbols())) {
        std::cout << "CDRewrite: symbol tables for tau and lambda must match"
                  << std::endl;
        return NULL;
      }
      if (!CompatSymbols(tau.InputSymbols(), rho.InputSymbols())) {
        std::cout << "CDRewrite: symbol tables for tau and rho must match"
                  << std::endl;
        return NULL;
      }
      if (!CompatSymbols(tau.InputSymbols(), sigma.InputSymbols())) {
        std::cout << "CDRewrite: symbol tables for tau and sigma must match"
                  << std::endl;
        return NULL;
      }
      symbols = (*args[0]->get<Transducer*>())->InputSymbols();  // tau
      tau.SetInputSymbols(NULL);
      tau.SetOutputSymbols(NULL);
      lambda.SetInputSymbols(NULL);
      lambda.SetOutputSymbols(NULL);
      rho.SetInputSymbols(NULL);
      rho.SetOutputSymbols(NULL);
      sigma.SetInputSymbols(NULL);
      sigma.SetOutputSymbols(NULL);
    }

    fst::CDRewriteDirection dir = fst::LEFT_TO_RIGHT;
    fst::CDRewriteMode mode = fst::OBLIGATORY;
    if (args.size() == 6) {
      for (int i = 4; i < 6; ++i) {
        if (!args[i]->is<string>()) {
          std::cout << "CDRewrite: Expected string for argument " << i + 1
                    << std::endl;
          return NULL;
        }
      }

      const string& direction_str = *args[4]->get<string>();
      if (direction_str == "ltr") {
        dir = fst::LEFT_TO_RIGHT;
      } else if (direction_str == "rtl") {
        dir = fst::RIGHT_TO_LEFT;
      } else if (direction_str == "sim") {
        dir = fst::SIMULTANEOUS;
      } else {
        std::cout << "CDRewrite: Invalid direction: " << direction_str
                  << std::endl;
        return NULL;
      }

      const string& mode_str = *args[5]->get<string>();
      if (mode_str == "obl") {
        mode = fst::OBLIGATORY;
      } else if (mode_str == "opt") {
        mode = fst::OPTIONAL;
      } else {
        std::cout << "CDRewrite: Invalid mode: " << mode_str << std::endl;
        return NULL;
      }
    }

    MutableTransducer sigma_aug(sigma);
    MutableTransducer* output = new MutableTransducer();
    AddBoundaryMarkersToSigma(&sigma_aug);
    fst::CDRewriteCompile(tau, lambda, rho, sigma_aug, output, dir, mode);
    MutableTransducer inserter;
    BoundaryInserter(sigma, &inserter);
    MutableTransducer deleter;
    BoundaryDeleter(sigma, &deleter);
    MutableTransducer tmp;
    fst::ArcSort(&inserter, fst::OLabelCompare<Arc>());
    fst::Compose(inserter, *output, &tmp);
    fst::ArcSort(&deleter, fst::ILabelCompare<Arc>());
    fst::Compose(tmp, deleter, output);
    if (FLAGS_save_symbols) {
      output->SetInputSymbols(symbols);
      output->SetOutputSymbols(symbols);
    }
    return new DataType(output);
  }

 private:
  // Essentially the same functionality as
  // ContextDependentRewriteRule<Arc>::AddMarkersToSigma(), which is a private
  // method of that class. This adds the beginning and ending markers which must
  // be a part of sigma.
  void AddBoundaryMarkersToSigma(MutableTransducer* sigma) {
    for (typename Arc::StateId s = 0; s < sigma->NumStates(); ++s) {
      if (sigma->Final(s) != Arc::Weight::Zero()) {
        sigma->AddArc(s, Arc(FLAGS_initial_boundary_marker,
                             FLAGS_initial_boundary_marker,
                             Arc::Weight::One(), sigma->Start()));
        sigma->AddArc(s, Arc(FLAGS_final_boundary_marker,
                             FLAGS_final_boundary_marker,
                             Arc::Weight::One(), sigma->Start()));
      }
    }
  }

  // Construct transducer that either inserts or deletes boundary markers.
  void HandleBoundaryMarkers(const Transducer& sigma,
                             MutableTransducer* final,
                             bool del) {
    MutableTransducer initial;
    typename Arc::StateId start = initial.AddState();
    typename Arc::StateId end = initial.AddState();
    initial.SetStart(start);
    initial.SetFinal(end, Arc::Weight::One());
    initial.AddArc(start, Arc(del ? FLAGS_initial_boundary_marker : 0,
                              del ? 0 : FLAGS_initial_boundary_marker,
                              Arc::Weight::One(), end));
    start = final->AddState();
    end = final->AddState();
    final->SetStart(start);
    final->SetFinal(end, Arc::Weight::One());
    final->AddArc(start, Arc(del ? FLAGS_final_boundary_marker : 0,
                             del ? 0 : FLAGS_final_boundary_marker,
                             Arc::Weight::One(), end));
    fst::Concat(&initial, sigma);
    fst::Concat(initial, final);
    // Fixes bug whereby
    //
    // CDRewrite["" : "a", "", "", sigma_star]
    //
    // produces no output ("rewrite failed") because the rule inserts an "a"
    // before the "[BOS]" and after the "[EOS]", in addition to anywhere in the
    // input string. The output filter "[BOS] sigma_star [EOS]" blocks these, so
    // that in an obligatory application, you get no output.  The new version
    // deletes anything from sigma that occurs before the [BOS] or after the
    // [EOS], so that you only get insertion where you should --- in the string
    // proper. Note that only in an insertion with no specified left or right
    // context will this situation arise.
    //
    // The slight drawback is that if someone writes an ill-formed
    // insertion rule such as
    //
    // CDRewrite["" : "a" , "[EOS]", "", sigma_star]
    //
    // (note the misplaced [EOS]), then this will give an output --- though not
    // with the illicit inserted "a" as written, as opposed to simply failing as
    // it would have before. It's not clear this is a bad result.
    if (del) {
      MutableTransducer initial_del_sigma(sigma);
      // Creates the sigma* deletion fst.
      for (int i = 0; i < initial_del_sigma.NumStates(); ++i) {
        for (fst::MutableArcIterator<MutableTransducer> aiter(
                 &initial_del_sigma, i);
             !aiter.Done();
             aiter.Next()) {
          Arc arc = aiter.Value();
          arc.olabel = 0;
          aiter.SetValue(arc);
        }
      }
      MutableTransducer final_del_sigma(initial_del_sigma);
      fst::Concat(&initial_del_sigma, *final);
      fst::Concat(initial_del_sigma, &final_del_sigma);
      *final = final_del_sigma;
    }
  }

  // Construct epsilon:initial sigma* epsilon:final
  void BoundaryInserter(const Transducer& sigma, MutableTransducer* final) {
    HandleBoundaryMarkers(sigma, final, false);
  }

  // Construct initial:epsilon sigma* final:epsilon
  void BoundaryDeleter(const Transducer& sigma, MutableTransducer* final) {
    HandleBoundaryMarkers(sigma, final, true);
  }

  DISALLOW_COPY_AND_ASSIGN(CDRewrite<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_CDREWRITE_H_
