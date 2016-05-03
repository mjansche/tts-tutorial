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

#include <thrax/import-node.h>

#include <thrax/identifier-node.h>
#include <thrax/string-node.h>
#include <thrax/walker.h>

namespace thrax {

ImportNode::ImportNode(StringNode* path, IdentifierNode* alias)
    : Node(), path_(path), alias_(alias) {}

ImportNode::~ImportNode() {
  delete path_;
  delete alias_;
}

StringNode* ImportNode::GetPath() const {
  return path_;
}

IdentifierNode* ImportNode::GetAlias() const {
  return alias_;
}

void ImportNode::Accept(AstWalker* walker) {
  walker->Visit(this);
}

}  // namespace thrax
