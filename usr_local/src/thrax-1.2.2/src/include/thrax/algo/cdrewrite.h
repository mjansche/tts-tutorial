// Compiles context-dependent rewrite rules into weighted transducers.
//
// The algorithm of Mohri and Sproat (1996) is implemented and most
// comments refer to this paper:
//
//   Mehryar Mohri and Richard Sproat.
//   An Efficient Compiler for Weighted Rewrite Rules.
//   In 34th Meeting of the Association for Computational Linguistics
//   (ACL '96), Proceedings of the Conference, Santa Cruz,
//   California. Santa Cruz, California, 1996.
//   http://acl.ldc.upenn.edu/P/P96/P96-1031.pdf

#ifndef FST_GRAMMAR_CDREWRITE_CDREWRITE_H_
#define FST_GRAMMAR_CDREWRITE_CDREWRITE_H_

#include <set>
#include <utility>
using std::pair; using std::make_pair;
#include <vector>
using std::vector;
#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/types.h>
#include <fst/fstlib.h>
#include <thrax/algo/optimize.h>

namespace fst {

enum EpsMapperType { EPS_ON_INPUT = 1, EPS_ON_OUTPUT = 2 };

// Mapper to change input or output label of every transition into
// epsilons.
template <class A, EpsMapperType T>
class EpsMapper {
 public:
  EpsMapper() {}

  A operator()(const A &arc) const {
    return A(T == EPS_ON_INPUT ? 0 : arc.ilabel,
             T == EPS_ON_OUTPUT ? 0 : arc.olabel,
             arc.weight,
             arc.nextstate);
  }

  uint64 Properties(uint64 props) const {
    props &= ~kAcceptor & ~kNotAcceptor;
    props &= (T == EPS_ON_INPUT ? ~kNoIEpsilons : ~kNoOEpsilons);
    props &= ~kNoEpsilons;
    props |= (T == EPS_ON_INPUT ? kILabelSorted : kOLabelSorted);
    return props;
  }

  MapFinalAction FinalAction() const { return MAP_NO_SUPERFINAL; }

  MapSymbolsAction InputSymbolsAction() const { return MAP_COPY_SYMBOLS; }

  MapSymbolsAction OutputSymbolsAction() const { return MAP_COPY_SYMBOLS;}
};

enum CDRewriteDirection { LEFT_TO_RIGHT, RIGHT_TO_LEFT, SIMULTANEOUS };
enum CDRewriteMode { OBLIGATORY, OPTIONAL };

// This class is used to represent context-dependent rewrite rules.  A
// given rule can be compile into a weighted transducer using
// different parameters (direction, mode, alphabet) by calling
// 'Compile'. See comments before constructor and 'Compile' member
// function for detailed usage.
//
// Note: 'Compile' implements the algorithm of Mohri and Sproat (1996),
// see comments at top of file for a link to that paper.
template <class A>
class CDRewriteRule {
 public:
  typedef A Arc;
  typedef typename A::Label Label;

  // Create object representing the context-dependent rewrite rule
  // 'phi' -> 'psi' | 'lamba' __ 'rho'. If 'phiXpsi' is true, 'psi' is
  // a transducer with input domain 'phi' instead of an acceptor.
  //
  // 'phi', 'lambda' and 'rho' need to be unweighted acceptors and
  // 'psi' a weighted transducer when 'phiXpsi' is true and a weighted
  // acceptor otherwise.
  CDRewriteRule(const Fst<Arc> &phi, const Fst<Arc> &psi,
                const Fst<Arc> &lambda, const Fst<Arc> &rho, bool phiXpsi)
      : phi_(phi.Copy()),
        psi_(psi.Copy()),
        lambda_(lambda.Copy()),
        rho_(rho.Copy()),
        phiXpsi_(phiXpsi) {}

  ~CDRewriteRule() {
    delete phi_;
    delete psi_;
    delete lambda_;
    delete rho_;
  }

