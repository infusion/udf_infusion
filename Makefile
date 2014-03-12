
all:
	gcc -fPIC -O2 -pipe -shared -I /usr/include -o udf_infusion.so udf_infusion.c
	mv udf_infusion.so /usr/lib/mysql/plugin
	gawk -f db.awk -v drop=0 < udf_infusion.c | /usr/bin/mysql -f


uninstall:
	gawk -f db.awk -v drop=1 < udf_infusion.c | /usr/bin/mysql -f
	rm -f /usr/lib/mysql/plugin/udf_infusion.so
