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

#include <thrax/evaluator.h>

#include <string>

#include <fst/fst-decl.h>
#include <thrax/function.h>
#include <thrax/compat/utils.h>
#include <thrax/compat/registry.h>

DEFINE_bool(optimize_all_fsts, false,
            "If true, we'll run Optimize[] on all FSTs.");
DEFINE_bool(print_rules, true,
            "If true, we'll print out the rules as we evaluate them.");

namespace thrax {


Registry<function::Function<fst::StdArc>* > function::kStdArcRegistry;
Registry<function::Function<fst::LogArc>* > function::kLogArcRegistry;
Registry<function::Function<fst::Log64Arc>* > function::kLog64ArcRegistry;

template <>
function::Function<fst::StdArc>* GetFunction(const string& func_name) {
  string name = "StdArc" + func_name;
  return function::kStdArcRegistry.Get(name);
}

template <>
function::Function<fst::LogArc>* GetFunction(const string& func_name) {
  string name = "LogArc" + func_name;
  return function::kLogArcRegistry.Get(name);
}

template <>
function::Function<fst::Log64Arc>* GetFunction(const string& func_name) {
  string name = "Log64Arc" + func_name;
  return function::kLog64ArcRegistry.Get(name);
}

}  // namespace thrax
