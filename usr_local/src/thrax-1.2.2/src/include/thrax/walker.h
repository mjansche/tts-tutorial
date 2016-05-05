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

#ifndef THRAX_WALKER_H_
#define THRAX_WALKER_H_

#include <fst/compat.h>
#include <thrax/compat/compat.h>

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

class AstWalker {
 public:
  AstWalker();
  virtual ~AstWalker();

  virtual void Visit(CollectionNode* node) = 0;
  virtual void Visit(FstNode* node) = 0;
  virtual void Visit(FunctionNode* node) = 0;
  virtual void Visit(GrammarNode* node) = 0;
  virtual void Visit(IdentifierNode* node) = 0;
  virtual void Visit(ImportNode* node) = 0;
  virtual void Visit(RepetitionFstNode* node) = 0;
  virtual void Visit(ReturnNode* node) = 0;
  virtual void Visit(RuleNode* node) = 0;
  virtual void Visit(StatementNode* node) = 0;
  virtual void Visit(StringFstNode* node) = 0;
  virtual void Visit(StringNode* node) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(AstWalker);
};

}  // namespace thrax

#endif  // THRAX_WALKER_H_
