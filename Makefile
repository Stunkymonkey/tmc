#RDSDEV := $(firstword $(wildcard /dev/radio*))

#export ANNOUNCE_BODY
#export PATH=bin:$$PATH; echo $$PATH;
#@echo RDSDEV IS $(RDSDEV)
#@echo "$$ANNOUNCE_BODY"

.PHONY: all
all: rds tmclog tmcimport tmcwebserver

.PHONY: rds librds rdsquery rdsd
rds: librds rdsquery rdsd

librds: rds/librds/src/.libs/librds.so
rds/librds/src/.libs/librds.so:
	$(MAKE) -C rds/librds

rdsquery: rds/rdsquery/src/rdsquery
rds/rdsquery/src/rdsquery: librds
	$(MAKE) -C rds/rdsquery

rdsd: rds/rdsd/src/rdsd
rds/rdsd/src/rdsd:
	$(MAKE) -C rds/rdsd

.PHONY: tmclog
tmclog: tmclog/src/tmclog
tmclog/src/tmclog: librds
	$(MAKE) -C tmclog

.PHONY: tmcimport
tmcimport: tmcimport/src/tmcimport
tmcimport/src/tmcimport: librds
	$(MAKE) -C tmcimport

.PHONY: tmcwebserver
tmcwebserver: tmcwebserver/src/tmcwebserver
tmcwebserver/src/tmcwebserver:
	$(MAKE) -C tmcwebserver


.PHONY: clean clean-rds clean-tmclog clean-tmcimport clean-tmcwebserver
clean: clean-rds clean-tmclog clean-tmcimport clean-tmcwebserver

.PHONY: clean-librds clean-rdsquery clean-rdsd
clean-rds: clean-librds clean-rdsquery clean-rdsd
clean-librds:
	$(MAKE) -C rds/librds clean
clean-rdsquery:
	$(MAKE) -C rds/rdsquery clean
clean-rdsd:
	$(MAKE) -C rds/rdsd clean

.PHONY: clean-tmclog
clean-tmclog:
	$(MAKE) -C tmclog clean

.PHONY: clean-tmcimport
clean-tmcimport:
	$(MAKE) -C tmcimport clean

.PHONY: clean-tmcwebserver
clean-tmcwebserver:
	$(MAKE) -C tmcwebserver clean


.PHONY: install install-rds install-librds install-rdsquery install-rdsd
install: install-rds install-server install-tmclog install-tmcimport install-tmcwebserver
install-rds: install-librds install-rdsquery install-rdsd
install-librds:
	$(MAKE) -C rds/librds install
install-rdsquery:
	$(MAKE) -C rds/rdsquery install
install-rdsd:
	$(MAKE) -C rds/rdsd install

.PHONY: install-tmclog
install-tmclog:
	$(MAKE) -C tmclog install

.PHONY: install-tmcimport
install-tmcimport:
	$(MAKE) -C tmcimport install

.PHONY: install-tmcwebserver
install-tmcwebserver:
	$(MAKE) -C tmcwebserver install

.PHONY: install-server
install-server:
	DOLLAR=\$ envsubst < "${BASE}/templates/rdsd.conf"    > "${PREFIX}/rdsd.conf"
	DOLLAR=\$ envsubst < "${BASE}/templates/rdsd.service" > "${PREFIX}/rdsd.service"
	DOLLAR=\$ envsubst < "${BASE}/templates/rdsd-wrapper" > "${PREFIX}/bin/rdsd-wrapper"
	chmod +x "${PREFIX}/bin/rdsd-wrapper"

define ANNOUNCE_BODY

Installation successful, please put this in your shellrc file and reload:

export PATH="${PREFIX}/bin$${PATH:+:}$${PATH:-}"
export MANPATH="${PREFIX}/man$${MANPATH:+:}$${MANPATH:-}"
export LD_LIBRARY_PATH="${PREFIX}/lib$${LD_LIBRARY_PATH:+:}$${LD_LIBRARY_PATH:-}"
export LDFLAGS="-L${PREFIX}/lib"

Afterwards, you can run rdsd with the following command:

rdsd-wrapper

A systemd service file is located in "${PREFIX}/rdsd.service". Run this to enable rdsd:
	ln -sT "${PREFIX}/rdsd.service" /etc/systemd/system/rdsd.service
	systemctl enable --now rdsd

endef
