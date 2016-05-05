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
// Return statements tell us what FST to return from a function, allowing
// binding to a variable in the main program.

#ifndef THRAX_RETURN_NODE_H_
#define THRAX_RETURN_NODE_H_

#include <string>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/node.h>

namespace thrax {

class AstWalker;
class IdentifierNode;

class ReturnNode : public Node {
 public:
  ReturnNode(Node* node);
  virtual ~ReturnNode();

  Node* Get() const;

  virtual void Accept(AstWalker* walker);

 private:
  Node* node_;

  DISALLOW_COPY_AND_ASSIGN(ReturnNode);
};

}  // namespace thrax

#endif  // THRAX_RETURN_NODE_H_
