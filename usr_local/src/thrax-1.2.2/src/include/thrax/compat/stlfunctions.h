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
// STL container functions used by the nlp/grm/language library

#ifndef THRAX_COMPAT_STL_FUNCTIONS_H_
#define THRAX_COMPAT_STL_FUNCTIONS_H_

namespace thrax {

using std::pair;

template <class ForwardIterator>
void STLDeleteContainerPointers(ForwardIterator begin,
                                ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete *temp;
  }
}

template <class ForwardIterator>
void STLDeleteContainerPairPointers(ForwardIterator begin,
                                    ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete temp->first;
    delete temp->second;
  }
}

template <class T>
void STLDeleteElements(T *container) {
  if (!container) return;
  STLDeleteContainerPointers(container->begin(), container->end());
  container->clear();
}

template <class Collection>
const typename Collection::value_type::second_type&
FindOrDie(const Collection& collection,
          const typename Collection::value_type::first_type& key) {
  typename Collection::const_iterator it = collection.find(key);
  if (it == collection.end())  {
    LOG(FATAL) << "Map key not found: " << key;
  }
  return it->second;
}

template <class Collection>
const typename Collection::value_type::second_type*
FindOrNull(const Collection& collection,
           const typename Collection::value_type::first_type& key) {
  typename Collection::const_iterator it = collection.find(key);
  if (it == collection.end()) {
    return NULL;
  }
  return &it->second;
}

template <class Collection>
typename Collection::value_type::second_type*
FindOrNull(Collection& collection,
           const typename Collection::value_type::first_type& key) {
  typename Collection::iterator it = collection.find(key);
  if (it == collection.end()) {
    return 0;
  }
  return &it->second;
}

template <class Collection>
bool InsertIfNotPresent(
    Collection * const collection,
    const typename Collection::value_type::first_type& key,
    const typename Collection::value_type::second_type& value) {
  pair<typename Collection::iterator, bool> ret =
    collection->insert(typename Collection::value_type(key, value));
  return ret.second;
}

template <class Collection>
typename Collection::value_type::second_type* const
InsertOrReturnExisting(Collection* const collection,
                       const typename Collection::value_type& vt) {
  pair<typename Collection::iterator, bool> ret = collection->insert(vt);
  if (ret.second) {
    return NULL;  // Inserted, no existing previous value.
  } else {
    return &ret.first->second;  // Return address of already existing value.
  }
}

// Same as above, except for explicit key and data.
template<class Collection>
typename Collection::value_type::second_type* const
InsertOrReturnExisting(
    Collection* const collection,
    const typename Collection::value_type::first_type& key,
    const typename Collection::value_type::second_type& data) {
  return InsertOrReturnExisting(collection,
                                typename Collection::value_type(key, data));
}

template <class ForwardIterator>
void STLDeleteContainerPairSecondPointers(ForwardIterator begin,
                                          ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete temp->second;
  }
}

}  // namespace thrax

namespace fst {

// used by prefixtree.h

template<class T>
void MapUtilAssignNewDefaultInstance(T **location) {
  *location = new T();
}

template<class T>
typename T::value_type::second_type
LookupOrInsertNew(T* const collection,
                  const typename T::value_type::first_type& key) {
  pair<typename T::iterator, bool> ret =
      collection->insert(
          typename T::value_type(
              key,
              static_cast<typename T::value_type::second_type>(NULL)));
  if (ret.second) {
    MapUtilAssignNewDefaultInstance(&(ret.first->second));
  }
  return ret.first->second;
}

}  // namespace fst

#endif  // THRAX_COMPAT_STL_FUNCTIONS_H_
