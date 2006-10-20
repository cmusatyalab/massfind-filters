## Process this file with automake to produce Makefile.in

gladedir = $(datadir)/massfind/glade
filterdir = $(datadir)/massfind/filter
libfilterdir = ${DIAMOND_PREFIX}/lib

INCLUDES = -I$(gladesrcs) \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DMASSFIND_GLADEDIR=\""$(gladedir)"\" \
	-DMASSFIND_FILTERDIR=\""$(filterdir)"\" \
	-DDIAMOND_FILTERDIR=\""${libfilterdir}"\" \
	@glade_CFLAGS@

bin_PROGRAMS = massfind

massfind_SOURCES = \
	main.c massfind.h \
	select.c select.h \
	roimap.c roimap.h \
	define.c define.h \
	search.c search.h \
	diamond_interface.c diamond_interface.h

massfind_LDADD = @glade_LIBS@ 

massfind_LDFLAGS = -export-dynamic
