#ifndef NLP_GRM_LANGUAGE_UTIL_REWRITE_TESTER_UTILS_H_
#define NLP_GRM_LANGUAGE_UTIL_REWRITE_TESTER_UTILS_H_

#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/arc.h>
#include <fst/fst.h>
#include <fst/string.h>
#include <fst/symbol-table.h>
#include <thrax/grm-manager.h>
#include <../bin/utildefs.h>

class RewriteTesterUtils {
  typedef fst::StringCompiler<fst::StdArc> Compiler;

 public:
  RewriteTesterUtils();
  ~RewriteTesterUtils();

  void Initialize();
  void Run();
  // Runs the input through the FSTs. Prepends "Output string:" to each line if
  // prepend_output is true
  const string ProcessInput(const string& input, bool prepend_output);
  // Default version with prepend_output=true
  const string ProcessInput(const string& input);

 private:
  // Sorts the output by increasing cost (Tropical semiring)
  static bool SortOutput(std::pair<string, float> i, std::pair<string, float> j);

  // Reader for the input in interactive version.
  bool ReadInput(string* s);

  GrmManagerSpec<StdArc> grm_;
  vector<string> rules_;
  Compiler* compiler_;
  fst::SymbolTable* byte_symtab_;
  fst::SymbolTable* utf8_symtab_;
  const SymbolTable* generated_symtab_;
  SymbolTable* input_symtab_;
  TokenType type_;
  SymbolTable* output_symtab_;

  DISALLOW_COPY_AND_ASSIGN(RewriteTesterUtils);
};  // class RewriteTesterUtils


#endif  // NLP_GRM_LANGUAGE_UTIL_REWRITE_TESTER_UTILS_H_
