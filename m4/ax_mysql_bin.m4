#####
#
# SYNOPSIS
#
#   AX_MYSQL_BIN()
#
# DESCRIPTION
#
#   This macro defines location of MySQL client (mysql), mysqladmin and mysqltest
#
#   This macro calls:
#
#     AC_SUBST(MYSQL)
#     AC_SUBST(MYSQLADMIN)
#     AC_SUBST(MYSQLTEST)
#
#####

AC_DEFUN([AX_MYSQL_BIN], [
  AC_MSG_CHECKING(for mysqlbin)
  AC_ARG_WITH(mysqlbin,
    [[  --with-mysqlbin[=CMD]   command to run mysql.]],
    [withmysqlbin="$withval"],
    [withmysqlbin="yes"]
  )

  AC_ARG_WITH(mysqladmin,
    [[  --with-mysqladmin[=CMD] command to run mysqladmin.]],
    [withmysqladmin="$withval"],
    [withmysqladmin="yes"]
  )

  AC_ARG_WITH(mysqltest,
    [[  --with-mysqltest[=CMD]  command to run mysqltest.]],
    [withmysqltest="$withval"],
    [withmysqltest="yes"]
  )

  if test -f /etc/mysql/debian.cnf ; then
    MYSQL_ARGS="--defaults-file=/etc/mysql/debian.cnf"
  else
    MYSQL_ARGS="-p"
  fi

  if test -z "$withmysqlbin" -o "$withmysqlbin" = "yes"; then
    MYSQL=`which mysql`;
    if test -n "$MYSQL"; then MYSQL="$MYSQL $MYSQL_ARGS"; fi
  elif test "$withmysqlbin" != "no"; then
    MYSQL="$withmysqlbin"
  fi

  if test -n "$MYSQL" ; then
    MYSQLBIN_PATH=`dirname "$MYSQL"`
  fi
  
  if test -z "$withmysqladmin" -o "$withmysqladmin" = "yes"; then
    if test -n "$MYSQLBIN_PATH" -a -f "$MYSQLBIN_PATH/mysqladmin"; then
      MYSQLADMIN="$MYSQLBIN_PATH/mysqladmin";
    else
      MYSQLADMIN=`which mysqladmin`;
    fi
    
    if test -n "$MYSQLADMIN"; then MYSQLADMIN="$MYSQLADMIN $MYSQL_ARGS"; fi
  elif test "$withmysqladmin" != "no"; then
    MYSQLADMIN="$withmysqladmin"
  fi

  if test -z "$withmysqltest" -o "$withmysqltest" = "yes"; then
    if test -n "$MYSQLBIN_PATH" -a -f "$MYSQLBIN_PATH/mysqltest"; then
      MYSQLTEST="$MYSQLBIN_PATH/mysqltest";
    else
      MYSQLTEST=`which mysqltest`;
    fi
    
    if test -n "$MYSQLTEST"; then MYSQLTEST="$MYSQLTEST $MYSQL_ARGS"; fi
  elif test "$withmysqltest" != "no"; then
    MYSQLTEST="$withmysqltest"
  fi

  AC_SUBST(MYSQL)
  AC_SUBST(MYSQLADMIN)
  AC_SUBST(MYSQLTEST)
])