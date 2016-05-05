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
// Simple implementation of OneOf, needed by datatype.h
// This version ONLY allows one to specify a template with exactly 6 types.

#ifndef THRAX_COMPAT_ONEOF_H_
#define THRAX_COMPAT_ONEOF_H_

namespace thrax {

// This is a simple version of ManualConstructor, which effectively shields
// the object from the compiler. I dispense with the clever AlignType stuff here
// and just assume this is always an int32 (size 4). After all the difference we
// are talking about is between one and four bytes. And for the intended
// application we never need anything other than four bytes anyway.
// No constructor/destructor so it can be used in a union.

template <typename T>
class Shield {
 public:
  inline T* get() {
    return reinterpret_cast<T*>(storage_);
  }
  const inline T* get() const {
    return reinterpret_cast<const T*>(storage_);
  }

  inline T* operator->() { return get(); }
  inline const T* operator->() const { return get(); }

  inline T& operator*() { return *get(); }
  inline const T& operator*() const { return *get(); }

  inline void Init(const T& p) {
    new(storage_) T(p);
  }

  inline void Kill() {
    get()->~T();
  }

 private:
  union {
    int32 force_alignment_;
    char storage_[sizeof(T)];
  };
};

template <typename T1,
          typename T2,
          typename T3,
          typename T4,
          typename T5,
          typename T6>
class Oneof {
 public:
  Oneof(const Oneof& oneof) {
    Copy(oneof);
  }

  explicit Oneof(const T1& thing) {
    thing_.t1_.Init(thing);
    type_ = GetType(static_cast<T1*>(NULL));
  }

  explicit Oneof(const T2& thing) {
    thing_.t2_.Init(thing);
    type_ = GetType(static_cast<T2*>(NULL));
  }

  explicit Oneof(const T3& thing) {
    thing_.t3_.Init(thing);
    type_ = GetType(static_cast<T3*>(NULL));
  }

  explicit Oneof(const T4& thing) {
    thing_.t4_.Init(thing);
    type_ = GetType(static_cast<T4*>(NULL));
  }

  explicit Oneof(const T5& thing) {
    thing_.t5_.Init(thing);
    type_ = GetType(static_cast<T5*>(NULL));
  }

  explicit Oneof(const T6& thing) {
    thing_.t6_.Init(thing);
    type_ = GetType(static_cast<T6*>(NULL));
  }

  ~Oneof() { Kill(); }

  template <typename T>
  bool is() const {
    return type_ == GetType(static_cast<T*>(NULL));
  }

  template <typename T>
  const T* get() const {
    T* dummy = NULL;
    if (GetType(dummy) != type_) return NULL;
    return get_internal(dummy)->get();
  }

  template <typename T>
  T* get_mutable() {
    T* dummy = NULL;
    if (GetType(dummy) != type_) return NULL;
    return get_mutable_internal(dummy)->get();
  }

 private:
  inline void Copy(const Oneof& oneof) {
    type_ = oneof.type_;
    switch (type_) {
      // NB: These cases are assumed to be synced with GetType() see below
      case 1: thing_.t1_.Init(*(oneof.thing_.t1_.get())); return;
      case 2: thing_.t2_.Init(*(oneof.thing_.t2_.get())); return;
      case 3: thing_.t3_.Init(*(oneof.thing_.t3_.get())); return;
      case 4: thing_.t4_.Init(*(oneof.thing_.t4_.get())); return;
      case 5: thing_.t5_.Init(*(oneof.thing_.t5_.get())); return;
    }
  }

  void Kill() {
    // NB: These cases are assumed to be synced with GetType() see below
    switch (type_) {
      case 1: thing_.t1_.Kill(); return;
      case 2: thing_.t2_.Kill(); return;
      case 3: thing_.t3_.Kill(); return;
      case 4: thing_.t4_.Kill(); return;
      case 5: thing_.t5_.Kill(); return;
    }
  }

  Shield<T1>* get_mutable_internal(T1*) {
    return &thing_.t1_;
  }

  Shield<T2>* get_mutable_internal(T2*) {
    return &thing_.t2_;
  }

  Shield<T3>* get_mutable_internal(T3*) {
    return &thing_.t3_;
  }

  Shield<T4>* get_mutable_internal(T4*) {
    return &thing_.t4_;
  }

  Shield<T5>* get_mutable_internal(T5*) {
    return &thing_.t5_;
  }

  Shield<T6>* get_mutable_internal(T6*) {
    return &thing_.t6_;
  }

  const Shield<T1>* get_internal(T1*) const {
    return &thing_.t1_;
  }

  const Shield<T2>* get_internal(T2*) const {
    return &thing_.t2_;
  }

  const Shield<T3>* get_internal(T3*) const {
    return &thing_.t3_;
  }

  const Shield<T4>* get_internal(T4*) const {
    return &thing_.t4_;
  }

  const Shield<T5>* get_internal(T5*) const {
    return &thing_.t5_;
  }

  const Shield<T6>* get_internal(T6*) const {
    return &thing_.t6_;
  }

  inline const unsigned int GetType(T1*) const { return 1; }
  inline const unsigned int GetType(T2*) const { return 2; }
  inline const unsigned int GetType(T3*) const { return 3; }
  inline const unsigned int GetType(T4*) const { return 4; }
  inline const unsigned int GetType(T5*) const { return 5; }
  inline const unsigned int GetType(T6*) const { return 6; }

  union datum {
    Shield<T1> t1_;
    Shield<T2> t2_;
    Shield<T3> t3_;
    Shield<T4> t4_;
    Shield<T5> t5_;
    Shield<T6> t6_;
  } thing_;

  int type_;
};  // Oneof

}  // namespace thrax

#endif  // THRAX_COMPAT_ONEOF_H_
