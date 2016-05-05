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
//
// Stand-alone binary to load up a FAR and rewrite some strings.

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <../bin/rewrite-tester-utils.h>

int main(int argc, char** argv) {
  std::set_new_handler(FailedNewHandler);
  SET_FLAGS(argv[0], &argc, &argv, true);
  RewriteTesterUtils* utils = new RewriteTesterUtils;
  utils->Initialize();
  utils->Run();
  delete utils;
  return 0;
}
