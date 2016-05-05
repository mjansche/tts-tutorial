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
// This is the main compiler class that takes in a source file and calls the
// parser to produce an AST and then walks that AST to generate the desired
// FSTs.  These FSTs are then loaded into a GrmManagerSpec.

#ifndef NLP_GRM_LANGUAGE_GRM_COMPILER_H_
#define NLP_GRM_LANGUAGE_GRM_COMPILER_H_

#include <iostream> // NOLINT
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/arc.h>
#include <thrax/node.h>
#include <thrax/grm-manager.h>
#include <thrax/lexer.h>
#include <thrax/evaluator.h>
#include <thrax/identifier-counter.h>
#include <thrax/printer.h>
#include <thrax/compat/stlfunctions.h>

DECLARE_bool(print_ast);
DECLARE_bool(line_numbers_in_ast);
DECLARE_string(indir);

namespace thrax {

class Namespace;
class Node;
template <typename Arc> class AstEvaluator;

// We must define a base class to be passed to the bison parser, which doesn't
// know about templates.
class GrmCompilerParserInterface {
 public:
  virtual ~GrmCompilerParserInterface() {}
  virtual void SetAst(Node* root) = 0;
  virtual Lexer* GetLexer() = 0;
  virtual void Error(const string& message) = 0;
};


template <typename Arc>
class GrmCompilerSpec : public GrmCompilerParserInterface {
 public:
  GrmCompilerSpec();
  ~GrmCompilerSpec();

  // ***************************************************************************
  // COMPILATION: These functions load up data into the GrmCompilerSpec.
  // Either:
  //   1.) call Parse*() followed by EvaluateAst(), or
  //   2.) load up an existing FST Archive by using LoadArchive().

  // Parses the provided grammar data via the filename or the file contents.
  // Defined in parser.y.  Returns true on success and false on failure.
  bool ParseFile(const string& filename);
  bool ParseContents(const string& contents);

  // Print the AST to stdout. Returns true if the AST is valid, false otherwise.
  // Prints the line numbers of the nodes if line_numbers is true.
  bool PrintAst(bool include_line_numbers);

  // Evaluate the AST from scratch, creating a new walker with no preset
  // environment.  Returns true on success and false on failure.
  bool EvaluateAst() { return EvaluateAstWithEnvironment(NULL, true); }

  // Evaluate the AST using the provided environment namespace.  This is likely
  // for imported files and modules and should really only be called by AST
  // walkers (i.e., not by users).  Call using NULL to create a new environment.
  // Returns true on success and false on failure.
  //
  // Boolean argument top_level indicates whether or not this is a top level
  // grammar file (i.e. not an imported grammar). This information gets passed
  // down ultimately to StringFst's GetLabelSymbolTable to determine (assuming
  // --save_symbols is set), whether or not to add generated labels to the byte
  // and utf8 symbol tables.
  bool EvaluateAstWithEnvironment(Namespace* env, bool top_level);

  // ***************************************************************************
  // The following functions give access to, modify, or serialize internal data.

  Lexer* GetLexer() { return &lexer_; }

  void SetAst(Node* root);           // Adds a new AST for this compiler.
  Node* GetAst() { return root_; }  // Gets the most recently AST.

  // Returns a pointer to the grammar manager so that we can perform rewrites
  // (or exports, or whatever).  This pointer remains owned by this class,
  // however, so it should not be deleted by the caller.
  const GrmManagerSpec<Arc>* GetGrmManager() const { return &grm_manager_; }

  // ***************************************************************************
  // Various other useful functions.

  // Sets the parsing to failure.  If provided with a non-empty message, then
  // we'll print that out for the user.  If the message is empty, print out
  // nothing (and just silently fail the parse/compile).
  void Error(const string& message);

 private:
  Lexer lexer_;

  vector<Node*> asts_;  // The list of actual ASTs owned by this compiler.
  Node* root_;          // A pointer to the most recent AST.

  GrmManagerSpec<Arc> grm_manager_;  // The manager that holds all of the FSTs.

  bool success_;

  string file_;  // File currently being processed

