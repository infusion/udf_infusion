AC_PREREQ([2.59])

AC_INIT([lib_udf_infusion], [1.0], [robert@xarg.org])

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([foreign])

LT_PREREQ([2.2])
LT_INIT([dlopen disable-static])

AC_PROG_CC
AC_PROG_CXX

m4_include([m4/ax_lib_mysql.m4])
AX_LIB_MYSQL(,,AC_MSG_ERROR("Can't find mysql library" ))

if test -n "$MYSQL_PLUGINDIR" && test "$libdir" == '${exec_prefix}/lib' ; then
  AC_MSG_NOTICE([setting libdir to mysql plugin dir $MYSQL_PLUGINDIR])
  libdir=$MYSQL_PLUGINDIR
  AC_SUBST([libdir])
fi

m4_include([m4/ax_mysql_bin.m4])
AX_MYSQL_BIN

AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_HEADERS([limits.h syslimits.h])

AC_ARG_ENABLE([functions], AS_HELP_STRING(
    [--enable-functions=ARG],
    [enable only selected UDF functions]
))

AC_SUBST([enable_functions])

AC_OUTPUT([Makefile src/Makefile test/Makefile])
