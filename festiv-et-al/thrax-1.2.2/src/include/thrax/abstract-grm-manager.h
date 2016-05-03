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
// The AbstractGrmManager holds a set of FSTs in memory and performs
// rewrites via composition. The class is parametrized by the FST arc
// type. AbstractGrmManager is thread-compatible.

#ifndef NLP_GRM_LANGUAGE_ABSTRACT_GRM_MANAGER_H_
#define NLP_GRM_LANGUAGE_ABSTRACT_GRM_MANAGER_H_

#include <map>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/extensions/mpdt/compose.h>
#include <fst/extensions/mpdt/mpdt.h>
#include <fst/extensions/pdt/compose.h>
#include <fst/extensions/pdt/pdt.h>
#include <fst/extensions/pdt/shortest-path.h>
#include <fst/arc.h>
#include <fst/fstlib.h>
#include <fst/fst.h>
#include <fst/string.h>
#include <fst/vector-fst.h>
#include <thrax/make-parens-pair-vector.h>
#include <thrax/compat/stlfunctions.h>

namespace thrax {

template <typename Arc>
class AbstractGrmManager {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;
  typedef typename Arc::Label Label;

  virtual ~AbstractGrmManager();

  // Read-only access to the underlying FST map.
  const map<string, const Transducer*> &GetFstMap() const { return fsts_; }

  // Compile-time access to the FST table.
  map<string, const Transducer*> *GetFstMap() { return &fsts_; }

  // ***************************************************************************
  // REWRITE: These functions perform the actual rewriting of inputs using the
  // named FSTs.

  // Rewrites the input to the output (in various parse modes).  Returns false
  // on a failed rewrite (i.e., the input was not accepted by the rule's FST)
  // and true otherwise.  Notably, a return value of true and an empty string
  // output is a deliberate rewrite to the empty string and not a failed match.
  // If pdt_parens_rule is not empty, then it the transducer associated with
  // "rule" is assumed to be a pushdown automaton, and that associated with
  // pdt_parens_rule is assumed to specify the parentheses.
  // If pdt_assignments_rule is not empty, then this is assumed to be an MPDT.
  bool RewriteBytes(const string& rule, const string& input,
                    string* output,
                    const string& pdt_parens_rule = "",
                    const string& mpdt_assignments_rule = "") const;
  bool RewriteBytes(const string& rule, const Transducer& input,
                    string* output,
                    const string& pdt_parens_rule = "",
                    const string& mpdt_assignments_rule = "") const;
  bool Rewrite(const string& rule, const string& input,
               MutableTransducer* output,
               const string& pdt_parens_rule = "",
               const string& mpdt_assignments_rule = "") const;
  bool Rewrite(const string& rule, const Transducer& input,
               MutableTransducer* output,
               const string& pdt_parens_rule = "",
               const string& mpdt_assignments_rule = "") const;

  // This helper function (when given a potential string fst) takes the shortest
  // path, projects the output, and then removes epsilon arcs.
  static void StringifyFst(MutableTransducer* output);

  // ***************************************************************************
  // The following functions give access to, modify, or serialize internal data.

  // Returns the FST associated with the particular name.  This class returns
  // the actual pointer to the FST (or NULL if it is not found), so the caller
  // should not free the pointer.
  const Transducer* GetFst(const string& name) const;

  // Gets the named FST, just like GetFst(), but this function doesn't
  // lock anything and is thread-safe because it returns a transducer
  // safely shallow-copied from the original. The caller assumes the
  // ownership of the returned transducer.
  Transducer* GetFstSafe(const string& name) const;

  // Modify the transducer under the given name.
  // If no such rule name exists, returns false, otherwise returns true.
  // Note: For thread-safety, it is assumed this function will not be
  // used in a multi-threaded context.
  bool SetFst(const string& name, const Transducer& input);

  // This function will write the created FSTs into an FST archive with the
  // provided filename.
  virtual void ExportFar(const string& filename) const = 0;

