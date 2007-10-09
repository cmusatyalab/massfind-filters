## Process this file with automake to produce Makefile.in

filterdir = $(datadir)/massfind/filter
commondir = $(top_srcdir)/src/common

INCLUDES = -I$(commondir) -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DMASSFIND_FILTERDIR=\""$(filterdir)"\" \
	@OPENDIAMOND_CFLAGS@ @GLADE_CFLAGS@

AM_CFLAGS = -fPIC

lib_LIBRARIES = libfil_euclidian.a \
				libfil_boostldm.a \
				libfil_qaldm.a \
				libfil_visual.a

libfil_euclidian_a_SOURCES = fil_euclidian.c fil_euclidian.h $(commondir)/roi_features.h

libfil_boostldm_a_SOURCES = fil_boostldm.c fil_boostldm.h $(commondir)/roi_features.h

libfil_qaldm_a_SOURCES = fil_qaldm.c fil_qaldm.h $(commondir)/roi_features.h

libfil_visual_a_SOURCES = fil_visual.c fil_visual.h $(commondir)/roi_features.h $(commondir)/upmc_features.h

## override automake's rules
libfil_euclidian.a: fil_euclidian.o
	$(LD) -melf_i386 -o $@ -r $^

libfil_boostldm.a: fil_boostldm.o
	$(LD) -melf_i386 -o $@ -r $^

libfil_qaldm.a: fil_qaldm.o
	$(LD) -melf_i386 -o $@ -r $^

libfil_visual.a: fil_visual.o
	$(LD) -melf_i386 -o $@ -r $^
