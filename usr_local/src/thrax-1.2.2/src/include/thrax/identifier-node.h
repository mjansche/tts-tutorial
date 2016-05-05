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
// An identifier is a variable name, essentially.  This node parses module
// namespaces, splitting on the dots and provides iteration capabilities over
// the identifier.

#ifndef THRAX_IDENTIFIER_NODE_H_
#define THRAX_IDENTIFIER_NODE_H_

#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/node.h>

namespace thrax {

class AstWalker;

class IdentifierNode : public Node {
 public:
  explicit IdentifierNode(const string& name);
  IdentifierNode(const string& name, int begin_pos);
  virtual ~IdentifierNode();

  // Return the entire identifier as originally used in the source.
  const string& Get() const;

  // Returns the actual identifier - the last component.  For example, if the
  // identifier is foo.bar.baz, this will return baz.
  const string& GetIdentifier() const;

  // Returns the beginning byte position of the identifier in the source.
  int GetBeginPos() const;

  // Returns true if there are any namespace qualifiers and false otherwise.
  bool HasNamespaces() const;

  // STL-style iterator-based accessors of the namespace list.
  typedef vector<string>::const_iterator const_iterator;
  const_iterator begin() const;
  const_iterator end() const;

  // Returns true if the identifier provided is valid.  We check for:
  //   - No empty components.
  //   - No components that are fully numeric or fully underscore.
  //   - Contains no invalid characters (only underscores, alphabet characters,
  //     and numbers are allowed).
  bool IsValid() const;

  virtual void Accept(AstWalker* walker);

 private:
  bool CalculateValidity();

  string full_name_;
  string identifier_;
  vector<string> namespaces_;  // The full_name_ broken up by namespaces.
  int begin_pos_;
  bool valid_;

  DISALLOW_COPY_AND_ASSIGN(IdentifierNode);
};

}  // namespace thrax

#endif  // THRAX_IDENTIFIER_NODE_H_
