## Process this file with automake to produce Makefile.in

filterdir = $(datadir)/massfind/filter

INCLUDES = -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DMASSFIND_FILTERDIR=\""$(filterdir)"\" \
	@glade_CFLAGS@

lib_LIBRARIES = libfil_euclidian.a \
				libfil_boostldm.a \
				libfil_qaldm.a 

libfil_euclidian_a_SOURCES = fil_euclidian.c fil_euclidian.h
	
libfil_boostldm_a_SOURCES = fil_boostldm.c fil_boostldm.h

libfil_qaldm_a_SOURCES = fil_qaldm.c fil_qaldm.h