  // Build the transducer 'fst' representing the context-dependent
  // rewrite rule. 'sigma' is an fst specifying (the closure of) the
  // alphabet for the resulting transducer. 'dir' can be
  // LEFT_TO_RIGHT, RIGHT_TO_LEFT or SIMULTANEOUS. 'mode' can be
  // OBLIGATORY or OPTIONAL. 'sigma' must be an unweighted acceptor
  // representing a bifix code.
  void Compile(const Fst<Arc> &sigma, MutableFst<Arc> *fst,
               CDRewriteDirection dir, CDRewriteMode mode);

 private:
  enum MarkerType { MARK = 1, CHECK = 2, CHECK_COMPLEMENT = 3};

  void MakeMarker(MutableFst<StdArc> *fst,
                  MarkerType type,
                  const vector<std::pair<Label, Label> > &markers);
  void IgnoreMarkers(MutableFst<Arc> *fst,
                     const vector<std::pair<Label, Label> > &markers);
  void AddMarkersToSigma(MutableFst<Arc> *sigma,
                         const vector<std::pair<Label, Label> > &markers);
  void AppendMarkers(MutableFst<Arc> *fst,
                     const vector<std::pair<Label, Label> > &markers);
  void PrependMarkers(MutableFst<Arc> *fst,
                      const vector<std::pair<Label, Label> > &markers);
  void MakeFilter(const Fst<Arc> &beta,
                  const Fst<Arc> &sigma,
                  MutableFst<Arc> *filter,
                  MarkerType type,
                  const vector<std::pair<Label, Label> > &markers,
                  bool reverse);
  void MakeReplace(MutableFst<Arc> *fst, const Fst<Arc> &sigma);
  static Label MaxLabel(const Fst<Arc> &fst);

  Fst<Arc>* phi_;
  Fst<Arc>* psi_;
  Fst<Arc>* lambda_;
  Fst<Arc>* rho_;
  bool phiXpsi_;
  CDRewriteDirection dir_;
  CDRewriteMode mode_;
  // The following labels are used to represent the symbols: <_1, <_2
  // and > in Mohri and Sproat. For instance, for left-to-right
  // obligatory rules, <_1 is used to mark the start of an occurence
  // of 'phi' that need to be rewritten, <_2 is used to mark the start
  // of an occurence of 'phi' that should not be rewritten and > is
  // used to mark the end of occurences of 'phi'.
  Label lbrace1_;
  Label lbrace2_;
  Label rbrace_;

