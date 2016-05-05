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
//
// Basic function class that implements some operation in C++.  Inheritors
// should perform some sort of manipulation based on the input arguments and
// then return a DataType.  Implementors also need to free the provided argument
// vector (both the contained pointers and the vector pointer itself).  Finally,
// they should also be templated on the Arc type and call
// REGISTER_GRM_FUNCTION(ClassName) for StdArc and LogArc support (usually in
// loader.cc).

#ifndef THRAX_FUNCTION_H_
#define THRAX_FUNCTION_H_

#include <iostream>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/fstlib.h>
#include <thrax/datatype.h>
#include <thrax/compat/stlfunctions.h>
#include <thrax/compat/registry.h>

namespace thrax {
namespace function {

template <typename Arc>
class Function {
 public:
  Function() {}
  virtual ~Function() {}

  // Runs the desired function by wrapping Execute() and then freeing the
  // arguments.
  DataType* Run(vector<DataType*>* args) {
    DataType* return_value = Execute(*args);

    STLDeleteContainerPointers(args->begin(), args->end());
    delete args;

    return return_value;
  }

 protected:
  // Actually performs the function's work, without deleting the provided
  // arguments.
  virtual DataType* Execute(const vector<DataType*>& args) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Function<Arc>);
};


extern Registry<Function<fst::StdArc>* > kStdArcRegistry;
extern Registry<Function<fst::LogArc>* > kLogArcRegistry;
extern Registry<Function<fst::Log64Arc>* > kLog64ArcRegistry;
extern void RegisterFunctions();

#define REGISTER_STDARC_FUNCTION(function) \
  kStdArcRegistry.Register(#function, new function)

#define REGISTER_LOGARC_FUNCTION(function) \
  kLogArcRegistry.Register(#function, new function)

#define REGISTER_LOG64ARC_FUNCTION(function) \
  kLogArcRegistry.Register(#function, new function)

#define REGISTER_GRM_FUNCTION(name) \
  typedef name<fst::StdArc> StdArc ## name; \
  REGISTER_STDARC_FUNCTION(StdArc ## name); \
  typedef name<fst::LogArc> LogArc ## name; \
  REGISTER_LOGARC_FUNCTION(LogArc ## name); \
  typedef name<fst::LogArc> Log64Arc ## name; \
  REGISTER_LOGARC_FUNCTION(Log64Arc ## name)

template <typename Arc>
class UnaryFstFunction : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;

  UnaryFstFunction() {}
  virtual ~UnaryFstFunction() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() < 1) {
      std::cout << "UnaryFstFunction: Expected at least 1 argument"
                << std::endl;
      return NULL;
    }
    if (!args[0]->is<Transducer*>()) {
      std::cout << "UnaryFstFunction: Expected FST for argument 1" << std::endl;
      return NULL;
    }

    const Transducer* fst = *args[0]->get<Transducer*>();
    Transducer* output = UnaryFstExecute(*fst, args);

    return output ? new DataType(output) : NULL;
  }

  // This should actually perform the operation, using the FST argument
  // provided along with extra arguments still in the vector.  The final result
  // should be allocated by this function.  Ownership of provided arguments
  // remains with the caller.  Return NULL to denote an error.
  virtual Transducer* UnaryFstExecute(const Transducer& fst,
                                      const vector<DataType*>& args) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(UnaryFstFunction);
};


template <typename Arc>
class BinaryFstFunction : public Function<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;

  BinaryFstFunction() {}
  virtual ~BinaryFstFunction() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() < 2) {
      std::cout << "BinaryFstFunction: Expected at least 2 arguments"
                << std::endl;
      return NULL;
    }
    for (int i = 0; i < 2; ++i) {
      if (!args[i]->is<Transducer*>()) {
        std::cout << "BinaryFstFunction: Expected FST for argument " << i + 1
                  << std::endl;
        return NULL;
      }
    }

    const Transducer* left = *args[0]->get<Transducer*>();
    const Transducer* right = *args[1]->get<Transducer*>();
    Transducer* output = BinaryFstExecute(*left, *right, args);

    return output ? new DataType(output) : NULL;
  }

  // Same as above (with UnaryFstFunction), except now with two arguments.
  virtual Transducer* BinaryFstExecute(const Transducer& left,
                                       const Transducer& right,
                                       const vector<DataType*>& args) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(BinaryFstFunction);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_FUNCTION_H_
