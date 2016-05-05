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
// Author: rws@google.com (Richard Sproat)
//
// A registry for named functions to be used by the compiler.  The function of
// the registry is to allow users to define new functions easily so that the
// compiler is extensible.

#ifndef THRAX_COMPAT_REGISTRY_H_
#define THRAX_COMPAT_REGISTRY_H_

#include <map>
#include <string>

namespace thrax {

using std::map;
using std::string;

template <class T>
class Registry {
public:
  Registry() { };
  ~Registry() {
    for (typename map<string, T>::iterator ix = registry_.begin();
         ix != registry_.end();
         ++ix) {
      delete ix->second;
    }
  };

  void Register(string name, T object) {
    typename map<string, T>::iterator ix = registry_.find(name);
    // Register once
    if (ix == registry_.end()) {
      registry_.insert(std::pair<string, T>(name, object));
    }
  }

  T Get(string name) {
    typename map<string, T>::iterator ix = registry_.find(name);
    if (ix == registry_.end()) {
      return NULL;
    } else {
      return ix->second;
    }
  }

  const T Get(string name) const {
    typename map<string, T>::const_iterator ix = registry_.find(name);
    if (ix == registry_.end()) {
      return NULL;
    } else {
      return ix->second;
    }
  }

private:
  map<string, T> registry_;
  DISALLOW_COPY_AND_ASSIGN(Registry);
};  // Registry

}  // namespace thrax

#endif  // THRAX_COMPAT_REGISTRY_H_
