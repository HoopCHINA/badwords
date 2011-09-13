dnl
dnl $Id: config9.m4 303962 2011-08-09 15:48:23Z Wang Wenlin $
dnl

PHP_ARG_ENABLE(badwords, whether to enable badwords support,
[  --enable-badwords       Enable badwords support])

if test "$PHP_BADWORDS" != "no"; then

    AC_DEFINE(HAVE_BADWORDS,1,[Whether you want badwords support])
    PHP_NEW_EXTENSION(badwords, badwords.c compiler.c php_badwords.c, $ext_shared)

dnl this is needed to build the extension with phpize and -Wall

  if test "$PHP_DEBUG" = "yes"; then
    CFLAGS="$CFLAGS -Wall"
  fi

fi
