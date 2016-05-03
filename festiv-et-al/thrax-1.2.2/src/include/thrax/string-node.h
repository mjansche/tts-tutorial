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
// Basic node wrapper around a text string.

#ifndef THRAX_STRING_NODE_H_
#define THRAX_STRING_NODE_H_

#include <string>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/node.h>

namespace thrax {

class AstWalker;

class StringNode : public Node {
 public:
  StringNode(const string& str);
  virtual ~StringNode();

  const string& Get() const;

  virtual void Accept(AstWalker* walker);

 private:
  string str_;

  DISALLOW_COPY_AND_ASSIGN(StringNode);
};

}  // namespace thrax

#endif  // THRAX_STRING_NODE_H_
