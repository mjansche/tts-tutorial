// Assert the first argument is the empty transuducer.
//
// Thus for example if I have a transducer "trans", I might test if applying
// "trans" to "foo" is empty:
//
// equality = AssertEmpty[foo @ trans];

#ifndef THRAX_ASSERT_EMPTY_H_
#define THRAX_ASSERT_EMPTY_H_

#include <iostream>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/arc-map.h>
#include <fst/determinize.h>
#include <fst/equivalent.h>
#include <fst/project.h>
#include <fst/rmepsilon.h>
#include <fst/shortest-path.h>
#include <fst/string.h>
#include <thrax/datatype.h>
#include <thrax/function.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.

// TODO(rws): some day we should make this so that it doesn't return a
// value, but merely runs the assertion. That, however, would require changing
// the parser.

namespace thrax {
namespace function {

template <typename Arc>
class AssertEmpty : public UnaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  AssertEmpty() {}
  ~AssertEmpty() override {}

 protected:
  Transducer* UnaryFstExecute (const Transducer& left,
                               const vector<DataType*>& args) override {
    if (args.size() != 1) {
      std::cout << "AssertEmpty: Expected 1 argument but got "
                << args.size() << std::endl;
      return nullptr;
    }

    MutableTransducer* mutable_left = new MutableTransducer(left);
    fst::Project(mutable_left, fst::PROJECT_OUTPUT);
    fst::RmEpsilon(mutable_left);
    if (mutable_left->NumStates() == 1 &&
        mutable_left->NumArcs(0) == 0 &&
        mutable_left->Final(0) != Arc::Weight::Zero()) {
      return mutable_left;
    } else {
      std::cout << "Argument to AssertEmpty is not empty:"
                << std::endl;
      delete mutable_left;
      return nullptr;
    }
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(AssertEmpty<Arc>);
};


}  // namespace function
}  // namespace thrax

#endif  // THRAX_ASSERT_EMPTY_H_
