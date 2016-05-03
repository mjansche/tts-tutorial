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

#include <thrax/printer.h>

#include <iostream>
#include <string>
#include <sstream>

#include <thrax/collection-node.h>
#include <thrax/fst-node.h>
#include <thrax/function-node.h>
#include <thrax/grammar-node.h>
#include <thrax/identifier-node.h>
#include <thrax/import-node.h>
#include <thrax/node.h>
#include <thrax/return-node.h>
#include <thrax/rule-node.h>
#include <thrax/statement-node.h>
#include <thrax/string-node.h>

namespace thrax {

AstPrinter::AstPrinter() : num_spaces_(0), out(std::cout) {}

AstPrinter::AstPrinter(ostream& output_stream)
    : num_spaces_(0), out(output_stream) {}

AstPrinter::~AstPrinter() {}

void AstPrinter::Visit(CollectionNode* node) {
  out << Spaces(node) << "CollectionNode" << std::endl;
  ScopedSpaceCounter ssc(&num_spaces_);
  for (int i = 0; i < node->Size(); ++i)
    (*node)[i]->Accept(this);
}

void AstPrinter::Visit(FstNode* node) {
  out << Spaces(node) << "FstNode" << std::endl;
  ScopedSpaceCounter ssc(&num_spaces_);
  out << Spaces(node)
      << "Type: " << FstNode::FstNodeTypeToString(node->GetType()) << std::endl;

  // Handle subtype specific logic.
  if (node->GetType() == FstNode::STRING_FSTNODE) {
    StringFstNode* snode = static_cast<StringFstNode*>(node);
    out << Spaces(node) << "Parsing: ";
    if (snode->GetParseMode() == StringFstNode::BYTE)
      out << "BYTE" << std::endl;
    else if (snode->GetParseMode() == StringFstNode::UTF8)
      out << "UTF8" << std::endl;
    else
      out << "SYMBOL_TABLE" << std::endl;
  } else if (node->GetType() == FstNode::REPETITION_FSTNODE) {
    RepetitionFstNode* rnode = static_cast<RepetitionFstNode*>(node);
    out << Spaces(node)
        << "Subtype: " << RepetitionFstNode::RepetitionFstNodeTypeToString(
                              rnode->GetRepetitionType()) << std::endl;
    if (rnode->GetRepetitionType() == RepetitionFstNode::RANGE) {
      int min, max;
      rnode->GetRange(&min, &max);
      out << Spaces(node) << "Range: " << min << " to " << max << std::endl;
    }
  }

  if (node->NumArguments() > 0) {
    out << Spaces(node) << "Arguments:" << std::endl;
    for (int i = 0; i < node->NumArguments(); ++i)
      node->GetArgument(i)->Accept(this);
  }
  if (node->HasWeight())
    out << Spaces(node) << "Weight: " << node->GetWeight() << std::endl;
}

void AstPrinter::Visit(FunctionNode* node) {
  out << Spaces(node) << "FunctionNode" << std::endl;
  ScopedSpaceCounter ssc(&num_spaces_);
  node->GetName()->Accept(this);
  node->GetArguments()->Accept(this);
  node->GetBody()->Accept(this);
}

void AstPrinter::Visit(GrammarNode* node) {
  out << Spaces(node) << "GrammarNode" << std::endl;
  ScopedSpaceCounter ssc(&num_spaces_);
  out << Spaces(node) << "Imports:" << std::endl;
  node->GetImports()->Accept(this);
  out << Spaces(node) << "Functions:" << std::endl;
  node->GetFunctions()->Accept(this);
  out << Spaces(node) << "Statements:" << std::endl;
  node->GetStatements()->Accept(this);
}

void AstPrinter::Visit(IdentifierNode* node) {
  out << Spaces(node) << "IdentifierNode: " << node->Get() << std::endl;
}

void AstPrinter::Visit(ImportNode* node) {
  out << Spaces(node) << "ImportNode" << std::endl;
  ScopedSpaceCounter ssc(&num_spaces_);
  out << Spaces(node) << "Path:" << std::endl;
  node->GetPath()->Accept(this);
  out << Spaces(node) << "Alias:" << std::endl;
  node->GetAlias()->Accept(this);
}

void AstPrinter::Visit(RepetitionFstNode* node) {
  // Use the base FstNode version.
  Visit(static_cast<FstNode*>(node));
}

void AstPrinter::Visit(ReturnNode* node) {
  out << Spaces(node) << "ReturnNode" << std::endl;
  ScopedSpaceCounter ssc(&num_spaces_);
  node->Get()->Accept(this);
}

void AstPrinter::Visit(RuleNode* node) {
  out << Spaces(node) << "RuleNode"
      << (node->ShouldExport() ? " (exported)" : "") << std::endl;
  ScopedSpaceCounter ssc(&num_spaces_);
  node->GetName()->Accept(this);
  node->Get()->Accept(this);
}

void AstPrinter::Visit(StatementNode* node) {
  out << Spaces(node) << "StatementNode" << std::endl;
  ScopedSpaceCounter ssc(&num_spaces_);
  node->Get()->Accept(this);
}

void AstPrinter::Visit(StringFstNode* node) {
  // Use the base FstNode version.
  Visit(static_cast<FstNode*>(node));
}

void AstPrinter::Visit(StringNode* node) {
  out << Spaces(node) << "StringNode: " << node->Get() << std::endl;
}

string AstPrinter::Spaces(Node *node) const {
  string output;
  for (int i = 0; i < num_spaces_; ++i) {
    if (i != num_spaces_ - 1)
      output += "|   ";
    else
      output += "|-- ";
  }
  if (include_line_numbers) {
    output += std::to_string(node->getline()) + "-";
  }
  return output;
}

AstPrinter::ScopedSpaceCounter::ScopedSpaceCounter(int* num_spaces)
    : num_spaces_(num_spaces) {
  *num_spaces_ += 1;
}

AstPrinter::ScopedSpaceCounter::~ScopedSpaceCounter() {
  *num_spaces_ -= 1;
}

}  // namespace thrax
