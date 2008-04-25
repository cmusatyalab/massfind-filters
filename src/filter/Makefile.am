## Process this file with automake to produce Makefile.in

filterdir = $(datadir)/massfind/filter
commondir = $(top_srcdir)/src/common

INCLUDES = -I$(commondir) -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DMASSFIND_FILTERDIR=\""$(filterdir)"\" \
	@OPENDIAMOND_CFLAGS@ @GLADE_CFLAGS@

AM_CFLAGS += -fPIC

lib_LIBRARIES = libfil_euclidian.so \
				libfil_boostldm.so \
				libfil_qaldm.so \
				libfil_visual.so

libfil_euclidian_so_SOURCES = fil_euclidian.c fil_euclidian.h $(commondir)/roi_features.h

libfil_boostldm_so_SOURCES = fil_boostldm.c fil_boostldm.h $(commondir)/roi_features.h

libfil_qaldm_so_SOURCES = fil_qaldm.c fil_qaldm.h $(commondir)/roi_features.h

libfil_visual_so_SOURCES = fil_visual.c fil_visual.h $(commondir)/roi_features.h $(commondir)/upmc_features.h

FIL_BUILD_COMMAND=$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) $(AM_CFLAGS) -shared @OPENDIAMOND_CFLAGS@ @GLADE_CFLAGS@ -o $@ $<

## override automake's rules
libfil_euclidian.so: fil_euclidian.o
	$(FIL_BUILD_COMMAND)

libfil_boostldm.so: fil_boostldm.o
	$(FIL_BUILD_COMMAND)

libfil_qaldm.so: fil_qaldm.o
	$(FIL_BUILD_COMMAND)

libfil_visual.so: fil_visual.o
	$(FIL_BUILD_COMMAND)
