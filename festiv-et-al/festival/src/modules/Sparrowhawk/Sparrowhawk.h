/********************************************************************************
 * Sparrowhawk integration code
 *
 * Author: Rob Clark
 *
 * Copyright 2015 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************
 * Known issues
 *
 * Sparowhawk keeps punctuation symbols as tokens. Festival keeps punctuation as
 * features of tokens. Sparrowhawk's position index of the token is used to
 * determine whether punctuation is pre- post-punctuation, but this is currently
 * broken
 *******************************************************************************/

#ifndef __SPARROWHAWK_H__
#define __SPARROWHAWK_H__

#include <sparrowhawk/normalizer.h>

#include "siod_defs.h"

using speech::sparrowhawk::Normalizer;

SIOD_REGISTER_CLASS_DCLS(sparrowhawk_normalizer, Normalizer)
VAL_REGISTER_CLASS_DCLS(sparrowhawk_normalizer, Normalizer)

#endif // __SPARROWHAWK_H__
