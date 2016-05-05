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
// Various functions for building and maintaining inflectional paradigms, which
// are equivalence classes of words that share morphological behavior.  Examples
// of paradigms are the declensions of nouns and adjectives in Latin, which fall
// into 5 basic paradigms defined by the endings used to mark case, number and
// gender. Within each of these 5 classes there are many subclasses, paradigm
// patterns that behave mostly like the parent paradigm, but differ in a few
// cases. For example, among masculine nouns in declension 2 the default ending
// in the nominative singular is "-us". There are however nouns such as "puer"
// ('boy') and "vir" ('man') that lack the "-us" ending in the nominative
// singular, but otherwise are inflected like other declension 2 masculine
// nouns.
//
// The basic ways in which these tools are used is as follows. First of all,
// define a paradigm using regular expressions. Note that the paradigm should
// represent an ACCEPTOR:
//
// decl2nomsg = sigma* "+us__nom,sg";
// decl2gensg = sigma* "+i__gen,sg";
// ...
// decl2nompl = sigma* "+i__nom,pl";
// decl2nompl = sigma* "+orum__gen,pl";
// ...
// declension2 = decl2nomsg | decl2gensg | decl2nompl | decl2genpl ... ;
//
// (Note that one might want to define morphosyntactic features using the
// mechanisms defined in features.h, rather than as strings of characters as we
// have them here.)
//
// Then we use the functions below to create analyzers, taggers, and to replace
// specific forms within a paradigm as for "vir" and "puer" above.
//
// There is no test set up in this directory for these functions. See
// google3/nlp/grm/language/test/paradigm_test instead.

#ifndef THRAX_PARADIGM_H_
#define THRAX_PARADIGM_H_

#include <string>
#include <vector>
using std::vector;

#include <fst/fstlib.h>
#include <fst/string.h>
#include <thrax/fst-node.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/optimize.h>
#include <thrax/stringfst.h>
#include <thrax/symbols.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.

namespace thrax {
namespace function {

// Analyzer[paradigm, stems, stemmer, deleter]
//
// Given an acceptor defining a paradigm, another acceptor defining a set of
// stems, a transducer that computes the relation between stems and the fully
// inflected forms, and a transducer to delete grammatical info (e.g. features,
// morph boundaries) not found in the surface forms, construct an analyzer that
// will take any inflected form of any stem according to the paradigm, and
// return an analysis of that form.
//
// For example if the paradigm defines forms as in the introduction above, and
// we have the stems "bon", "maxim" and a stemmer:
//
// CDRewrite["+" sigma_star : "", "", "[EOS]", sigma_star];
//
// and a deleter that deletes "+" and feature tags such as "__nom,sg" then the
// analyzer constructed here will map, e.g.
//
// maximus -> maxim+us__nom,sg
// boni -> bon+i__nom,pl

template <typename Arc>
class Analyzer : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  Analyzer() : Function<Arc>() {}

  virtual ~Analyzer() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    CHECK_EQ(args.size(), 4);
    MutableTransducer paradigm(**args[0]->get<Transducer*>());
    MutableTransducer stems(**args[1]->get<Transducer*>());
    MutableTransducer stemmer(**args[2]->get<Transducer*>());
    MutableTransducer deleter(**args[3]->get<Transducer*>());
    MutableTransducer* analyzer = BuildAnalyzer(&paradigm, &stems,
                                                &stemmer, &deleter);
    Transducer* optimized_analyzer = Optimize<Arc>::ActuallyOptimize(*analyzer);
    delete analyzer;
    MutableTransducer* final_analyzer =
        new MutableTransducer(*optimized_analyzer);
    if (FLAGS_save_symbols) {
      final_analyzer->SetInputSymbols(paradigm.InputSymbols());
      final_analyzer->SetOutputSymbols(paradigm.OutputSymbols());
    }
    return new DataType(final_analyzer);
  }

 public:
  static MutableTransducer* BuildAnalyzer(MutableTransducer* paradigm,
                                          MutableTransducer* stems,
                                          MutableTransducer* stemmer,
                                          MutableTransducer* deleter) {
    MutableTransducer mapper;
    // Construct a mapper from all possible analyzed forms to a stemmed version,
    // where "stemmer" defines what "stemmed" means. For example, this might map
    // from "long+us[cas=nom,num=sg]" to "long[cas=nom,num=sg]"
    fst::Compose(*paradigm, *stemmer, &mapper);
    // Invert to map the other way.
    fst::Invert(&mapper);
    MutableTransducer inflected;
    // Now compose with the set of stems to map from a given list of stems into
    // analyzed inflected forms.
    fst::Compose(*stems, mapper, &inflected);
    // We only want the second dimension, namely analyzed inflected forms.
    fst::Project(&inflected, fst::PROJECT_OUTPUT);
    MutableTransducer result;
    // Compose the specific set of analyzed forms with the deleter, which should
    // get rid of features, morph boundaries and other annotations to yield
    // actual surface forms.
    fst::Compose(inflected, *deleter, &result);
    // Invert to map from surface inflected forms to their analyses.
    fst::Invert(&result);
    return new MutableTransducer(result);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Analyzer<Arc>);
};

