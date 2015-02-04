MySQL Infusion UDF
==================

Description
-----------

The MySQL Infusion UDF is a functionality enhancement for MySQL.
It provides a variety of new string, math and aggregate functions.

Installation
------------

Please make sure you have the following required software on your system
before installation:

* C and C++ compiler
* MySQL server, client and development files

To install MySQL Infusion UDF:

```sh
git clone https://github.com/infusion/udf_infusion.git
cd udf_infusion
./configure
make
sudo make install
mysql <options> < load.sql
```

You can choose to install only given UDF functions with the
`--enable-functions` option:

```sh
./configure --enable-functions="<list-of-functions>"
```

where `<list-of-functions>` is a list of function names separated by space.

To uninstall:

```sh
mysql <options> < unload.sql
make uninstall
```

Aggregate Functions
-------------------

Get the median of a set
```
double median(double n);

mysql> SELECT median(weight) from t1;
```


Calculate the co-variance of two random variables
```
double covariance(double x, double y);

mysql> SELECT covariance(a, b) from t1;
```


Calculate the correlation of two random variables
```
double corr(double x, double y);

mysql> SELECT corr(a, b) from t1;
```


Get the first element of a group
```
string group_first(string s);

mysql> SELECT group_first(s) from t1;
```


Get the last element of a group
```
string group_last(string s);

mysql> SELECT group_last(s) from t1;
```


Get the maximum number of elements less than a certain partial sum
```
int lesspart(double m, double psum);

mysql> SELECT lesspart(m, 1000) from t1;
```


Get the maximum number of elements less than a certain percentage of the partial sum
```
int lesspartpct(double m, double pct);

mysql> SELECT lesspartpct(m, 0.4) from t1;
```


Get the maximum number of elements less than average
```
int lessavg(double m);

mysql> SELECT lessavg(double m) from t1;
```


Calculate continuous percentile. Returns the value at a relative position
specified by the fraction, interpolating between input values if needed.
```
double percentile_cont(double x, double fraction);

mysql> SELECT percentile_cont(x, 0.5) from t1;
```


Calculate discrete percentile. Returns the first input value whose relative
position is greater than or equal to the specified fraction.
```
double percentile_disc(double x, double fraction);

mysql> SELECT percentile_disc(x, 0.5) from t1;
```


Calculates the 3th statistical moment of a data set: skewness
See: http://geography.uoregon.edu/geogr/topics/moments.htm
```
double skewness(double m);

mysql> SELECT skewness(double m) from t1;
```


Find statistical mode, i.e. the most frequent input value.
```
double stat_mode(double x);

mysql> SELECT stat_mode(double x) from t1;
```


Calculates the 4th statistical moment of a data set: kurtosis
See: http://geography.uoregon.edu/geogr/topics/moments.htm
```
double kurtosis(double m);

mysql> SELECT kurtosis(double m) from t1;
```


Misc Functions
--------------

