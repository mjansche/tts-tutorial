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
// A namespace manager for the tree evaluator.  This handles nested levels of
// identifiers.  Essentially, this is a manager/wrapper for ResourceMap,
// collecting shared resources into a single map and allowing local variables to
// live in a hierarchical stack of maps.

#ifndef THRAX_NAMESPACE_H_
#define THRAX_NAMESPACE_H_

#include <map>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/identifier-node.h>
#include <thrax/algo/resource-map.h>

namespace thrax {
class ResourceMap;
}  // namespace thrax

namespace thrax {

class Namespace {
 public:
  Namespace();
  ~Namespace();

  // Create (and return) a new sub-namespace using the provided filename and
  // alias, adding it to the namespace tree.  The returned namespace is still
  // owned by this class and must not be freed by the caller.
  Namespace* AddSubNamespace(const string& filename, const string& alias);

  // These functions will create a new local environment space (or destroy the
  // newest one).
  void PushLocalEnvironment();
  void PopLocalEnvironment();

  // Returns the size of stack size of the local environment.  This will help us
  // to determine how many recursive iterations we are down.
  int LocalEnvironmentDepth() const;

  // Adds the provided resource to this namespace.  Returns true if the resource
  // was a new insertion and false if we clobbered a pre-existing object.  The
  // second version inserts the object without taking over ownership of the
  // pointer.
  template <typename T>
  bool Insert(const string& identifier_name, T* resource) {
    const string& name = ConstructMapName(identifier_name);
    return resources_->Insert(name, resource);
  }
  template <typename T>
  bool InsertWithoutDelete(const string& identifier_name, T* resource) {
    const string& name = ConstructMapName(identifier_name);
    return resources_->InsertWithDeleter(name, resource, NULL);
  }

  // The same as the above, but this time, we insert into the local namespace
  // instead of the globally shared one.
  template <typename T>
  bool InsertLocal(const string& identifier_name, T* resource) {
    return local_env_.back()->Insert(identifier_name, resource);
  }
  template <typename T>
  bool InsertLocalWithoutDelete(const string& identifier_name, T* resource) {
    return local_env_.back()->InsertWithDeleter(
        identifier_name, resource, NULL);
  }

  // Returns the resource associated with this namespace (and NULL if the
  // provided name is not found or if the type is incorrect).  If the provided
  // Namespace pointer-pointer is not NULL, then we'll also return the namespace
  // where it was found.
  template <typename T>
  T* Get(const IdentifierNode& identifier, Namespace** where) {
    // If the identifier doesn't have a namespace, then we should check the
    // local variables first if possible.
    if (!identifier.HasNamespaces() && !local_env_.empty()) {
      const string& name = identifier.GetIdentifier();
      if (local_env_.back()->ContainsType<T>(name)) {
        if (where)
          *where = this;
        return local_env_.back()->Get<T>(name);
      }
    }

    // At this point, either there is no local resource of the provided name, or
    // we have a namespaced identifier.  So in either case, let's check the
    // global map.
    Namespace* final_namespace = ResolveNamespace(identifier);
    if (final_namespace) {
      const string& name =
          final_namespace->ConstructMapName(identifier.GetIdentifier());
      if (resources_->ContainsType<T>(name)) {
        if (where)
          *where = final_namespace;
        return resources_->Get<T>(name);
      }
    }

    return NULL;
  }
  template <typename T>
  T* Get(const IdentifierNode& identifier) {
    return Get<T>(identifier, NULL);
  }

  // Removes the provided identifier from the top-most local environment.
  bool EraseLocal(const string& identifier);

  // Returns namespace in the identifier, according to the current iterator
  // position.
  Namespace* ResolveNamespace(const IdentifierNode& identifier);

  string GetFilename() const;

  void SetTopLevel();
  bool IsTopLevel() const;

 private:
  // This constructor creates a sub-namespace that shares resources with the
  // parent.  As such, it should be invoked only through AddSubNamespace().
  Namespace(const string& filename, ResourceMap* resource_map);

  // Creates the semi-unique joined name for a given identifier, currently the
  // filename followed by a slash followed by the identifier name.
  string ConstructMapName(const string& identifier_name) const;

  // Internal version of ResolveNamespace() which also takes an iterator to the
  // current identifier's namespace position.
  Namespace* ResolveNamespaceInternal(
      const IdentifierNode& identifier,
      IdentifierNode::const_iterator* identifier_nspos);

  // This is true if this namespace is the top-level one (i.e., the one
  // corresponding to the main body of the file currently being compiled) and
  // false otherwise.
  bool toplevel_;

  // The filename associated with this particular namespace alias.
  string filename_;

  // Provides a mapping from a single-component alias to the next Namespace
  // object.  This map is expected to be reasonably small, so we'll use a normal
  // map instead of a hash_map.
  map<string, Namespace*> alias_namespace_map_;

  // The actual map of string to global resources.  This resource map will
  // likely be shared across this namespace and all sub-namespaces.  Keys are
  // those provided by ConstructMapName().
  ResourceMap* resources_;
  bool owns_resources_;

  // We still, however, need a list of local variables on a per-namespace basis.
  // This will be for function calls primarily and other non-globally-exported
  // stuff.  The front of the list is the bottom of the stack; the back will be
  // the newest scope.
  vector<ResourceMap*> local_env_;

  DISALLOW_COPY_AND_ASSIGN(Namespace);
};

}  // namespace thrax

#endif  // THRAX_NAMESPACE_H_
