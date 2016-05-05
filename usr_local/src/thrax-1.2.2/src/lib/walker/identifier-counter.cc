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

#include <thrax/identifier-counter.h>

#include <unordered_map>
#include <string>

#include <thrax/collection-node.h>
#include <thrax/fst-node.h>
#include <thrax/grammar-node.h>
#include <thrax/identifier-node.h>
#include <thrax/rule-node.h>
#include <thrax/statement-node.h>
#include <thrax/compat/stlfunctions.h>

namespace thrax {

AstIdentifierCounter::AstIdentifierCounter()
    : next_identifier_exported_(false) {}
AstIdentifierCounter::~AstIdentifierCounter() {}

void AstIdentifierCounter::Visit(CollectionNode* node) {
  for (int i = 0; i < node->Size(); ++i)
    (*node)[i]->Accept(this);
}

void AstIdentifierCounter::Visit(FstNode* node) {
  for (int i = 0; i < node->NumArguments(); ++i)
    node->GetArgument(i)->Accept(this);
}

void AstIdentifierCounter::Visit(GrammarNode* node) {
  // We only care about the actual statements, and not the imports or functions.
  node->GetStatements()->Accept(this);
}

void AstIdentifierCounter::Visit(IdentifierNode* node) {
  if (node->HasNamespaces())  // We only care about local variables.
    return;

  const string& name = node->Get();
  if (next_identifier_exported_) {
    references_[name] = -1;    // -1 = infinite
  } else {
    int* count = InsertOrReturnExisting(&references_, name, 0);
    if (count && *count != -1)  // New insertions will return NULL, so we only
      ++(*count);               // update if we have a pre-existing value.
  }
}

void AstIdentifierCounter::Visit(RepetitionFstNode* node) {
  Visit(static_cast<FstNode*>(node));
}

void AstIdentifierCounter::Visit(RuleNode* node) {
  next_identifier_exported_ = node->ShouldExport();
  node->GetName()->Accept(this);
  next_identifier_exported_ = false;

  node->Get()->Accept(this);
}

void AstIdentifierCounter::Visit(StatementNode* node) {
  node->Get()->Accept(this);
}

void AstIdentifierCounter::Visit(StringFstNode* node) {
  Visit(static_cast<FstNode*>(node));
}

int AstIdentifierCounter::GetCount(const string& identifier) {
  return FindOrDie(references_, identifier);
}

bool AstIdentifierCounter::Decrement(const string& identifier) {
  std::unordered_map<string, int>::iterator where = references_.find(identifier);
  CHECK(where != references_.end())
      ;
  if (where->second < 0)
    return true;
  if (where->second == 0)
    return false;
  VLOG(3) << "Decrementing count for identifier " << identifier << " from "
          << where->second << " to " << where->second - 1 << ".";
  return --where->second;
}

}  // namespace thrax
