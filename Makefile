RDSDEV := $(firstword $(wildcard /dev/radio*))

export ANNOUNCE_BODY

.PHONY: all
all: rds rdslog #tmc
	@echo RDSDEV IS $(RDSDEV)
	@echo "$$ANNOUNCE_BODY"
	export PATH=bin:$$PATH; echo $$PATH;

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

.PHONY: rdslog
rdslog: rdslog/src/rdslog
rdslog/src/rdslog: librds
	$(MAKE) -C rdslog

.PHONY: tmc
tmc: tmc/src/tmc
tmc/src/tmc: librds
	$(MAKE) -C tmc

.PHONY: clean clean-rds clean-librds clean-rdsquery clean-rdsd
clean: clean-rds clean-rdslog #clean-tmc
clean-rds: clean-librds clean-rdsquery clean-rdsd
clean-librds:
	$(MAKE) -C rds/librds clean
clean-rdsquery:
	$(MAKE) -C rds/rdsquery clean
clean-rdsd:
	$(MAKE) -C rds/rdsd clean

.PHONY: clean-rdslog
clean-rdslog:
	$(MAKE) -C rdslog clean

.PHONY: clean-tmc
clean-tmc:
	$(MAKE) -C tmc clean

.PHONY: install install-rds install-librds install-rdsquery install-rdsd
install: install-rds install-server install-rdslog #install-tmc
install-rds: install-librds install-rdsquery install-rdsd
install-librds:
	$(MAKE) -C rds/librds install
install-rdsquery:
	$(MAKE) -C rds/rdsquery install
install-rdsd:
	$(MAKE) -C rds/rdsd install

.PHONY: install-rdslog
install-rdslog:
	$(MAKE) -C rdslog install

.PHONY: install-tmc
install-tmc:
	$(MAKE) -C tmc install

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
