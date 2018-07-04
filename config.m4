dnl
dnl $Id: config9.m4 2011-08-09 15:48:23Z Wang Wenlin/2018-04-11 18:19:23Z wlmwang $
dnl

PHP_ARG_ENABLE(badwords, whether to enable badwords support,
[  --enable-badwords       Enable badwords support])

if test "$PHP_BADWORDS" != "no"; then
	
	AC_DEFINE(HAVE_BADWORDS,1,[Whether you want badwords support])

dnl PHP-7.*.*
dnl PHP_NEW_EXTENSION(badwords, badwords.c compiler.c php_badwords.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
    
    PHP_NEW_EXTENSION(badwords, badwords.c compiler.c php_badwords.c, $ext_shared)

dnl this is needed to build the extension with phpize and -Wall

dnl  if test "$PHP_DEBUG" = "yes"; then
dnl    CFLAGS="$CFLAGS -Wall"
dnl  fi

if test -z "$PHP_DEBUG"; then
    AC_ARG_ENABLE(debug,
    	[ --enable-debug  compile with debugging system],
    	[ PHP_DEBUG=$enableval], [PHP_DEBUG=no])
fi

fi
