[ $# -eq 0 ] && enable_all=1
enable_functions="$@"

if_enable() {
    [ "$enable_all" = 1 ] && return 0
    for func in $enable_functions; do
        [ "$func" = "$1" ] && return 0
    done
    return 1
}

create_agg_function() {
    echo "CREATE AGGREGATE FUNCTION $1 RETURNS $2 SONAME 'udf_infusion.so';"
}

create_function() {
    echo "CREATE FUNCTION $1 RETURNS $2 SONAME 'udf_infusion.so';"
}

sh unload.sql.sh

if_enable "bound" && create_function "bound" "real"
if_enable "bround" && create_function "bround" "real"
if_enable "corr" && create_agg_function "corr" "real"
if_enable "covariance" && create_agg_function "covariance" "real"
if_enable "cut" && create_function "cut" "string"
if_enable "fnv" && create_function "fnv" "integer"
if_enable "getint" && create_function "getint" "integer"
if_enable "group_first" && create_agg_function "group_first" "string"
if_enable "group_last" && create_agg_function "group_last" "string"
if_enable "invbit" && create_function "invbit" "integer"
if_enable "isbit" && create_function "isbit" "integer"
if_enable "kurtosis" && create_agg_function "kurtosis" "real"
if_enable "lessavg" && create_agg_function "lessavg" "integer"
if_enable "lesspartpct" && create_agg_function "lesspartpct" "integer"
if_enable "lesspart" && create_agg_function "lesspart" "integer"
if_enable "median" && create_agg_function "median" "real"
if_enable "stat_mode" && create_agg_function "stat_mode" "real"
if_enable "ngram" && create_function "ngram" "string"
if_enable "noverk" && create_function "noverk" "integer"
if_enable "percentile_cont" && create_agg_function "percentile_cont" "real"
if_enable "percentile_disc" && create_agg_function "percentile_disc" "real"
if_enable "rotbit" && create_function "rotbit" "integer"
if_enable "rotint" && create_function "rotint" "integer"
if_enable "row_number" && create_function "row_number" "integer"
if_enable "rsumd" && create_function "rsumd" "real"
if_enable "rsumi" && create_function "rsumi" "integer"
if_enable "setbit" && create_function "setbit" "integer"
if_enable "setint" && create_function "setint" "integer"
if_enable "skewness" && create_agg_function "skewness" "real"
if_enable "slug" && create_function "slug" "string"
if_enable "xround" && create_function "xround" "integer"

true
