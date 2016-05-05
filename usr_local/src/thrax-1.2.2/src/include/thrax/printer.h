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
// A AST walker that prints out each node as it visits it.

#ifndef THRAX_PRINTER_H_
#define THRAX_PRINTER_H_

#include <iostream>
#include <string>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/node.h>
#include <thrax/walker.h>

namespace thrax {

class CollectionNode;
class FstNode;
class FunctionNode;
class GrammarNode;
class IdentifierNode;
class ImportNode;
class RepetitionFstNode;
class ReturnNode;
class RuleNode;
class StatementNode;
class StringFstNode;
class StringNode;

class AstPrinter : public AstWalker {
 public:
  AstPrinter();
  explicit AstPrinter(ostream& output_stream);
  virtual ~AstPrinter();

  virtual void Visit(CollectionNode* node);
  virtual void Visit(FstNode* node);
  virtual void Visit(FunctionNode* node);
  virtual void Visit(GrammarNode* node);
  virtual void Visit(IdentifierNode* node);
  virtual void Visit(ImportNode* node);
  virtual void Visit(RepetitionFstNode* node);
  virtual void Visit(ReturnNode* node);
  virtual void Visit(RuleNode* node);
  virtual void Visit(StatementNode* node);
  virtual void Visit(StringFstNode* node);
  virtual void Visit(StringNode* node);

  // If true, put line numbers into ast.
  bool include_line_numbers = false;

 private:
  // Returns the spacing prefix for the current nested level.
  // Plus the line number if line_numbers is true.
  string Spaces(Node *node) const;

  // A scoped counter that increments and decrements the spacing as necessary.
  class ScopedSpaceCounter {
   public:
    ScopedSpaceCounter(int* num_spaces);
    ~ScopedSpaceCounter();

   private:
    int* num_spaces_;
  };

  int num_spaces_;

  // This is the actual stream to which we write out, which we do not own.
  ostream& out;

  DISALLOW_COPY_AND_ASSIGN(AstPrinter);
};

}  // namespace thrax

#endif  // THRAX_PRINTER_H_
