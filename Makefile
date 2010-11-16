

all:
	gcc -O2 -shared -I /usr/local/mysql/include/ -o udf_infusion.so udf_infusion.c
	mv udf_infusion.so /usr/local/mysql/lib/plugin
	/usr/local/mysql/bin/mysql -uroot -p -D test < delete.sql
	/usr/local/mysql/bin/mysql -uroot -p -D test < create.sql
