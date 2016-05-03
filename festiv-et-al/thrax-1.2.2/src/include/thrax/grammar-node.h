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
// The main grammar is the top node (i.e., root) of the AST.  A grammar consists
// of a list of imports, a list of functions, and a list of body functions.

#ifndef THRAX_GRAMMAR_NODE_H_
#define THRAX_GRAMMAR_NODE_H_

#include <string>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/node.h>

namespace thrax {

class AstWalker;
class CollectionNode;

class GrammarNode : public Node {
 public:
  GrammarNode(CollectionNode* imports,
              CollectionNode* functions,
              CollectionNode* statements);
  virtual ~GrammarNode();

  CollectionNode* GetImports() const;
  CollectionNode* GetFunctions() const;
  CollectionNode* GetStatements() const;

  virtual void Accept(AstWalker* walker);

 private:
  CollectionNode* imports_;
  CollectionNode* functions_;
  CollectionNode* statements_;

  DISALLOW_COPY_AND_ASSIGN(GrammarNode);
};

}  // namespace thrax

#endif  // THRAX_GRAMMAR_NODE_H_
