.PHONY: all tmclog tmcimport tmcwebserver
.PHONY: rds librds rdsquery rdsd

all: rds tmclog tmcimport tmcwebserver
rds: librds rdsquery rdsd
librds:
	$(MAKE) -C rds/librds
rdsquery: librds
	$(MAKE) -C rds/rdsquery
rdsd:
	$(MAKE) -C rds/rdsd

tmclog: librds
	$(MAKE) -C tmclog
tmcimport: librds
	$(MAKE) -C tmcimport
tmcwebserver:
	$(MAKE) -C tmcwebserver


.PHONY: clean clean-rds clean-tmclog clean-tmcimport clean-tmcwebserver
.PHONY: clean-librds clean-rdsquery clean-rdsd

clean: clean-rds clean-tmclog clean-tmcimport clean-tmcwebserver
clean-rds: clean-librds clean-rdsquery clean-rdsd
clean-librds:
	$(MAKE) -C rds/librds clean
clean-rdsquery:
	$(MAKE) -C rds/rdsquery clean
clean-rdsd:
	$(MAKE) -C rds/rdsd clean

clean-tmclog:
	$(MAKE) -C tmclog clean
clean-tmcimport:
	$(MAKE) -C tmcimport clean
clean-tmcwebserver:
	$(MAKE) -C tmcwebserver clean


.PHONY: install install-rds install-tmclog install-tmcimport install-tmcwebserver
.PHONY: install-librds install-rdsquery install-rdsd

install: install-rds install-tmclog install-tmcimport install-tmcwebserver
install-rds: install-librds install-rdsquery install-rdsd
install-librds:
	$(MAKE) -C rds/librds install
install-rdsquery:
	$(MAKE) -C rds/rdsquery install
install-rdsd:
	$(MAKE) -C rds/rdsd install

install-tmclog:
	$(MAKE) -C tmclog install
install-tmcimport:
	$(MAKE) -C tmcimport install
install-tmcwebserver:
	$(MAKE) -C tmcwebserver install
