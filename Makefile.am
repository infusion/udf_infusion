ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src
EXTRA_DIST = load.sql unload.sql
BUILT_SOURCES = load.sql unload.sql
CLEANFILES = load.sql unload.sql

enable_functions = @enable_functions@

load.sql:
	sh load.sql.sh $(enable_functions) > load.sql

unload.sql:
	sh unload.sql.sh $(enable_functions) > unload.sql

.PHONY: load.sql unload.sql
