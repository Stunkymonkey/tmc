.PHONY: all
all: rds

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

.PHONY: clean clean-rds clean-librds clean-rdsquery clean-rdsd
clean: clean-rds
clean-rds: clean-librds clean-rdsquery clean-rdsd
clean-librds:
	$(MAKE) -C rds/librds clean
clean-rdsquery:
	$(MAKE) -C rds/rdsquery clean
clean-rdsd:
	$(MAKE) -C rds/rdsd clean

.PHONY: install install-rds install-librds install-rdsquery install-rdsd
install: install-rds
install-rds: install-librds install-rdsquery install-rdsd
install-librds:
	$(MAKE) -C rds/librds install
install-rdsquery:
	$(MAKE) -C rds/rdsquery install
install-rdsd:
	$(MAKE) -C rds/rdsd install
