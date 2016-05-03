// Assert the first argument is null.
//
// Thus for example if I have a transducer "trans", I might test if applying
// "trans" to "foo" is null:
//
// equality = AssertNull[foo @ trans];

#ifndef THRAX_ASSERT_NULL_H_
#define THRAX_ASSERT_NULL_H_

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
class AssertNull : public UnaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  AssertNull() {}
  virtual ~AssertNull() {}

 protected:
  virtual Transducer* UnaryFstExecute(const Transducer& left,
                                      const vector<DataType*>& args) {
    if (args.size() != 1) {
      std::cout << "AssertNull: Expected 1 argument but got "
                << args.size() << std::endl;
      return nullptr;
    }

    MutableTransducer* mutable_left = new MutableTransducer(left);
    fst::Project(mutable_left, fst::PROJECT_OUTPUT);
    fst::RmEpsilon(mutable_left);
    if (mutable_left->NumStates() != 0) {
      std::cout << "Argument to AssertNull is not null:"
                << std::endl;
      delete mutable_left;
      return nullptr;
    }
    return mutable_left;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(AssertNull<Arc>);
};


}  // namespace function
}  // namespace thrax

#endif  // THRAX_ASSERT_NULL_H_
