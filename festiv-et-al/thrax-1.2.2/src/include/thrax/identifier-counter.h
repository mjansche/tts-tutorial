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
// A AST walker that counts the references to base level identifiers, not
// including the very first time when it's declared/defined.

#ifndef THRAX_IDENTIFIER_COUNTER_H_
#define THRAX_IDENTIFIER_COUNTER_H_

#include <unordered_map>
#include <string>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
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

class AstIdentifierCounter : public AstWalker {
 public:
  AstIdentifierCounter();
  virtual ~AstIdentifierCounter();

  virtual void Visit(CollectionNode* node);
  virtual void Visit(FstNode* node);
  virtual void Visit(GrammarNode* node);
  virtual void Visit(IdentifierNode* node);
  virtual void Visit(RepetitionFstNode* node);
  virtual void Visit(RuleNode* node);
  virtual void Visit(StatementNode* node);
  virtual void Visit(StringFstNode* node);

  // The following functions have no useful work to be done, either because they
  // have no sub-nodes or because we don't care about their contents for
  // identifier counting.
  virtual void Visit(FunctionNode* node) { return; }
  virtual void Visit(ImportNode* node)   { return; }
  virtual void Visit(ReturnNode* node)   { return; }
  virtual void Visit(StringNode* node)   { return; }

  // Returns the number of times the identifier was referenced and -1 if the
  // provided name is exported (with an infinite reference count).  Crashes if
  // the provided name isn't found.
  int GetCount(const string& identifier);

  // Decrements the count of the identifier and returns true if there're still
  // available references remaining (and false otherwise).  Crashes if the
  // provided identifier isn't found.
  bool Decrement(const string& identifier);

 private:
  // A map of references from the identifier name to the number of times it's
  // used.
  std::unordered_map<string, int> references_;

  // A boolean to tell us whether the next IdentifierNode we encounter is
  // exported (and thus should have a reference count of infinity (-1)).
  bool next_identifier_exported_;

  DISALLOW_COPY_AND_ASSIGN(AstIdentifierCounter);
};

}  // namespace thrax

#endif  // THRAX_IDENTIFIER_COUNTER_H_
