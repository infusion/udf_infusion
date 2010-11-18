
CREATE FUNCTION isbit RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION setbit RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION invbit RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION numbit RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION msbit RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION setint RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION getint RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION bround RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION xround RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION bound RETURNS real SONAME 'udf_infusion.so';
CREATE FUNCTION cut RETURNS string SONAME 'udf_infusion.so';
CREATE FUNCTION slug RETURNS string SONAME 'udf_infusion.so';
CREATE FUNCTION thumbscale RETURNS integer SONAME 'udf_infusion.so';
CREATE FUNCTION thumbratio RETURNS real SONAME 'udf_infusion.so';
CREATE FUNCTION starratio RETURNS real SONAME 'udf_infusion.so';
CREATE FUNCTION sigfig RETURNS real SONAME 'udf_infusion.so';
