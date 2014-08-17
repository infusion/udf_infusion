.PHONY: help build install load unload uninstall all

CFLAGS=$(shell mysql_config --cflags)
LIBS=$(shell mysql_config --libs)
PLUGINDIR=$(shell mysql_config --variable=plugindir)

help:
	@echo "make (build|install|load|uninstall)"
	@echo " - build     : build the .so file"
	@echo " - install   : copy the .so to the ${PLUGINDIR}"
	@echo " - load      : make the library available in mysql runtime"
	@echo " - unload    : remove the library from mysql runtime"
	@echo " - uninstall : remove the library from ${PLUGINDIR}"

build:
	gcc ${CFLAGS} -fPIC -shared -o udf_infusion.so udf_infusion.c

install:
	mv udf_infusion.so ${PLUGINDIR}

load: build install
	gawk -f db.awk -v drop=0 < udf_infusion.c | /usr/bin/mysql -f

unload:
	rm -f ${PLUGINDIR}/udf_infusion.so

uninstall: unload
	gawk -f db.awk -v drop=1 < udf_infusion.c | /usr/bin/mysql -f

all: load