  DISALLOW_COPY_AND_ASSIGN(GrmCompilerSpec);
};

template <typename Arc>
GrmCompilerSpec<Arc>::GrmCompilerSpec()
    : root_(NULL) {}

template <typename Arc>
GrmCompilerSpec<Arc>::~GrmCompilerSpec() {
  STLDeleteContainerPointers(asts_.begin(), asts_.end());
}

template <typename Arc>
void GrmCompilerSpec<Arc>::SetAst(Node* root) {
  asts_.push_back(root);
  root_ = root;
}

template <typename Arc>
bool GrmCompilerSpec<Arc>::PrintAst(bool include_line_numbers) {
  if (!success_ || !root_) {
    return false;
  }
  AstPrinter printer;
  printer.include_line_numbers = include_line_numbers;
  root_->Accept(&printer);
  return true;
}

template <typename Arc>
bool GrmCompilerSpec<Arc>::EvaluateAstWithEnvironment(Namespace* env,
                                                      bool top_level) {
  if (!success_ || !root_) {
    int line_number = GetLexer()->line_number();
    std::cout << "****************************************\n";
    if (line_number == -1) {
      std::cout << "At end of file\n";
    } else {
      std::cout << "****************************************\n"
                << "Line " << GetLexer()->line_number() << "\n"
                << "Context: " << GetLexer()->GetCurrentContext() << std::endl;
    }
    return false;
  }
  if (FLAGS_print_ast) {
    PrintAst(FLAGS_line_numbers_in_ast);
  }
  VLOG(1) << "Commencing main compilation (AST evaluation).";
  AstEvaluator<Arc>* evaluator;
  if (env) {
    // If we have an environment, then we pass it to the Evaluator so that it
    // knows that we only want the includes.
    evaluator = new AstEvaluator<Arc>(env);
  } else {
    // We want to get a count of the identifiers so that we can free their
    // memory when the time comes.
    AstIdentifierCounter* id_counter = new AstIdentifierCounter();
    root_->Accept(id_counter);

    // If we don't have an environment, then we're doing the top level version,
    // where we execute the body.
    evaluator = new AstEvaluator<Arc>();
    evaluator->SetIdCounter(id_counter);
  }
  evaluator->set_file(file_);
  root_->Accept(evaluator);

  if (evaluator->Success()) {
    // We can always retrieve the FSTs.  If there are none (ex., since we're
    // only importing the file), this operation is still safe/fast.
    VLOG(1) << "Compilation complete.  Expanding exported FSTs.";
    evaluator->GetFsts(grm_manager_.GetFstMap(), top_level);
    grm_manager_.SortRuleInputLabels();
  } else {
    std::cout << "Compilation failed." << std::endl;
    success_ = false;
  }
  delete evaluator;
  return success_;
}

template <typename Arc>
void GrmCompilerSpec<Arc>::Error(const string& message) {
  success_ = false;
  if (!message.empty()) {
    std::cout << "****************************************\n" << file_ << ":"
              << GetLexer()->line_number() << ": " << message << "\n"
              << "Context: " << GetLexer()->GetCurrentContext() << std::endl;
  }
}

template <typename Arc>
bool GrmCompilerSpec<Arc>::ParseFile(const string &filename) {
  string local_grammar = JoinPath(FLAGS_indir, filename);
  VLOG(1) << "Parsing file: " << local_grammar;

  file_ = filename;
  string contents;
  ReadFileToStringOrDie(local_grammar, &contents);
  // Adds a newline in case one was left off. It doesn't hurt to have an extra
  // one (so not worth checking to see if one is already there), but the bison
  // parser fails for cryptic reasons if one is missing.
  contents += "\n";
  return ParseContents(contents);
}

// A wrapper for yyparse, just to avoid having to declare namespace
// thrax_rewriter here, and the various declarations for yyparse...
int CallParser(GrmCompilerParserInterface* compiler);

template <typename Arc>
bool GrmCompilerSpec<Arc>::ParseContents(const string& contents) {
  success_ = true;
  lexer_.ScanString(contents);
  CallParser(this);
  return success_;
}

// A lot of code outside this build uses GrmCompiler with the old meaning of
// GrmCompilerSpec<fst::StdArc>, forward-declaring it as a class. To obviate
// the need to change all that outside code, we provide this derived class:
class GrmCompiler : public GrmCompilerSpec<fst::StdArc> {};

}  // namespace thrax

#endif  // NLP_GRM_LANGUAGE_GRM_COMPILER_H_
