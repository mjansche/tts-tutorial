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

#include <thrax/fst-node.h>

#include <string>
#include <vector>
using std::vector;

#include <thrax/string-node.h>
#include <thrax/walker.h>
#include <thrax/compat/stlfunctions.h>

namespace thrax {

FstNode::FstNode(FstNodeType type)
    : Node(), type_(type), weight_(NULL), optimize_(false) {}

FstNode::~FstNode() {
  STLDeleteContainerPointers(arguments_.begin(), arguments_.end());
  delete weight_;
}

void FstNode::AddArgument(Node* arg) {
  arguments_.push_back(arg);
}

bool FstNode::SetWeight(StringNode* weight) {
  if (weight_) {
    LOG(WARNING) << "Cannot clobber existing weight node.";
    delete weight;
    return false;
  }

  weight_ = weight;
  return true;
}

FstNode::FstNodeType FstNode::GetType() const {
  return type_;
}

int FstNode::NumArguments() const {
  return arguments_.size();
}

Node* FstNode::GetArgument(int index) const {
  return arguments_[index];
}

bool FstNode::HasWeight() const {
  return weight_;
}
const string& FstNode::GetWeight() const {
  return weight_->Get();
}

const bool FstNode::ShouldOptimize() const {
  return optimize_;
}
void FstNode::SetOptimize() {
  optimize_ = true;
}

void FstNode::Accept(AstWalker* walker) {
  walker->Visit(this);
}

const char* FstNode::FstNodeTypeToString(FstNodeType type) {
  switch (type) {
    case COMPOSITION_FSTNODE:  return "COMPOSITION_FSTNODE";
    case CONCAT_FSTNODE:       return "CONCAT_FSTNODE";
    case DIFFERENCE_FSTNODE:   return "DIFFERENCE_FSTNODE";
    case FUNCTION_FSTNODE:     return "FUNCTION_FSTNODE";
    case IDENTIFIER_FSTNODE:   return "IDENTIFIER_FSTNODE";
    case REPETITION_FSTNODE:   return "REPETITION_FSTNODE";
    case REWRITE_FSTNODE:      return "REWRITE_FSTNODE";
    case STRING_FSTNODE:       return "STRING_FSTNODE";
    case UNION_FSTNODE:        return "UNION_FSTNODE";
    case UNKNOWN_FSTNODE:      return "UNKNOWN_FSTNODE";
    default:                   LOG(FATAL) << "Invalid FstNodeType: " << type;
  }
}

StringFstNode::StringFstNode(ParseMode parse_mode)
    : FstNode(STRING_FSTNODE), parse_mode_(parse_mode) {}

StringFstNode::~StringFstNode() {}

StringFstNode::ParseMode StringFstNode::GetParseMode() const {
  return parse_mode_;
}

void StringFstNode::Accept(AstWalker* walker) {
  walker->Visit(this);
}

RepetitionFstNode::RepetitionFstNode(RepetitionFstNodeType type)
    : FstNode(REPETITION_FSTNODE), repetition_type_(type) {}

RepetitionFstNode::~RepetitionFstNode() {}

RepetitionFstNode::RepetitionFstNodeType RepetitionFstNode::GetRepetitionType()
    const {
  return repetition_type_;
}

void RepetitionFstNode::SetRange(int min, int max) {
  range_min_ = min;
  range_max_ = max;
}

void RepetitionFstNode::GetRange(int* min, int* max) const {
  CHECK(repetition_type_ == RANGE);
  *min = range_min_;
  *max = range_max_;
}

void RepetitionFstNode::Accept(AstWalker* walker) {
  walker->Visit(this);
}

const char* RepetitionFstNode::RepetitionFstNodeTypeToString(
    RepetitionFstNodeType type) {
  switch (type) {
    case STAR:      return "STAR";
    case PLUS:      return "PLUS";
    case QUESTION:  return "QUESTION";
    case RANGE:     return "RANGE";
    default:        LOG(FATAL) << "Invalid RepetitionFstNodeType: " << type;
  }
}

}  // namespace thrax
