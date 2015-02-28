ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src test
EXTRA_DIST = load.sql unload.sql
BUILT_SOURCES = load.sql unload.sql
CLEANFILES = load.sql unload.sql

enable_functions = @enable_functions@

load.sql:
	sh load.sql.sh $(enable_functions) > load.sql

unload.sql:
	sh unload.sql.sh $(enable_functions) > unload.sql

load:
	mysql $(MYSQL_OPTIONS) < load.sql

unload:
	mysql $(MYSQL_OPTIONS) < unload.sql

test test_prepare test_clean: all
	cd test && $(MAKE) $(AM_MAKEFLAGS) $@

.PHONY: load.sql unload.sql load unload test test_prepare test_clean
