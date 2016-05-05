#! /bin/bash

set -o errexit
set -o nounset

modeldir="$(dirname $0)"

exec g2p-lookup \
  --bytes_to_graphones="$modeldir/bytes_to_graphones.fst" \
  --graphone_model="$modeldir/graphone_model.fst" \
  --phonemes_to_graphones="$modeldir/phonemes_to_graphones.fst" \
  "$@"
