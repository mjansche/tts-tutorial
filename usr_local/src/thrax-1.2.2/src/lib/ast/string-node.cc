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

#include <thrax/string-node.h>

#include <string>

#include <thrax/walker.h>

namespace thrax {

StringNode::StringNode(const string& str)
    : Node(), str_(str) {}

StringNode::~StringNode() {}

const string& StringNode::Get() const {
  return str_;
}

void StringNode::Accept(AstWalker* walker) {
  walker->Visit(this);
}

}  // namespace thrax