  DISALLOW_COPY_AND_ASSIGN(CDRewriteRule);
};


// Turn fst into a marker transducer of specified type using the
// specified markers (markers) for the regular expression
// represented by fst, as specified in Mohri and Sproat (1996).
template <class Arc>
void CDRewriteRule<Arc>::MakeMarker(
    MutableFst<StdArc> *fst,
    MarkerType type,
    const vector<std::pair<Label, Label> > &markers) {
  typedef typename StdArc::StateId StateId;
  typedef typename StdArc::Weight Weight;

  if (!(fst->Properties(kAcceptor, true) & kAcceptor))
    LOG(FATAL) << "Marker: input fst needs to be an acceptor";

  StateId num_states = fst->NumStates();
  switch (type) {
    case MARK:
      // Type 1 in Mohri and Sproat:
      // Insert (or delete) markers after each match.
      for (StateId i = 0; i < num_states; ++i) {
        if (fst->Final(i) == Weight::Zero()) {
          fst->SetFinal(i, Weight::One());
        } else {
          StateId j = fst->AddState();
          fst->SetFinal(j, fst->Final(i));
          ArcIterator<Fst<StdArc> > arc_iter(*fst, i);
          for (; !arc_iter.Done(); arc_iter.Next())
            fst->AddArc(j, arc_iter.Value());
          fst->SetFinal(i, Weight::Zero());
          fst->DeleteArcs(i);
          for (ssize_t k = 0; k < markers.size(); ++k)
            fst->AddArc(i, StdArc(markers[k].first, markers[k].second,
                                  Weight::One(), j));
        }
      }
      break;
    case CHECK:
      // Type 2 in Mohri and Sproat:
      // Check that each marker is preceded by a match.
      for (StateId i = 0; i < num_states; ++i) {
        if (fst->Final(i) == Weight::Zero()) {
          fst->SetFinal(i, Weight::One());
        } else {
          for (ssize_t k = 0; k < markers.size(); ++k)
            fst->AddArc(i, StdArc(markers[k].first, markers[k].second,
                                  Weight::One(), i));
        }
      }
      break;
    case CHECK_COMPLEMENT:
      // Type 3 in Mohri and Sproat:
      // Check that each marker is not preceded by a match.
      for (StateId i = 0; i < num_states; ++i) {
        if (fst->Final(i) == Weight::Zero()) {
          fst->SetFinal(i, Weight::One());
          for (ssize_t k = 0; k < markers.size(); ++k)
            fst->AddArc(i, StdArc(markers[k].first, markers[k].second,
                                  Weight::One(), i));
        }
      }
      break;
  }
}


// Add self loops at all states in fst for allowing the markers
// specified by markers in any position.
// This corresponds to the subscripting in Mohri and Sproat (1996).
template <class Arc>
void CDRewriteRule<Arc>::IgnoreMarkers(MutableFst<Arc> *fst,
    const vector<std::pair<Label, Label> > &markers) {
  for (typename Arc::StateId i = 0; i < fst->NumStates(); ++i) {
    for (ssize_t k = 0; k < markers.size(); ++k)
      fst->AddArc(i, Arc(markers[k].first, markers[k].second,
                         Arc::Weight::One(), i));
  }
}


// Turn Sigma^* into (Sigma union markers)^*
template <class Arc>
void CDRewriteRule<Arc>::AddMarkersToSigma(MutableFst<Arc> *sigma,
    const vector<std::pair<Label, Label> > &markers) {
  for (typename Arc::StateId s = 0; s < sigma->NumStates(); ++s) {
    if (sigma->Final(s) != Arc::Weight::Zero()) {
      for (size_t k = 0; k < markers.size(); ++k)
        sigma->AddArc(s, Arc(markers[k].first, markers[k].second,
                             Arc::Weight::One(), sigma->Start()));
    }
  }
}

// Add loops at the initial state of fst for all alphabet symbols
// in the current alphabet (sigma).
template <class Arc>
inline void PrependSigmaStar(MutableFst<Arc> *fst,
                             const Fst<Arc> &sigma) {
  Concat(sigma, fst);
  RmEpsilon(fst);
}


// Append to fst a transition for each of the pairs (ilabel, olabel) specified
// by markers.
template <class Arc>
void CDRewriteRule<Arc>::AppendMarkers(MutableFst<Arc> *fst,
    const vector<std::pair<Label, Label> > &markers) {
  VectorFst<Arc> temp_fst;
  typename Arc::StateId start = temp_fst.AddState();
  typename Arc::StateId final = temp_fst.AddState();
  temp_fst.SetStart(start);
  temp_fst.SetFinal(final, Arc::Weight::One());
  for (ssize_t k = 0; k < markers.size(); ++k)
    temp_fst.AddArc(start, Arc(markers[k].first, markers[k].second,
                               Arc::Weight::One(), final));
  Concat(fst, temp_fst);
}


// Prepend to fst a transition for each of the pairs (ilabel, olabel) specified
// by markers.
template <class Arc>
void CDRewriteRule<Arc>::PrependMarkers(MutableFst<Arc> *fst,
    const vector<std::pair<Label, Label> > &markers) {
  if (fst->Start() == kNoStateId) fst->SetStart(fst->AddState());
  typename Arc::StateId new_start = fst->AddState(), old_start = fst->Start();
  fst->SetStart(new_start);
  for (ssize_t k = 0; k < markers.size(); ++k)
    fst->AddArc(new_start, Arc(markers[k].first, markers[k].second,
                               Arc::Weight::One(), old_start));
}


// Create in 'filter' the marker transducer of type 'type' for the markers
// defined in 'markers' for the regular expression sigma^* beta.  When
// reverse is true, the reverse of the marker transducer corresponding
// to sigma^* reverse(beta).
//
// The operations in MakeFilter do not depend on the semiring, and indeed for
// some semirings the various optimizations needed in MakeFilter cause
// problems. We therefore map incoming acceptors in whatever semiring to
// unweighted acceptors. Ideally this would be in the boolean, but we simulate
// it with the tropical.
template <class Arc>
void CDRewriteRule<Arc>::MakeFilter(const Fst<Arc> &beta,
    const Fst<Arc> &sigma, MutableFst<Arc> *filter, MarkerType type,
    const vector<std::pair<Label, Label> > &markers, bool reverse) {
  VectorFst<StdArc> ufilter;
  Map(beta, &ufilter, RmWeightMapper<Arc, StdArc>());
  VectorFst<StdArc> usigma;
  Map(sigma, &usigma, RmWeightMapper<Arc, StdArc>());
  if (ufilter.Start() == kNoStateId)
    ufilter.SetStart(ufilter.AddState());
  if (reverse) {
    Reverse(
        MapFst<StdArc, StdArc,
        IdentityMapper<StdArc> >(ufilter,
                                 IdentityMapper<StdArc>()),
        &ufilter);
    VectorFst<StdArc> reversed_sigma;
    Reverse(usigma, &reversed_sigma);
    RmEpsilon(&reversed_sigma);
    PrependSigmaStar<StdArc>(&ufilter, reversed_sigma);
  } else {
    PrependSigmaStar<StdArc>(&ufilter, usigma);
  }
  RmEpsilon(&ufilter);
  DeterminizeFst<StdArc> det(ufilter);
  Map(det, &ufilter, IdentityMapper<StdArc>());
  Minimize(&ufilter);
  MakeMarker(&ufilter, type, markers);
  if (reverse)
    Reverse(MapFst<StdArc, StdArc,
            IdentityMapper<StdArc> >(ufilter, IdentityMapper<StdArc>()),
            &ufilter);
  ArcSort(&ufilter, ILabelCompare<StdArc>());
  Map(ufilter, filter, RmWeightMapper<StdArc, Arc>());
}


// Turn fst representing phi X psi into a "replace" transducer as
// specified in Mohri and Sproat (1996).
template <class Arc>
void CDRewriteRule<Arc>::MakeReplace(MutableFst<Arc> *fst,
                                                   const Fst<Arc> &sigma) {
  typedef typename Arc::StateId StateId;
  typedef typename Arc::Weight Weight;

  Optimize(fst);
  if (fst->Start() == kNoStateId)
    fst->SetStart(fst->AddState());
  // Label pairs for to be added arcs to the initial state or from the
  // final states.
  std::pair<Label, Label> initial_pair;
  std::pair<Label, Label> final_pair;
  // Label for self-loops to be added at the new initial state (to be
  // created) and at every other state.
  vector<std::pair<Label, Label> > initial_loops;
  vector<std::pair<Label, Label> > all_loops;
  switch (mode_) {
    case OBLIGATORY:
      all_loops.push_back(std::make_pair(lbrace1_, 0));
      all_loops.push_back(std::make_pair(lbrace2_, 0));
      all_loops.push_back(std::make_pair(rbrace_, 0));
      switch (dir_) {
        case LEFT_TO_RIGHT:
          initial_pair = std::make_pair(lbrace1_, lbrace1_);
          final_pair = std::make_pair(rbrace_, 0);
          initial_loops.push_back(std::make_pair(lbrace2_, lbrace2_));
          initial_loops.push_back(std::make_pair(rbrace_, 0));
          break;
        case RIGHT_TO_LEFT:
          initial_pair = std::make_pair(rbrace_, 0);
          final_pair = std::make_pair(lbrace1_, lbrace1_);
          initial_loops.push_back(std::make_pair(lbrace2_, lbrace2_));
          initial_loops.push_back(std::make_pair(rbrace_, 0));
          break;
        case SIMULTANEOUS:
          initial_pair = std::make_pair(lbrace1_, 0);
          final_pair = std::make_pair(rbrace_, 0);
          initial_loops.push_back(std::make_pair(lbrace2_, 0));
          initial_loops.push_back(std::make_pair(rbrace_, 0));
          break;
      }
      break;
    case OPTIONAL:
      all_loops.push_back(std::make_pair(rbrace_, 0));
      initial_loops.push_back(std::make_pair(rbrace_, 0));
      switch (dir_) {
        case LEFT_TO_RIGHT:
          initial_pair = std::make_pair(0, lbrace1_);
          final_pair = std::make_pair(rbrace_, 0);
          break;
        case RIGHT_TO_LEFT:
          initial_pair = std::make_pair(rbrace_, 0);
          final_pair = std::make_pair(0, lbrace1_);
          break;
        case SIMULTANEOUS:
          initial_pair = std::make_pair(lbrace1_, 0);
          final_pair = std::make_pair(rbrace_, 0);
          break;
      }
      break;
  }
  // Add loops at all state of fst.
  IgnoreMarkers(fst, all_loops);
  // Create new initial and final states,
  StateId num_states = fst->NumStates();
  StateId start = fst->AddState();
  StateId final = fst->AddState();
  fst->AddArc(start, Arc(initial_pair.first, initial_pair.second,
                         Weight::One(), fst->Start()));
  // Make all final states non final with transition to new final state
  for (StateId i = 0; i < num_states; ++i) {
    if (fst->Final(i) == Weight::Zero()) continue;
    fst->AddArc(i, Arc(final_pair.first, final_pair.second,
                       fst->Final(i), final));
    fst->SetFinal(i, Weight::Zero());
  }
  fst->SetFinal(final, Weight::One());
  fst->SetFinal(start, Weight::One());
  fst->SetStart(start);
  // Add required loops at new initial state
  VectorFst<Arc> sigma_m(sigma);
  AddMarkersToSigma(&sigma_m, initial_loops);
  PrependSigmaStar<Arc>(fst, sigma_m);
  Closure(fst, CLOSURE_STAR);
  Optimize(fst);
  ArcSort(fst, ILabelCompare<Arc>());
}


template <class Arc>
typename Arc::Label CDRewriteRule<Arc>::MaxLabel(const Fst<Arc>& fst) {
  Label max = kNoLabel;
  StateIterator<Fst<Arc> > state_iter(fst);
  for (; !state_iter.Done(); state_iter.Next()) {
    ArcIterator<Fst<Arc> > arc_iter(fst, state_iter.Value());
    for (; !arc_iter.Done(); arc_iter.Next()) {
      if (arc_iter.Value().ilabel > max)
        max = arc_iter.Value().ilabel;
      if (arc_iter.Value().olabel > max)
        max = arc_iter.Value().olabel;
    }
  }
  return max;
}


// Build the transducer 'fst' representing the context-dependent
// rewrite rule. 'sigma' is an fst specifying (the closure of) the
// alphabet for the resulting transducer.  transducer. 'dir' can be
// LEFT_TO_RIGHT, RIGHT_TO_LEFT or SIMULTANEOUS. 'mode' can be
// OBLIGATORY or OPTIONAL.
//
// This function implements the algorithm of Mohri and Sproat (1996).
template <class Arc>
void CDRewriteRule<Arc>::Compile(
    const Fst<Arc> &sigma,
    MutableFst<Arc> *fst,
    CDRewriteDirection dir,
    CDRewriteMode mode) {
  dir_ = dir;
  mode_ = mode;

  uint64 props = kAcceptor | kUnweighted;
  if (phi_->Properties(props, true) != props)
    LOG(FATAL) << "CDRewriteRule::Compile: phi needs to be"
               << " an unweighted acceptor";
  if (lambda_->Properties(props, true) != props)
    LOG(FATAL) << "CDRewriteRule::Compile: lambda needs to be"
               << " an unweighted acceptor";
  if (rho_->Properties(props, true) != props)
    LOG(FATAL) << "CDRewriteRule::Compile: rho needs to be"
               << " an unweighted acceptor";
  if (!phiXpsi_ && (psi_->Properties(kAcceptor, true) != kAcceptor))
    LOG(FATAL) << "CDRewriteRule::Compile: psi needs to be"
               << " an acceptor or phiXpsi needs to be set to true";
  if (sigma.Properties(props, true) != props)
    LOG(FATAL) << "CDRewriteRule::Compile: sigma needs to be"
               << " an unweighted acceptor";

  rbrace_ = MaxLabel(sigma) + 1;
  lbrace1_ = rbrace_ + 1;
  lbrace2_ = rbrace_ + 2;
  vector<std::pair<Label, Label> > markers;
  VectorFst<Arc> sigma_rbrace(sigma);
  markers.push_back(std::make_pair(rbrace_, rbrace_));
  AddMarkersToSigma(&sigma_rbrace, markers);
  markers.clear();
  fst->DeleteStates();
  VectorFst<Arc> replace;
  if (phiXpsi_)
    Map(*psi_, &replace, IdentityMapper<Arc>());
  else
    Compose(
        MapFst<Arc, Arc, EpsMapper<Arc, EPS_ON_OUTPUT> >(
            *phi_, EpsMapper<Arc, EPS_ON_OUTPUT>()),
        MapFst<Arc, Arc, EpsMapper<Arc, EPS_ON_INPUT> >(
            *psi_, EpsMapper<Arc, EPS_ON_INPUT>()),
        &replace);
  MakeReplace(&replace, sigma);

  switch (dir_) {
    case LEFT_TO_RIGHT: {
      // Building r filter:
      VectorFst<Arc> r;
      markers.clear();
      markers.push_back(std::make_pair(0, rbrace_));
      MakeFilter(*rho_, sigma, &r, MARK, markers, true);
      switch (mode_) {
        case OBLIGATORY: {
          VectorFst<Arc> phi_rbrace;  // Append > after phi_, match all >
          Map(*phi_, &phi_rbrace, IdentityMapper<Arc>());
          markers.clear();
          markers.push_back(std::make_pair(rbrace_, rbrace_));
          IgnoreMarkers(&phi_rbrace, markers);
          AppendMarkers(&phi_rbrace, markers);
          // Building f filter:
          VectorFst<Arc> f;
          markers.clear();
          markers.push_back(std::make_pair(0, lbrace1_));
          markers.push_back(std::make_pair(0, lbrace2_));
          MakeFilter(phi_rbrace, sigma_rbrace, &f, MARK, markers, true);
          // Building l1 filter:
          VectorFst<Arc> l1;
          markers.clear();
          markers.push_back(std::make_pair(lbrace1_, 0));
          MakeFilter(*lambda_, sigma, &l1, CHECK, markers, false);
          markers.clear();
          markers.push_back(std::make_pair(lbrace2_, lbrace2_));
          IgnoreMarkers(&l1, markers);
          ArcSort(&l1, ILabelCompare<Arc>());
          // Building l2 filter:
          VectorFst<Arc> l2;
          markers.clear();
          markers.push_back(std::make_pair(lbrace2_, 0));
          MakeFilter(*lambda_, sigma, &l2, CHECK_COMPLEMENT, markers, false);
          // Building (((r o f) o replace) o l1) o l2
          VectorFst<Arc> c;
          Compose(r, f, &c);
          Compose(c, replace, fst);
          Compose(*fst, l1, &c);
          Compose(c, l2, fst);
          break;
        }
        case OPTIONAL: {
          // Building l filter
          VectorFst<Arc> l;
          markers.clear();
          markers.push_back(std::make_pair(lbrace1_, 0));
          MakeFilter(*lambda_, sigma, &l, CHECK, markers, false);
          // Building (r o replace) o l
          VectorFst<Arc> c;
          Compose(r, replace, &c);
          Compose(c, l, fst);
          break;
        }
      }
      break;
    }
    case RIGHT_TO_LEFT: {
      // Building l filter
      VectorFst<Arc> l;
      markers.clear();
      markers.push_back(std::make_pair(0, rbrace_));
      MakeFilter(*lambda_, sigma, &l, MARK, markers, false);
      switch (mode_) {
        case OBLIGATORY: {
          VectorFst<Arc> rbrace_phi;  // Prepend > before phi, match all >
          Map(*phi_, &rbrace_phi, IdentityMapper<Arc>());
          markers.clear();
          markers.push_back(std::make_pair(rbrace_, rbrace_));
          IgnoreMarkers(&rbrace_phi, markers);
          PrependMarkers(&rbrace_phi, markers);
          // Building f filter
          VectorFst<Arc> f;
          markers.clear();
          markers.push_back(std::make_pair(0, lbrace1_));
          markers.push_back(std::make_pair(0, lbrace2_));
          MakeFilter(rbrace_phi, sigma_rbrace, &f, MARK, markers, false);
          // Building r1 filter
          VectorFst<Arc> r1;
          markers.clear();
          markers.push_back(std::make_pair(lbrace1_, 0));
          MakeFilter(*rho_, sigma, &r1, CHECK, markers, true);
          markers.clear();
          markers.push_back(std::make_pair(lbrace2_, lbrace2_));
          IgnoreMarkers(&r1, markers);
          ArcSort(&r1, ILabelCompare<Arc>());
          // Building r2 filter
          VectorFst<Arc> r2;
          markers.clear();
          markers.push_back(std::make_pair(lbrace2_, 0));
          MakeFilter(*rho_, sigma, &r2, CHECK_COMPLEMENT, markers, true);
          // Building (((l o f) o replace) o r1) o r2
          VectorFst<Arc> c;
          Compose(l, f, &c);
          Compose(c, replace, fst);
          Compose(*fst, r1, &c);
          Compose(c, r2, fst);
          break;
        }
        case OPTIONAL: {
          // Building r filter
          VectorFst<Arc> r;
          markers.clear();
          markers.push_back(std::make_pair(lbrace1_, 0));
          MakeFilter(*rho_, sigma, &r, CHECK, markers, true);
          // Building (l o replace) o r
          VectorFst<Arc> c;
          Compose(l, replace, &c);
          Compose(c, r, fst);
          break;
        }
      }
      break;
    }
    case SIMULTANEOUS: {
      // Building r filter
      VectorFst<Arc> r;
      markers.clear();
      markers.push_back(std::make_pair(0, rbrace_));
      MakeFilter(*rho_, sigma, &r, MARK, markers, true);
      switch (mode_) {
        case OBLIGATORY: {
          VectorFst<Arc> phi_rbrace;  // Append > after phi, match all >
          Map(*phi_, &phi_rbrace, IdentityMapper<Arc>());
          markers.clear();
          markers.push_back(std::make_pair(rbrace_, rbrace_));
          IgnoreMarkers(&phi_rbrace, markers);
          AppendMarkers(&phi_rbrace, markers);
          // Building f filter
          VectorFst<Arc> f;
          markers.clear();
          markers.push_back(std::make_pair(0, lbrace1_));
          markers.push_back(std::make_pair(0, lbrace2_));
          MakeFilter(phi_rbrace, sigma_rbrace, &f, MARK, markers, true);
          // Building l1 filter
          VectorFst<Arc> l1;
          markers.clear();
          markers.push_back(std::make_pair(lbrace1_, lbrace1_));
          MakeFilter(*lambda_, sigma, &l1, CHECK, markers, false);
          markers.clear();
          markers.push_back(std::make_pair(lbrace2_, lbrace2_));
          markers.push_back(std::make_pair(rbrace_, rbrace_));
          IgnoreMarkers(&l1, markers);
          ArcSort(&l1, ILabelCompare<Arc>());
          // Building l2 filter
          VectorFst<Arc> l2;
          markers.clear();
          markers.push_back(std::make_pair(lbrace2_, lbrace2_));
          MakeFilter(*lambda_, sigma, &l2, CHECK_COMPLEMENT, markers, false);
          markers.clear();
          markers.push_back(std::make_pair(lbrace1_, lbrace1_));
          markers.push_back(std::make_pair(rbrace_, rbrace_));
          IgnoreMarkers(&l2, markers);
          ArcSort(&l2, ILabelCompare<Arc>());
          // Building (((r o f) o l1) o l2) o replace
          VectorFst<Arc> c;
          Compose(r, f, &c);
          Compose(c, l1, fst);
          Compose(*fst, l2, &c);
          Compose(c, replace, fst);
          break;
        }
        case OPTIONAL: {
          // Building l filter
          VectorFst<Arc> l;
          markers.clear();
          markers.push_back(std::make_pair(0, lbrace1_));
          MakeFilter(*lambda_, sigma, &l, CHECK, markers, false);
          markers.clear();
          markers.push_back(std::make_pair(rbrace_, rbrace_));
          IgnoreMarkers(&l, markers);
          ArcSort(&l, ILabelCompare<Arc>());
          // Building (r o l) o replace
          VectorFst<Arc> c;
          Compose(r, l, &c);
          Compose(c, replace, fst);
          break;
        }
      }
      break;
    }
  }
  Optimize(fst);
  ArcSort(fst, ILabelCompare<Arc>());
}


// Build the transducer 'fst' representing the context-dependent
// rewrite rule 'phi' -> 'psi' | 'lamba' __ 'rho'.  'sigma' is an fst
// specifying (the closure of) the alphabet for the resulting
// transducer. 'dir' can be LEFT_TO_RIGHT, RIGHT_TO_LEFT or
// SIMULTANEOUS. 'mode' can be OBLIGATORY or OPTIONAL. If 'phiXpsi' is
// true, 'psi' is a transducer with input domain 'phi' instead of an
// acceptor.
//
// 'phi', 'lambda', 'rho' and 'sigma' need to be unweighted acceptors
// and 'psi' a weighted transducer when 'phiXpsi' is true and a
// weighted acceptor otherwise. 'sigma' must represent a bifix code.
//
// This function implements the algorithm of Mohri and Sproat (1996).
template <class Arc>
void CDRewriteCompile(const Fst<Arc> &phi,
                                    const Fst<Arc> &psi,
                                    const Fst<Arc> &lambda,
                                    const Fst<Arc> &rho,
                                    const Fst<Arc> &sigma,
                                    MutableFst<Arc> *fst,
                                    CDRewriteDirection dir,
                                    CDRewriteMode mode,
                                    bool phiXpsi) {
  CDRewriteRule<Arc> cdrule(phi, psi, lambda, rho, phiXpsi);
  cdrule.Compile(sigma, fst, dir, mode);
}


// Build the transducer 'fst' representing the context-dependent
// rewrite rule 'phi' -> 'psi' | 'lamba' __ 'rho'.  'sigma' is an fst
// specifying (the closure of) the alphabet for the resulting
// transducer. 'dir' can be LEFT_TO_RIGHT, RIGHT_TO_LEFT or
// SIMULTANEOUS. 'mode' can be OBLIGATORY or OPTIONAL.
//
// 'phi', 'lambda', 'rho' and 'sigma' need to be unweighted acceptors
// and 'psi' a weighted acceptor.  'sigma' must represent a bifix
// code.
//
// This function implements the algorithm of Mohri and Sproat (1996).
template <class Arc>
void CDRewriteCompile(const Fst<Arc> &phi,
                                    const Fst<Arc> &psi,
                                    const Fst<Arc> &lambda,
                                    const Fst<Arc> &rho,
                                    const Fst<Arc> &sigma,
                                    MutableFst<Arc> *fst,
                                    CDRewriteDirection dir,
                                    CDRewriteMode mode) {
  CDRewriteCompile(phi, psi, lambda, rho, sigma, fst, dir, mode, false);
}


// Build the transducer 'fst' representing the context-dependent
// rewrite rule corresponding to applying the transductions 'tau' in
// the context 'lamba' __ 'rho'.  'sigma' is an fst specifying (the
// closure of) the alphabet for the resulting transducer. 'dir' can be
// LEFT_TO_RIGHT, RIGHT_TO_LEFT or SIMULTANEOUS. 'mode' can be
// OBLIGATORY or OPTIONAL.
//
// 'tau' need to be a weighted transducer, 'sigma', 'lambda' and 'rho'
// unweighted acceptors. 'sigma' must represent a bifix code.
//
// This function implements the algorithm of Mohri and Sproat (1996).
template <class Arc>
void CDRewriteCompile(const Fst<Arc> &tau,
                                    const Fst<Arc> &lambda,
                                    const Fst<Arc> &rho,
                                    const Fst<Arc> &sigma,
                                    MutableFst<Arc> *fst,
                                    CDRewriteDirection dir,
                                    CDRewriteMode mode) {
  VectorFst<Arc> phi(tau);
  Project(&phi, PROJECT_INPUT);
  Map(&phi, RmWeightMapper<Arc>());
  Optimize(&phi);
  CDRewriteCompile(phi, tau, lambda, rho, sigma, fst, dir, mode, true);
}

}  // namespace fst

#endif  // FST_GRAMMAR_CDREWRITE_CD_REWRITE_H_
