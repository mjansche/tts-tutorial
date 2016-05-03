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
// This is only needed for PdtCompose and for GrmManager when it uses PDTs. We
// separate it off here so that the GrmManager does not depend on all the
// function and datatype components.

#ifndef THRAX_MAKE_PARENS_PAIR_VECTOR_H_
#define THRAX_MAKE_PARENS_PAIR_VECTOR_H_

#include <map>
#include <set>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/fstlib.h>

namespace thrax {

template <typename Arc>
inline void MakeParensPairVector(
    const fst::VectorFst<Arc>& parens_transducer,
    vector<std::pair<typename Arc::Label, typename Arc::Label> > *parens) {
  set<typename Arc::Label> seen_labels;
  typename set<typename Arc::Label>::iterator iter;
  for (typename Arc::StateId s = 0; s < parens_transducer.NumStates(); ++s) {
    fst::ArcIterator<fst::VectorFst<Arc> > aiter(parens_transducer, s);
    while (!aiter.Done()) {
      const Arc& arc = aiter.Value();
      if (!arc.ilabel && !arc.olabel) {
      } else if (!arc.ilabel) {
        LOG(WARNING) << "PdtCompose: left parenthesis corresponding to "
                     << arc.olabel << " is null";
      } else if (!arc.olabel) {
        LOG(WARNING) << "PdtCompose: right parenthesis corresponding to "
                     << arc.ilabel << " is null";
        continue;
      } else {
        iter = seen_labels.find(arc.ilabel);
        if (iter != seen_labels.end()) {
          LOG(FATAL) << "PdtCompose: risky reuse of left paren "
                     << arc.ilabel;
        }
        iter = seen_labels.find(arc.olabel);
        if (iter != seen_labels.end()) {
          LOG(FATAL) << "PdtCompose: risky reuse of right paren "
                     << arc.olabel;
        }
        if (arc.ilabel == arc.olabel) {
          LOG(FATAL) << "PdtCompose: left parenthesis "
                     << arc.ilabel
                     << " is identical to right parenthesis "
                     << arc.olabel;
        }
        parens->push_back(std::pair<typename Arc::Label, typename Arc::Label>(
            arc.ilabel, arc.olabel));
      }
      aiter.Next();
    }
  }
}

// For extracting MPDT assignments
template <typename Arc>
inline void MakeAssignmentsVector(
    const fst::VectorFst<Arc>& assignments_transducer,
    const vector<std::pair<typename Arc::Label, typename Arc::Label> >& parens,
    vector<typename Arc::Label>* assignments) {
  map<typename Arc::Label, typename Arc::Label> assignment_map;
  typename map<typename Arc::Label, typename Arc::Label>::iterator iter;
  for (typename Arc::StateId s = 0;
       s < assignments_transducer.NumStates();
       ++s) {
    fst::ArcIterator<fst::VectorFst<Arc> >
        aiter(assignments_transducer, s);
    while (!aiter.Done()) {
      const Arc& arc = aiter.Value();
      if (!arc.ilabel && !arc.olabel) {
      } else if (!arc.ilabel) {
        LOG(WARNING) << "MPdtCompose: left parenthesis"
                     << "corresponding to assignment "
                     << arc.olabel << " is null";
      } else if (!arc.olabel) {
        LOG(WARNING) << "MPdtCompose: assignment corresponding"
                     << " to left parenthesis "
                     << arc.ilabel << " is null";
        continue;
      } else {
        assignment_map[arc.ilabel] = arc.olabel;
      }
      aiter.Next();
    }
  }
  for (int i = 0; i < parens.size(); ++i) {
    typename Arc::Label iparen = parens[i].first;
    iter = assignment_map.find(iparen);
    if (iter == assignment_map.end())
      LOG(FATAL) << "MPdtCompose: left parenthesis "
                 << iparen
                 << " has no stack assignment.";
    assignments->push_back(iter->second);
  }
}

}  // namespace thrax

#endif  // THRAX_MAKE_PARENS_PAIR_VECTOR_H_
