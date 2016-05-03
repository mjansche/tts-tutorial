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
// Some commonly needed stuff from std not listed in in fst/compat.h. This is
// included, often redundantly, in most of the files since these things are used
// scattered around the code.

#ifndef THRAX_COMPAT_COMPAT_H_
#define THRAX_COMPAT_COMPAT_H_

#include <deque>
#include <map>
#include <set>
#include <stack>
#include <istream>
#include <ostream>
#include <sstream>

using std::deque;
using std::map;
using std::set;
using std::stack;
using std::istream;
using std::istringstream;
using std::ostream;

#endif  // THRAX_COMPAT_COMPAT_H_