Implementation of the famous ROW_NUMBER (http://msdn.microsoft.com/en-us/library/ms186734.aspx) function, which was previously only possible in MySQL with an ugly variable handling.
```
int row_number();

mysql> SELECT row_number() FROM t1;

### Determine how many rows are equal to an auto_increment column
mysql> SELECT count(id = row_number() or null) FROM t1;
```


A running SUM() for int and double/real values. Same as @x:= @x + value, but much faster and cleaner to code. The names are built up of R(unning)-SUM-(Int|Double)
```
int rsumi(int col);
double rsumd(double col);


mysql> SELECT rsumi(int_col) FROM t1;
mysql> SELECT rsumd(double_col) FROM t1;
```


A 64 bit hash function for MySQL, implementing a FNV algorithm
```
int fnv(string str);

mysql> SELECT cast( fnv( str ) as unsigned ) from t1;
```

Unfortunately, the UDF API doesn't support unsigned int return values. If you don't like the cast approach above, just add a stored function on your own:

```
CREATE FUNCTION ufnv(str TEXT) RETURNS INT UNSIGNED
   RETURN cast( fnv( str ) as unsigned );
```

String Functions
----------------

Cuts a string if it's longer then a max value and appends "...". Words are not chopped.
```
string cut(string str, int num[, string x='...']);

mysql> SELECT cut('This is the funny world of MySQL...', 15);
+------------------------------------------------+
| cut('This is the funny world of MySQL...', 15) |
+------------------------------------------------+
| This is the...                                 |
+------------------------------------------------+
1 row in set (0.00 sec)
```


The old name of this function was makeurl but I renamed it to slug, because this seems to be the more official term for this one. The Wordpress Codex says this: A slug is a few words that describe a post or a page. Slugs are usually a URL friendly version of the post title [...], but a slug can be anything you like. Slugs are meant to be used with permalinks as they help describe what the content at the URL is.
```
string slug(string str);

mysql> SELECT slug('Max Müller Straße!');
+----------------------------+
| slug('Max Müller Straße!') |
+----------------------------+
| max-mueller-strasse        |
+----------------------------+
1 row in set (0.00 sec)
```


Generates a list of ngrams from a given string.
```
string ngram(string str[, int size=2]);

mysql> SELECT ngram('Lorem ipsum dolor');
+-------------------------------------------------------+
| ngram('Lorem ipsum dolor')                            |
+-------------------------------------------------------+
| _l lo or re em m_ _i ip ps su um m_ _d do ol lo or r_ |
+-------------------------------------------------------+
1 row in set (0.01 sec)
```


Math Functions
--------------

Round to the next multiple of a base.
```
int bround(int num, int base);

mysql> SELECT bround(13, 3);
+---------------+
| bround(13, 3) |
+---------------+
|            15 |
+---------------+
1 row in set (0.00 sec)
```


Round to the next power of 10. This breaks down 10^ceil(log(n) / log(10))
```
int xround(int num);

mysql> SELECT xround(55);
+------------+
| xround(55) |
+------------+
|        100 |
+------------+
1 row in set (0.00 sec)
```


Limits a number to a specified lower min- and/or a upper max value. Not used min/max can be ignored with NULL
```
double bound(double num, double min, double max);

mysql> SELECT bound(12, 0, 4);
+-----------------+
| bound(12, 0, 4) |
+-----------------+
|      4.00000000 |
+-----------------+
1 row in set (0.00 sec)
```


Calculates N over K
```
int noverk(int n, int k);

mysql> select noverk(49, 6);
+---------------+
| noverk(49, 6) |
+---------------+
|      13983816 |
+---------------+
1 row in set (0.00 sec)
```


Binary Functions
----------------

Check if a bit-flag is set in the number.
```
bool isbit(int mask, int n);

mysql> SELECT isbit(5, 2);
+-------------+
| isbit(5, 2) |
+-------------+
|           1 |
+-------------+
1 row in set (0.00 sec)
```


Set a bit-flag at the position to 1 (or optionally 0).
```
int setbit(int mask, int n[, bool init=1]);

mysql> SELECT setbit(8, 4, 1);
+-----------------+
| setbit(8, 4, 1) |
+-----------------+
|              24 |
+-----------------+
1 row in set (0.00 sec)
```


Toggle/invert a bit-flag at the given position.
```
int invbit(int mask, int n);

mysql> SELECT invbit(8, 2);
+--------------+
| invbit(8, 2) |
+--------------+
|           12 |
+--------------+
1 row in set (0.00 sec)
```


Rotate the bits within the number
```
int rotbit(int mask, int n);

mysql> select rotbit(13, 1);
+---------------+
| rotbit(13, 1) |
+---------------+
|            26 |
+---------------+
1 row in set (0.00 sec)
```


Get a smaller integer from a bigger integer.
See: http://www.xarg.org/2009/12/store-small-numbers-in-big-numbers/
```
int getint(int mask, int x, int y);

mysql> SELECT GETINT(4283942, 4, 8);
+-----------------------+
| getint(4283942, 4, 8) |
+-----------------------+
|                     2 |
+-----------------------+
1 row in set (0.00 sec)
```

Set a smaller integer into a bigger integer.
See: http://www.xarg.org/2009/12/store-small-numbers-in-big-numbers/
```
int setint(int mask, int x, int y, int m);

mysql> SELECT SETINT(4283942, 4, 8, 10);
+---------------------------+
| setint(4283942, 4, 8, 10) |
+---------------------------+
|                   4284070 |
+---------------------------+
1 row in set (0.00 sec)
```

Testing
=======

udf_infusion contains a set of unit tests to verify the correctness
of the provided UDF functions. Running them after installation is optional.

Prerequisites:

* Python 2.7
* [numpy](http://www.numpy.org/)
* [scipy](http://scipy.org/)

**Note**: The testing framework requires all UDF functions to be enabled
during installation.

First, it is recommended you set connection details (incl. password) in
`~/.my.cnf`, e.g.:

```
[client]
user=<user>
password=<password>
```

Alternatively, you can set options to be passed to the MySQL client
in the `MYSQL_OPTIONS` environment variable.

To prepare the testing environment (requires administrator rights in MySQL):

```
make test_prepare
```

This may take a while as sample data is generated and imported.
Database `udf_infusion_test` is created and populated with generated data.

Run tests with:

```
make test
```

After completion, the temporary database can be dropped with `test_clean`:

```
make test_clean
```

License
======
Licensed under GPL Version 2 license.
