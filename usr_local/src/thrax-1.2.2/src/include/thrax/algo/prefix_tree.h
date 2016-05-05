#ifndef THRAX_ALGO_PREFIX_TREE_H_
#define THRAX_ALGO_PREFIX_TREE_H_

#include <map>
#include <stack>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/arc.h>
#include <fst/vector-fst.h>
#include <thrax/compat/stlfunctions.h>

namespace fst {

// This class is neither thread-safe nor thread-hostile.
template <class Arc>
class PrefixTree {
 public:
  typedef typename Arc::Label Label;
  typedef typename Arc::StateId StateId;
  typedef typename Arc::Weight Weight;

  struct INode;
  struct ONode;

  using ichild_map = map<Label, INode *>;
  using ochild_map = map<Label, ONode *>;

  // Prefix tree node for the input labels of the FST.
  struct INode {
    INode() : output(nullptr), state(kNoStateId) {}

    ichild_map children;
    ONode *output;
    StateId state;
  };

  // Prefix tree node for the output labels of the FST.
  struct ONode {
    ONode() : weight(Weight::Zero()), state(kNoStateId) {}

    ochild_map children;
    Weight weight;
    StateId state;
  };

  PrefixTree() : num_states_(0), root_(nullptr) {}

  // Disallows copy and assignment construction.
  PrefixTree(const PrefixTree &) = delete;
  PrefixTree& operator=(const PrefixTree &) = delete;

  ~PrefixTree() { Clear(); }

  StateId NumStates() const { return num_states_; }

  // Add an entry to the prefix tree, consisting of two label sequences and a
  // weight.  Each label sequence must be provided as a pair of iterators.
  template <class Iterator1, class Iterator2>
  void Add(Iterator1 iter1, Iterator1 end1,
           Iterator2 iter2, Iterator2 end2,
           const Weight &weight = Weight::One()) {
    if (!root_) {
      CHECK_EQ(0, num_states_);
      root_ = new INode();
      root_->state = num_states_++;
    }
    INode *n = root_;
    for (/* empty */; iter1 != end1; ++iter1) {
      if (!*iter1)  // Skips over epsilons.
        continue;
      n = LookupOrInsertNew(&n->children, *iter1);
      if (kNoStateId == n->state)
        n->state = num_states_++;
    }
    if (!n->output) {
      n->output = new ONode();
      n->output->state = num_states_++;
    }
    ONode *o = n->output;
    for (/* empty */; iter2 != end2; ++iter2) {
      if (!*iter2)  // Skips over epsilons.
        continue;
      o = LookupOrInsertNew(&o->children, *iter2);
      if (kNoStateId == o->state)
        o->state = num_states_++;
    }
    o->weight = Plus(o->weight, weight);
  }

  // Removes all elements from this prefix tree.
  void Clear() {
    if (!root_) {
      CHECK_EQ(0, num_states_);
      return;
    }
    stack<INode *> iq;
    stack<ONode *> oq;
    // First, perform a simple depth-first traversal over the input trie,
    // starting at the root node.  Node coloring is not needed, since we're
    // dealing with a tree.
    iq.push(root_);
    while (!iq.empty()) {
      INode *n = iq.top();
      iq.pop();
      if (n->output)
        oq.push(n->output);  // Found a root node of an output trie.
      for (auto iter = n->children.begin(); iter != n->children.end(); ++iter) {
        iq.push(iter->second);
      }
      delete n;
    }
    // Second, perform simple depth-first traversals over the output tries,
    // starting at their root nodes.
    while (!oq.empty()) {
      ONode *o = oq.top();
      oq.pop();
      for (auto oter = o->children.begin(); oter != o->children.end(); ++oter) {
        oq.push(oter->second);
      }
      delete o;
    }
    num_states_ = 0;
    root_ = nullptr;
  }

  // Write the current prefix tree transducer to 'fst'.
  void ToFst(MutableFst<Arc> *fst) {
    fst->DeleteStates();
    if (num_states_ == 0) {
      CHECK(!root_);
      return;
    }
    // For the creation of the FST to be efficient, we must reserve enough space
    // for the states and arcs, to avoid reallocation and internal copying.
    fst->ReserveStates(num_states_);
    for (auto i = 0; i < num_states_; ++i)
      fst->AddState();
    fst->SetStart(root_->state);
    stack<INode *> iq;
    stack<ONode *> oq;
    iq.push(root_);
    while (!iq.empty()) {
      INode *n = iq.top();
      iq.pop();
      StateId q = n->state;
      CHECK_NE(kNoStateId, q);
      ONode *o = n->output;
      fst->ReserveArcs(q, (o ? 1 : 0) + n->children.size());
      if (o) {
        fst->AddArc(q, Arc(0, 0, Arc::Weight::One(), o->state));
        oq.push(o);
      }
      for (auto iter = n->children.begin(); iter != n->children.end(); ++iter) {
        fst->AddArc(q, Arc(iter->first, 0, Arc::Weight::One(),
                           iter->second->state));
        iq.push(iter->second);
      }
    }
    while (!oq.empty()) {
      ONode *o = oq.top();
      oq.pop();
      StateId q = o->state;
      CHECK_NE(kNoStateId, q);
      for (auto oter = o->children.begin(); oter != o->children.end(); ++oter) {
        fst->AddArc(q, Arc(0, oter->first, Arc::Weight::One(),
                           oter->second->state));
        oq.push(oter->second);
      }
      fst->SetFinal(q, o->weight);
    }
  }

 private:
  StateId num_states_;
  INode *root_;
};

}  // namespace fst

#endif  // THRAX_ALGO_PREFIX_TREE_H_
