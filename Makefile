
all:
	gcc -fPIC -O2 -pipe -shared -I /usr/local/mysql/include/mysql -o udf_infusion.so udf_infusion.c
	mv udf_infusion.so /usr/local/mysql/lib/plugin
	gawk -f db.awk -v drop=1 < udf_infusion.c | /usr/local/mysql/bin/mysql
	/etc/init.d/mysql restart
	gawk -f db.awk -v drop=0 < udf_infusion.c | /usr/local/mysql/bin/mysql

