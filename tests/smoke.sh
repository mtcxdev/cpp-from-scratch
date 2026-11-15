#!/bin/sh
set -eu
compiler="$1"
source="$2"
output=$("$compiler" "$source")
expected='Hello from our own C++ front end. Answer: 42'
if [ "$output" != "$expected" ]; then
  printf 'expected: %s\nactual:   %s\n' "$expected" "$output" >&2
  exit 1
fi
