--
-- test_prepare.sql
--
-- Prepare testing environment.
--
-- Usage: mysql [options] < test_prepare.sql
--

drop database if exists udf_infusion_test;
create database udf_infusion_test;
use udf_infusion_test;

create table empty_table(x real, y real);

create table null_table(x real, y real);
insert into null_table values (NULL, NULL), (NULL, NULL);

create table small_table(x real, y real, g int);
insert into small_table values
    (1, -5, 1),
    (NULL, NULL, 1),
    (2, 1, 1),
    (3, NULL, 2),
    (4, 0, 2),
    (5, 0, 2),
    (NULL, 1, 2),
    (6, 1, 3),
    (7, -5, 3),
    (8, -5, 3);

create table example_table(x real, y real);
load data local infile 'example_table' into table example_table fields terminated by ' ';
update example_table set x = null where x = 1e+20;
update example_table set y = null where y = 1e+20;