// Tagger[paradigm, stems, stemmer, deleter, boundary_deleter]
//
// A tagger is just like an analyzer except that instead of outputting full
// analysis of the form, it outputs the form along with its morphosyntactic
// features.  Tagger first builds an analyzer, then composes that with an
// additional FST that deletes just the boundaries.
//
// If the "boundary_deleter" is instead a transducer that maps every inflected
// form to the dictionary form (lemma) of the word, then this becomes a
// lemmatizer. For example, in Latin 2nd declension masculine nouns, map all
// endings to "us":
//
// maximorum -> maximus__gen,pl
//
// The inversion of a lemmatizer is an inflector:
//
// maximus__gen,pl -> maximorum

template <typename Arc>
class Tagger : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  Tagger() : Function<Arc>() {}

  virtual ~Tagger() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    CHECK_EQ(args.size(), 5);
    MutableTransducer paradigm(**args[0]->get<Transducer*>());
    MutableTransducer stems(**args[1]->get<Transducer*>());
    MutableTransducer stemmer(**args[2]->get<Transducer*>());
    MutableTransducer deleter(**args[3]->get<Transducer*>());
    MutableTransducer boundary_deleter(**args[4]->get<Transducer*>());
    MutableTransducer* analyzer = Analyzer<Arc>::BuildAnalyzer(&paradigm,
                                                               &stems,
                                                               &stemmer,
                                                               &deleter);
    MutableTransducer tagger;
    // See above on "boundary_deleter"
    fst::Compose(*analyzer, boundary_deleter, &tagger);
    Transducer* optimized_tagger = Optimize<Arc>::ActuallyOptimize(tagger);
    delete analyzer;
    MutableTransducer* final_tagger = new MutableTransducer(*optimized_tagger);
    if (FLAGS_save_symbols) {
      final_tagger->SetInputSymbols(paradigm.InputSymbols());
      final_tagger->SetOutputSymbols(paradigm.OutputSymbols());
    }
    return new DataType(final_tagger);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Tagger<Arc>);
};

// ParadigmReplace[paradigm, old_forms, new_forms]
//
// Given an ACCEPTOR defining a paradigm, a (possibly empty) ACCEPTOR defining
// the old set of forms to replace, and another (possibly empty) ACCEPTOR
// defining the new forms. Perform the replacement of old_forms with
// new_forms. This is most useful in cases where we want to define a new
// paradigm that is just like another paradigm, except in a few forms.
//
// For example, one can define the replacement needed for "vir", "puer" (see
// above) by redefining the nominative singular ending:
//
// decl2nomsgr =  sigma* "+" "__nom,sg";
//
// Then one could do:
//
// declension2r = ParadigmReplace[declension2, decl2nomsg, decl2nomsgr];

template <typename Arc>
class ParadigmReplace : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  ParadigmReplace() : Function<Arc>() {}

  virtual ~ParadigmReplace() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    CHECK_EQ(args.size(), 3);
    MutableTransducer paradigm(**args[0]->get<Transducer*>());
    MutableTransducer old_forms(**args[1]->get<Transducer*>());
    MutableTransducer new_forms(**args[2]->get<Transducer*>());
    fst::RmEpsilon(&old_forms);
    MutableTransducer det_old_forms;
    fst::Determinize(old_forms, &det_old_forms);
    MutableTransducer difference;
    // First subtract out the old forms that are to be replaced
    fst::Difference(paradigm, det_old_forms, &difference);
    // Then add in the new forms
    fst::Union(&difference, new_forms);
    Transducer* optimized_paradigm =
        Optimize<Arc>::ActuallyOptimize(difference);
    MutableTransducer* final_paradigm =
        new MutableTransducer(*optimized_paradigm);
    if (FLAGS_save_symbols) {
      final_paradigm->SetInputSymbols(paradigm.InputSymbols());
      final_paradigm->SetOutputSymbols(paradigm.OutputSymbols());
    }
    return new DataType(final_paradigm);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ParadigmReplace<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_PARADIGM_H_
