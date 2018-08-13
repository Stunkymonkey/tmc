#!/usr/bin/env bash

BASE="$(dirname "$(readlink -f "$0")")"
die() { echo $* >&2; exit 1; }

set -euo pipefail

export PREFIX="${PREFIX:-${HOME}/rds-installation}"
export RDSD_USER="${RDSD_USER:-$USER}"

export PATH="${PREFIX}/bin${PATH:+:}${PATH:-}"
export MANPATH="${PREFIX}/man${MANPATH:+:}${MANPATH:-}"
export LD_LIBRARY_PATH="${PREFIX}/lib${LD_LIBRARY_PATH:+:}${LD_LIBRARY_PATH:-}"
export LDFLAGS="-L${PREFIX}/lib"

for folder in \
  "${BASE}/rds/librds/" \
  "${BASE}/rds/rdsquery/" \
  "${BASE}/rds/rdsd/" \
  ; do
	(cd "${folder}" \
		&& autoreconf -fi \
		&& ./configure --prefix="${PREFIX}"
	)
	make -C "${folder}" -j
	make -C "${folder}" install
done

for dev in /dev/radio*; do
	export RDSDEV="${dev}"
	break
done

DOLLAR='$' envsubst < "${BASE}/templates/rdsd.conf"    > "${PREFIX}/rdsd.conf"
DOLLAR='$' envsubst < "${BASE}/templates/rdsd.service" > "${PREFIX}/rdsd.service"
DOLLAR='$' envsubst < "${BASE}/templates/rdsd-wrapper" > "${PREFIX}/bin/rdsd-wrapper"
chmod +x "${PREFIX}/bin/rdsd-wrapper"

cat <<-EOM

Installation successful, please put this in your shellrc file and reload:

export PATH="${PREFIX}/bin\${PATH:+:}\${PATH:-}"
export MANPATH="${PREFIX}/man\${MANPATH:+:}\${MANPATH:-}"
export LD_LIBRARY_PATH="${PREFIX}/lib\${LD_LIBRARY_PATH:+:}\${LD_LIBRARY_PATH:-}"
export LDFLAGS="-L${PREFIX}/lib"

Afterwards, you can run rdsd with the following command:

rdsd-wrapper

A systemd service file is located in "${PREFIX}/rdsd.service". Run this to enable rdsd:

ln -sT "${PREFIX}/rdsd.service" /etc/systemd/system/rdsd.service
systemctl enable --now rdsd

EOM
