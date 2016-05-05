// Compiling a single arc specialization of CompileGrammar takes 60 seconds,
// so we split each arc type into a separate file.

#include <thrax/compiler.h>

namespace thrax {

template bool CompileGrammar<fst::LogArc>(const string&, const string&,
                                              bool, bool);

}  // namespace thrax