  // Sorts input labels of all FSTs in the archive.
  void SortRuleInputLabels();

 protected:
  AbstractGrmManager();

  // Loads up the FSTs given the supplied reader. Returns true on
  // success and false otherwise.
  template <typename FarReader>
  bool LoadArchive(FarReader *reader);

  // The list of FSTs held by this manager.
  map<string, const Transducer*> fsts_;

 private:
  DISALLOW_COPY_AND_ASSIGN(AbstractGrmManager);
};

template <typename Arc>
AbstractGrmManager<Arc>::AbstractGrmManager() {
}

template <typename Arc>
AbstractGrmManager<Arc>::~AbstractGrmManager() {
  STLDeleteContainerPairSecondPointers(fsts_.begin(), fsts_.end());
}

template <typename Arc>
template <typename FarReader>
bool AbstractGrmManager<Arc>::LoadArchive(FarReader *reader) {
  STLDeleteContainerPairSecondPointers(fsts_.begin(), fsts_.end());
  fsts_.clear();
  for (reader->Reset(); !reader->Done(); reader->Next()) {
    const string& name = reader->GetKey();
    const Transducer* fst = new MutableTransducer(*(reader->GetFst()));
    fsts_[name] = fst;
  }
  SortRuleInputLabels();
  return true;
}

template <typename Arc>
void AbstractGrmManager<Arc>::SortRuleInputLabels() {
  for (typename map<string, const Transducer*>::iterator pos = fsts_.begin();
       pos != fsts_.end(); ++pos) {
    const Transducer *fst = pos->second;
    if (!fst->Properties(fst::kILabelSorted, false)) {
      VLOG(1) << "Updating FST " << fst << " with input label sorted version.";
      MutableTransducer* sorted_fst = new MutableTransducer(*fst);
      fst::ArcSort(sorted_fst, fst::ILabelCompare<Arc>());
      delete fst;
      pos->second = static_cast<const Transducer *>(sorted_fst);
    }
  }
}

template <typename Arc>
const typename AbstractGrmManager<Arc>::Transducer*
AbstractGrmManager<Arc>::GetFst(const string& name) const {
  typename map<string, const Transducer*>::const_iterator pos =
      fsts_.find(name);
  if (pos != fsts_.end()) {
    return pos->second;
  }
  return NULL;
}

template <typename Arc>
typename AbstractGrmManager<Arc>::Transducer*
AbstractGrmManager<Arc>::GetFstSafe(const string& name) const {
  const Transducer *fst = GetFst(name);
  if (fst) {
    return fst->Copy(true);
  } else {
    return NULL;
  }
}

template <typename Arc>
bool AbstractGrmManager<Arc>::SetFst(
    const string& name,
    const Transducer& input) {
  if (fsts_.count(name) == 0) {
    return false;
  }
  delete fsts_[name];
  fsts_[name] = input.Copy(true);
  return true;
}

template <typename Arc>
bool AbstractGrmManager<Arc>::RewriteBytes(
    const string& rule, const string& input,
    string* output,
    const string& pdt_parens_rule,
    const string& mpdt_assignments_rule) const {
  fst::StringCompiler<Arc>
      string_compiler(fst::StringCompiler<Arc>::BYTE);
  MutableTransducer str_fst;
  if (!string_compiler(input, &str_fst))
    return false;
  return RewriteBytes(rule, str_fst, output, pdt_parens_rule,
                      mpdt_assignments_rule);
}

template <typename Arc>
bool AbstractGrmManager<Arc>::RewriteBytes(
    const string& rule,
    const Transducer& input,
    string* output,
    const string& pdt_parens_rule,
    const string& mpdt_assignments_rule) const {
  MutableTransducer output_fst;
  if (!Rewrite(rule, input, &output_fst, pdt_parens_rule,
               mpdt_assignments_rule))
    return false;

  StringifyFst(&output_fst);

  fst::StringPrinter<Arc> printer(fst::StringPrinter<Arc>::BYTE);
  return printer(output_fst, output);
}

template <typename Arc>
bool AbstractGrmManager<Arc>::Rewrite(
    const string& rule, const string& input,
    MutableTransducer* output,
    const string& pdt_parens_rule,
    const string& mpdt_assignments_rule) const {
  fst::StringCompiler<Arc>
      string_compiler(fst::StringCompiler<Arc>::BYTE);
  MutableTransducer str_fst;
  if (!string_compiler(input, &str_fst))
    return false;

  return Rewrite(rule, str_fst, output, pdt_parens_rule,
                 mpdt_assignments_rule);
}

template <typename Arc>
bool AbstractGrmManager<Arc>::Rewrite(
    const string& rule, const Transducer& input,
    MutableTransducer* output,
    const string& pdt_parens_rule,
    const string& mpdt_assignments_rule) const {
  const Transducer *rule_fst = GetFstSafe(rule);
  if (!rule_fst) {
    LOG(ERROR) << "Rule " << rule << " not found.";
    return false;
  }

  const Transducer *pdt_parens_fst = NULL;
  if (!pdt_parens_rule.empty()) {
    pdt_parens_fst = GetFstSafe(pdt_parens_rule);
    if (!pdt_parens_fst) {
      LOG(ERROR) << "PDT parentheses rule " << pdt_parens_rule << " not found.";
      delete rule_fst;
      return false;
    }
  }

  const Transducer *mpdt_assignments_fst = NULL;
  if (!mpdt_assignments_rule.empty()) {
    mpdt_assignments_fst = GetFstSafe(mpdt_assignments_rule);
    if (!mpdt_assignments_fst) {
      LOG(ERROR) << "MPDT assignments rule " << mpdt_assignments_rule
                 << " not found.";
      delete rule_fst;
      if (pdt_parens_fst) {
        delete pdt_parens_fst;
      }
      return false;
    }
  }

  if (pdt_parens_fst) {
    MutableTransducer mut_pdt_parens_fst(*pdt_parens_fst);
    vector<std::pair<Label, Label> > pdt_parens;
    MakeParensPairVector(mut_pdt_parens_fst, &pdt_parens);

    // EXPAND_FILTER removes the parentheses, allowing for subsequent
    // application of PDTs. At the end (in StringifyFst() we use ordinary
    // ShortestPath().
    if (mpdt_assignments_fst) {
      MutableTransducer mut_mpdt_assignments_fst(*mpdt_assignments_fst);
      vector<Label> mpdt_assignments;
      MakeAssignmentsVector(mut_mpdt_assignments_fst, pdt_parens,
                            &mpdt_assignments);
      fst::MPdtComposeOptions opts(true, fst::EXPAND_FILTER);
      fst::Compose(input, *rule_fst, pdt_parens, mpdt_assignments,
                       output, opts);
      delete mpdt_assignments_fst;
    } else {
      fst::PdtComposeOptions opts(true, fst::EXPAND_FILTER);
      fst::Compose(input, *rule_fst, pdt_parens, output, opts);
    }
    delete pdt_parens_fst;
  } else {
    fst::ComposeOptions opts(true, fst::ALT_SEQUENCE_FILTER);
    fst::Compose(input, *rule_fst, output, opts);
  }
  delete rule_fst;
  return true;
}


template <typename Arc>
void AbstractGrmManager<Arc>::StringifyFst(MutableTransducer* fst) {
  MutableTransducer temp;
  fst::ShortestPath(*fst, &temp);
  fst::Project(&temp, fst::PROJECT_OUTPUT);
  fst::RmEpsilon(&temp);
  *fst = temp;
}

}  // namespace thrax

#endif  // NLP_GRM_LANGUAGE_ABSTRACT_GRM_MANAGER_H_
