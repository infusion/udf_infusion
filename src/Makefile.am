lib_LTLIBRARIES = udf_infusion.la

udf_infusion_la_SOURCES = \
    bound.c \
    bround.c \
    corr.c \
    covariance.c \
    cut.c \
    fnv.c \
    getint.c \
    group_first.c \
    group_last.c \
    invbit.c \
    isbit.c \
    kurtosis.c \
    lessavg.c \
    lesspartpct.c \
    lesspart.c \
    median.c \
    ngram.c \
    noverk.c \
    rotbit.c \
    rotint.c \
    row_number.c \
    rsumd.c \
    rsumi.c \
    setbit.c \
    setint.c \
    skewness.c \
    slug.c \
    _translate_string.c \
    xround.c

udf_infusion_la_CFLAGS = -DSTANDARD -DMYSQL_SERVER @MYSQL_CFLAGS@
udf_infusion_la_CXXFLAGS = -DSTANDARD -DMYSQL_SERVER @MYSQL_CFLAGS@
udf_infusion_la_LDFLAGS = -module -avoid-version -no-undefined -rpath @MYSQL_PLUGINDIR@
