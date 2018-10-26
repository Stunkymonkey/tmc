#!/bin/bash

BASE="$(dirname "$(readlink -f "$0")")"
die() { echo $* >&2; exit 1; }

for folder in \
  "${BASE}/rds/librds/" \
  "${BASE}/rds/rdsquery/" \
  "${BASE}/rds/rdsd/" \
  "${BASE}/rdslog/" \
  "${BASE}/tmcimport/" \
  ; do
	(cd "${folder}" \
		&& autoreconf -fi \
		&& ./configure $*
	)
done
