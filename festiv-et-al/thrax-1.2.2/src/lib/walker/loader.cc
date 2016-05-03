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
//         rws@google.com (Richard Sproat)

#include <string>

#include <thrax/arcsort.h>
#include <thrax/assert-equal.h>
#include <thrax/cdrewrite.h>
#include <thrax/closure.h>
#include <thrax/compose.h>
#include <thrax/concat.h>
#include <thrax/connect.h>
#include <thrax/determinize.h>
#include <thrax/difference.h>
#include <thrax/expand.h>
#include <thrax/features.h>
#include <thrax/function.h>
#include <thrax/invert.h>
#include <thrax/loadfst.h>
#include <thrax/loadfstfromfar.h>
#include <thrax/minimize.h>
#include <thrax/mpdtcompose.h>
#include <thrax/optimize.h>
#include <thrax/paradigm.h>
#include <thrax/pdtcompose.h>
#include <thrax/project.h>
#include <thrax/replace.h>
#include <thrax/reverse.h>
#include <thrax/rewrite.h>
#include <thrax/rmepsilon.h>
#include <thrax/stringfile.h>
#include <thrax/stringfst.h>
#include <thrax/symboltable.h>
#include <thrax/symbols.h>
#include <thrax/union.h>

namespace thrax {
namespace function {

void RegisterFunctions() {
  REGISTER_GRM_FUNCTION(Analyzer);
  REGISTER_GRM_FUNCTION(ArcSort);
  REGISTER_GRM_FUNCTION(AssertEqual);
  REGISTER_GRM_FUNCTION(Category);
  REGISTER_GRM_FUNCTION(CDRewrite);
  REGISTER_GRM_FUNCTION(Closure);
  REGISTER_GRM_FUNCTION(Compose);
  REGISTER_GRM_FUNCTION(Concat);
  REGISTER_GRM_FUNCTION(ConcatDelayed);
  REGISTER_GRM_FUNCTION(Connect);
  REGISTER_GRM_FUNCTION(Determinize);
  REGISTER_GRM_FUNCTION(Difference);
  REGISTER_GRM_FUNCTION(Expand);
  REGISTER_GRM_FUNCTION(Feature);
  REGISTER_GRM_FUNCTION(FeatureVector);
  REGISTER_GRM_FUNCTION(Invert);
  REGISTER_GRM_FUNCTION(LoadFst);
  REGISTER_GRM_FUNCTION(LoadFstFromFar);
  REGISTER_GRM_FUNCTION(Minimize);
  REGISTER_GRM_FUNCTION(MPdtCompose);
  REGISTER_GRM_FUNCTION(Optimize);
  REGISTER_GRM_FUNCTION(ParadigmReplace);
  REGISTER_GRM_FUNCTION(PdtCompose);
  REGISTER_GRM_FUNCTION(Project);
  REGISTER_GRM_FUNCTION(Replace);
  REGISTER_GRM_FUNCTION(Reverse);
  REGISTER_GRM_FUNCTION(Rewrite);
  REGISTER_GRM_FUNCTION(RmEpsilon);
  REGISTER_GRM_FUNCTION(StringFile);
  REGISTER_GRM_FUNCTION(StringFst);
  REGISTER_GRM_FUNCTION(SymbolTable);
  REGISTER_GRM_FUNCTION(Tagger);
  REGISTER_GRM_FUNCTION(Union);
  REGISTER_GRM_FUNCTION(UnionDelayed);
}

}  // namespace function
}  // namespace thrax
