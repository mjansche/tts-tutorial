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

#include <thrax/stringfst.h>

#include <thrax/function.h>

DEFINE_string(initial_boundary_symbol, "BOS",
              "Predefined symbol to mark beginning of string.");
DEFINE_string(final_boundary_symbol, "EOS",
              "Predefined symbol to mark end of string.");

DEFINE_int64(generated_label_start_index, 0x100000,
             "The first index to use for a generated label.");
DEFINE_int64(generated_label_end_index, 0x10fffb,
             "The last index to use for a generated label.");
DEFINE_int64(initial_boundary_marker, 0x10fffc,
             "Marker for the beginning of a string.");
DEFINE_int64(final_boundary_marker, 0x10fffd,
             "Marker for the end of a string.");

