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

#include <thrax/rule-node.h>

#include <thrax/identifier-node.h>
#include <thrax/walker.h>

namespace thrax {

RuleNode::RuleNode(IdentifierNode* name, Node* rhs, ExportStatus exp)
    : Node(), name_(name), rhs_(rhs), export_(exp) {}

RuleNode::~RuleNode() {
  delete name_;
  delete rhs_;
}

IdentifierNode* RuleNode::GetName() const {
  return name_;
}

Node* RuleNode::Get() const {
  return rhs_;
}

bool RuleNode::ShouldExport() const {
  return export_ == EXPORT;
}

void RuleNode::Accept(AstWalker* walker) {
  walker->Visit(this);
}

}  // namespace thrax
