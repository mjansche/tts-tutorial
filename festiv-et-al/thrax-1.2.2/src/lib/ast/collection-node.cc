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

#include <thrax/collection-node.h>

#include <deque>

#include <thrax/walker.h>
#include <thrax/compat/stlfunctions.h>

namespace thrax {

CollectionNode::CollectionNode()
    : Node(), collection_(new Collection()) {}

CollectionNode::~CollectionNode() {
  if (collection_) {
    STLDeleteContainerPointers(collection_->begin(), collection_->end());
    delete collection_;
  }
}

void CollectionNode::AddFront(Node* node) {
  collection_->push_front(node);
}

int CollectionNode::Size() const {
  return collection_->size();
}

Node* CollectionNode::Get(int index) const {
  return (*collection_)[index];
}

Node* CollectionNode::operator[](int index) const {
  return Get(index);
}

void CollectionNode::Accept(AstWalker* walker) {
  walker->Visit(this);
}

}  // namespace thrax
