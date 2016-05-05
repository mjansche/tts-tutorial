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
// This houses a list of other nodes.  During construction, we'll create them by
// pushing onto the front of the list.

#ifndef THRAX_COLLECTION_NODE_H_
#define THRAX_COLLECTION_NODE_H_

#include <deque>
#include <string>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/node.h>

namespace thrax {

class AstWalker;

class CollectionNode : public Node {
 public:
  typedef deque<Node*> Collection;

  CollectionNode();
  virtual ~CollectionNode();

  // Add a collection to the list.
  void AddFront(Node* node);

  int Size() const;

  Node* Get(int index) const;
  Node* operator[](int index) const;

  virtual void Accept(AstWalker* walker);

 private:
  Collection* collection_;

  DISALLOW_COPY_AND_ASSIGN(CollectionNode);
};

}  // namespace thrax

#endif  // THRAX_COLLECTION_NODE_H_
