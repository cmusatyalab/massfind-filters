dnl Process this file with autoconf to produce a configure script.

AC_INIT(MassFind, 0.1)
AC_CONFIG_SRCDIR(src)
AM_INIT_AUTOMAKE(foreign)
AM_CONFIG_HEADER(config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_CXX
AC_HEADER_STDC
RANLIB=:
AC_SUBST(RANLIB)

pkg_modules="gtk+-2.0 >= 2.8.0 \
	libglade-2.0 >= 2.5.1"

PKG_CHECK_MODULES(GLADE, [$pkg_modules])
AC_SUBST(GLADE_CFLAGS)
AC_SUBST(GLADE_LIBS)

PKG_CHECK_MODULES(OPENDIAMOND, [opendiamond])
AC_SUBST(OPENDIAMOND_CFLAGS)
AC_SUBST(OPENDIAMOND_LIBS)


AC_ARG_WITH(filters,
    [  --with-filters=<path> path of filters installation. e.g. /usr/local/diamond/lib or /opt/diamond/lib],
    [FILTERS_PREFIX=$with_filters],
    [FILTERS_PREFIX=/usr/local/diamond/lib])

AC_SUBST(FILTERS_PREFIX)
AC_SUBST(AM_CPPFLAGS, "-Wall -Werror-implicit-function-declaration")

AC_SEARCH_LIBS([sqrt], [m],, AC_MSG_FAILURE([cannot find math library]))
AC_SEARCH_LIBS([pthread_create],
	[pthread],, AC_MSG_FAILURE([cannot find pthread_create function]))

# XXX should work on more than 1 arch
AC_SUBST(AM_CFLAGS, "-m32")


# this filter library is installed with snapfind
AC_CHECK_FILE([${FILTERS_PREFIX}/fil_rgb.a],,
			[AC_MSG_ERROR([Missing fil_rgb.a, consider using --with-filters option])])

AC_OUTPUT([
Makefile
src/Makefile
src/app/Makefile
src/filter/Makefile
data/Makefile
data/glade/Makefile
data/filter/Makefile
])

