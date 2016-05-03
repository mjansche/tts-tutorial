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
// The GrmManager holds a set of FSTs in memory and performs rewrites
// via composition as well as various I/O functions. GrmManager is
// thread-compatible.

#ifndef NLP_GRM_LANGUAGE_GRM_MANAGER_H_
#define NLP_GRM_LANGUAGE_GRM_MANAGER_H_

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/compat/utils.h>
#include <fst/extensions/far/far.h>
#include <thrax/abstract-grm-manager.h>

DECLARE_string(outdir);  // From util/flags.cc.

namespace thrax {

template <typename Arc>
class GrmManagerSpec : public AbstractGrmManager<Arc> {
  typedef AbstractGrmManager<Arc> Base;
  typedef map<string, const typename Base::Transducer*> FstMap;

 public:
  GrmManagerSpec() : Base() { }
  virtual ~GrmManagerSpec() { }

  // Loads up the FSTs from a FAR file.  Returns true on success and false
  // otherwise.
  bool LoadArchive(const string& filename);

  // This function will write the created FSTs into an FST archive with the
  // provided filename.
  void ExportFar(const string& filename) const;

 private:
  DISALLOW_COPY_AND_ASSIGN(GrmManagerSpec);
};

template <typename Arc>
bool GrmManagerSpec<Arc>::LoadArchive(const string& filename) {
  fst::FarReader<Arc>* reader =
      fst::STTableFarReader<Arc>::Open(filename);
  if (!reader) {
    std::cout << "Unable to open FAR: " << filename;
    delete reader;
    return false;
  }
  bool rc = Base::LoadArchive(reader);
  delete reader;
  return rc;
}

template <typename Arc>
void GrmManagerSpec<Arc>::ExportFar(const string &filename) const {
  const string dir(JoinPath(
      FLAGS_outdir, StripBasename(filename)));
  VLOG(1) << "Creating output directory: " << dir;
  if (!RecursivelyCreateDir(dir))
    LOG(FATAL) << "Unable to create output directory: " << dir;

  const string out_path(JoinPath(FLAGS_outdir, filename));
  fst::STTableFarWriter<Arc>* writer =
      fst::STTableFarWriter<Arc>::Create(out_path);
  if (!writer)
    LOG(FATAL) << "Failed to create writer for: " << out_path;

  const FstMap &fsts = Base::GetFstMap();
  for (typename FstMap::const_iterator it = fsts.begin();
       it != fsts.end(); ++it) {
    VLOG(1) << "Writing FST: " << it->first;
    writer->Add(it->first, *it->second);
  }
  delete writer;
}

// A lot of code outside this build uses GrmManager with the old meaning of
// GrmManagerSpec<fst::StdArc>, forward-declaring it as a class. To obviate
// the need to change all that outside code, we provide this derived class:

class GrmManager : public GrmManagerSpec<fst::StdArc> {};

}  // namespace thrax

#endif  // NLP_GRM_LANGUAGE_GRM_MANAGER_H_
