mysql_version="$1"
mysql_version_major=`expr $mysql_version : '\([[0-9]]*\)'`
shift

[ $# -eq 0 ] && enable_all=1
enable_functions="$@"

if_enable() {
    [ "$enable_all" = 1 ] && return 0
    for func in $enable_functions; do
        [ "$func" = "$1" ] && return 0
    done
    return 1
}

drop_function() {
    echo "DROP FUNCTION IF EXISTS $1;"
}

if_enable "bound" && drop_function "bound"
if_enable "bround" && drop_function "bround"
if_enable "corr" && drop_function "corr"
if_enable "covariance" && drop_function "covariance"
if_enable "cut" && drop_function "cut"
if_enable "fnv" && drop_function "fnv"
if_enable "getint" && drop_function "getint"
if_enable "group_first" && drop_function "group_first"
if_enable "group_last" && drop_function "group_last"
if_enable "invbit" && drop_function "invbit"
if_enable "isbit" && drop_function "isbit"
if_enable "kurtosis" && drop_function "kurtosis"
if_enable "lessavg" && drop_function "lessavg"
if_enable "lesspartpct" && drop_function "lesspartpct"
if_enable "lesspart" && drop_function "lesspart"
if_enable "med" && drop_function "med"
if_enable "stats_mode" && drop_function "stats_mode"
if_enable "ngram" && drop_function "ngram"
if_enable "noverk" && drop_function "noverk"
if_enable "percentile_contin" && drop_function "percentile_contin"
if_enable "percentile_discr" && drop_function "percentile_discr"
if_enable "rotbit" && drop_function "rotbit"
if_enable "rotint" && drop_function "rotint"
if_enable "rsumd" && drop_function "rsumd"
if_enable "rsumi" && drop_function "rsumi"
if_enable "setbit" && drop_function "setbit"
if_enable "setint" && drop_function "setint"
if_enable "skewness" && drop_function "skewness"
if_enable "slug" && drop_function "slug"
if_enable "xround" && drop_function "xround"

true
