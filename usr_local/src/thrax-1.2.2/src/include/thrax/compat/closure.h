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
// Implementation of DeletePointerClosure() and NewCallback() needed by
// resource-map.h

#ifndef THRAX_COMPAT_CLOSURE_H_
#define THRAX_COMPAT_CLOSURE_H_

namespace thrax {

class Closure {
 public:
  virtual ~Closure() {}
  virtual void Run() = 0;
};

template <typename T>
class TypedCallback : public Closure {
 public:
  TypedCallback(void(*function)(T* ptr), T* ptr) :
      function_(function), ptr_(ptr) { }
  void Run() {
    function_(ptr_);
    delete this;
  }

 private:
  void(*function_)(T*);
  T* ptr_;
};

template <typename T>
Closure* NewCallback(void(*function)(T* ptr), T* ptr) {
  TypedCallback<T>* callback = new TypedCallback<T>(function, ptr);
  return callback;
}

template <typename T>
void DeletePointer(T* p) {
  delete p;
}

template <typename T>
Closure* DeletePointerClosure(T* p) {
  return NewCallback(&DeletePointer<T>, p);
}

}  // namespace thrax

#endif  // THRAX_COMPAT_CLOSURE_H_
