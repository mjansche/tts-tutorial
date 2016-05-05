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
// A generic resource manager that can hold objects of any pointer type.
// Pointers can be placed into the ResourceMap associated with particular string
// keys.  At a later time, those pointers can then be retrieved again using the
// appropriate keys along with the types of the original pointers.  The
// ResourceMap takes and maintains (forever) ownership of all provided pointers.
//
// ResourceMap is thread-safe with respect to itself.  Pointers can be provided
// to multiple clients, however, and might be accessed/modified in a non-safe
// manner (depending on the object).
//
// Example:
//   ResourceMap map;
//
//   SymbolTable* main_symtab = GetSymbolTable();
//   string* text = new string("...");
//
//   map.Insert("main", main_symtab);
//   map.Insert("textfile", text);
//
//   // Do other stuff...
//
//   SymbolTable* main_symtab_reborn = map.Get<SymbolTable>("main");
//   string* text_reborn = map.Get<string>("textfile");

#ifndef THRAX_ALGO_RESOURCE_MAP_H_
#define THRAX_ALGO_RESOURCE_MAP_H_

#include <unordered_map>
#include <string>
#include <typeinfo>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/compat/closure.h>
#include <thrax/compat/stlfunctions.h>

namespace thrax {

class ResourceMap {
 private:
  struct Resource;
  typedef std::unordered_map<string, Resource*> Map;

 public:
  ResourceMap() {}
  ~ResourceMap() {
    fst::MutexLock lock(&mutex_);
    STLDeleteContainerPairSecondPointers(map_.begin(), map_.end());
  }

  // Inserts the specified object into the map using the provided name,
  // replacing any existing object.  Returns true if the object is a new
  // insertion.  Returns false if the insertion clobbered an existing object.
  // Generates a default pointer deleter closure.  This method will construct a
  // deleter that frees the object by calling delete (see DeletePointerClosure
  // in base/callback.h).
  template <typename T>
  bool Insert(const string& name, T* thing) {
    return InsertWithDeleter(name, thing, DeletePointerClosure(thing));
  }

  // Like Insert() above, but the client can optionally provide a custom delete
  // closure.  If a delete closure is provided, ResourceMap will take ownership
  // of the closure pointer and call it when the object dies.
  template <typename T>
  bool InsertWithDeleter(const string& name, T* thing, Closure* deleter) {
    fst::MutexLock lock(&mutex_);

    // Search the map for the correct hash position of the new insert.  We'll
    // use NULL as the value for now since we'll create it from scratch in the
    // future, after the potential deletion of the pre-existing object.
    std::pair<Map::iterator, bool> ret = map_.insert(Map::value_type(name, NULL));
    if (!ret.second)             // Already exists in the map, so we'll get rid
      delete ret.first->second;  // of the old value.

    // Create a new Resource container to hold the pointer as well as a deleter
    // closure, which we create now since we only are sure of the type at this
    // moment.
    ret.first->second =
        new Resource(static_cast<const void*>(thing), typeid(thing), deleter);

    return ret.second;
  }

  // Retrieves the object with the provided name and templated type.  Returns
  // NULL if the object with the provided name isn't found.  Crashes if the
  // object is found but the types do not match.  The ResourceMap maintains
  // ownership of the pointer, so clients should not delete the pointer
  // received.
  template <typename T>
  T* Get(const string& name) const {
    fst::MutexLock lock(&mutex_);
    Map::const_iterator it = map_.find(name);
    if (it == map_.end())
      return NULL;
    CheckType<T>(it, name);

    // We need to remove the const if the client's original type wasn't const.
    // If it was, however, we'll stick it right back on during the static_cast
    // and return.
    return static_cast<T*>(const_cast<void*>(it->second->data));
  }

  // Returns true if the map contains an object with the given name
  // (disregarding the type of the stored object).
  bool Contains(const string& name) const {
    fst::MutexLock lock(&mutex_);
    return map_.find(name) != map_.end();
  }

  // Returns true if the map contains an object with the given name of the
  // proper type.
  template <typename T>
  bool ContainsType(const string& name) const {
    fst::MutexLock lock(&mutex_);
    Map::const_iterator it = map_.find(name);
    return it != map_.end() && it->second->type == typeid(T*);
  }

  // Removes the specified object from the map.  Returns true if an object was
  // successfully erased, and false if the object didn't exist.
  bool Erase(const string& name) {
    fst::MutexLock lock(&mutex_);

    Map::iterator it = map_.find(name);
    if (it == map_.end())
      return false;

    delete it->second;
    map_.erase(it);
    return true;
  }

  template <typename T>
  T* Release(const string& name) {
    fst::MutexLock lock(&mutex_);

    T* val = NULL;
    Map::iterator it = map_.find(name);
    if (it != map_.end()) {
      CheckType<T>(it, name);
      val = static_cast<T*>(const_cast<void*>(it->second->data));
      // Release the data in the Resource, then delete the Resource
      // but not the data it contains
      it->second->Release();
      delete it->second;
      map_.erase(it);
    }
    return val;
  }

  // Returns the number of elements in the map.
  int Size() const {
    fst::MutexLock lock(&mutex_);
    return map_.size();
  }

  // Erases all elements in the current map.
  void Clear() {
    fst::MutexLock lock(&mutex_);
    STLDeleteContainerPairSecondPointers(map_.begin(), map_.end());
    map_.clear();
  }

 private:
  // Checks that the desired type is actually the same as the one originally
  // stored.  T should be the base (non-pointer) type.
  template <typename T>
  void CheckType(Map::const_iterator it, const string& name) const {
    const std::type_info& original_type = it->second->type;
    const std::type_info& requested_type = typeid(T*);
    CHECK(original_type == requested_type)
        ;
  }

  struct Resource {
    Resource(const void* data_,
             const std::type_info& type_,
             Closure* destructor_)
        : data(data_), type(type_), destructor(destructor_) {}

    ~Resource() {
      if (destructor)
        destructor->Run();
    }

    void Release() {
      delete destructor;
      destructor = NULL;
    }

    const void* data;
    const std::type_info& type;
    Closure* destructor;
  };

  Map map_;

  mutable fst::Mutex mutex_;
};

};  // namespace thrax

#endif  // THRAX_ALGO_RESOURCE_MAP_H_
