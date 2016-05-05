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

#include <thrax/namespace.h>

#include <map>
#include <string>
#include <vector>
using std::vector;

#include <thrax/identifier-node.h>
#include <thrax/algo/resource-map.h>
#include <thrax/compat/utils.h>
#include <thrax/compat/stlfunctions.h>

namespace thrax {



Namespace::Namespace()
    : toplevel_(false), resources_(new ResourceMap()), owns_resources_(true) {}

Namespace::Namespace(const string& filename, ResourceMap* resource_map)
    : toplevel_(false), filename_(filename), resources_(resource_map),
      owns_resources_(false) {}

Namespace::~Namespace() {
  CHECK(local_env_.empty());
  STLDeleteContainerPairSecondPointers(alias_namespace_map_.begin(),
                                       alias_namespace_map_.end());
  if (owns_resources_)
    delete resources_;
}

Namespace* Namespace::AddSubNamespace(const string& filename,
                                      const string& alias) {
  Namespace* new_namespace = new Namespace(filename, resources_);
  CHECK(InsertIfNotPresent(&alias_namespace_map_, alias, new_namespace))
      ;
  return new_namespace;
}

void Namespace::PushLocalEnvironment() {
  local_env_.push_back(new ResourceMap());
}

void Namespace::PopLocalEnvironment() {
  delete local_env_.back();
  local_env_.pop_back();
}

int Namespace::LocalEnvironmentDepth() const {
  return local_env_.size();
}

bool Namespace::EraseLocal(const string& identifier) {
  return local_env_.back()->Erase(identifier);
}

Namespace* Namespace::ResolveNamespace(const IdentifierNode& identifier) {
  IdentifierNode::const_iterator where = identifier.begin();
  return ResolveNamespaceInternal(identifier, &where);
}

Namespace* Namespace::ResolveNamespaceInternal(
    const IdentifierNode& identifier,
    IdentifierNode::const_iterator* identifier_nspos) {
  if (*identifier_nspos == identifier.end()) {
    // Here, we're at the end and can just look this up.
    return this;
  } else {
    // Here, we need to look up the next namespace and return that, maybe
    // creating it if requested.
    const string& namespace_name = **identifier_nspos;
    Namespace** next = FindOrNull(alias_namespace_map_, namespace_name);
    if (next) {
      ++(*identifier_nspos);
      return (*next)->ResolveNamespaceInternal(identifier, identifier_nspos);
    } else {
      return NULL;
    }
  }
}

string Namespace::GetFilename() const {
  return filename_.empty() ? "<unknown file>" : filename_;
}

void Namespace::SetTopLevel() {
  toplevel_ = true;
}

bool Namespace::IsTopLevel() const {
  return toplevel_;
}

string Namespace::ConstructMapName(const string& identifier_name) const {
  return StrCat(filename_, "/", identifier_name);
}

}  // namespace thrax
