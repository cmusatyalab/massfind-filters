## Process this file with automake to produce Makefile.in

filterdir = $(datadir)/massfind/filter
commondir = $(top_srcdir)/src/common

INCLUDES = -I$(commondir) -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DMASSFIND_FILTERDIR=\""$(filterdir)"\" \
	@GLADE_CFLAGS@

lib_LIBRARIES = libfil_euclidian.a \
				libfil_boostldm.a \
				libfil_qaldm.a \
				libfil_visual.a

libfil_euclidian_a_SOURCES = fil_euclidian.c fil_euclidian.h roi_features.h

libfil_boostldm_a_SOURCES = fil_boostldm.c fil_boostldm.h roi_features.h

libfil_qaldm_a_SOURCES = fil_qaldm.c fil_qaldm.h roi_features.h

libfil_visual_a_SOURCES = fil_visual.c fil_visual.h roi_features.h upmc_features.h

## override automake's rules
libfil_euclidian.a: fil_euclidian.o
	$(LD) -o $@ -r $^

libfil_boostldm.a: fil_boostldm.o
	$(LD) -o $@ -r $^

libfil_qaldm.a: fil_qaldm.o
	$(LD) -o $@ -r $^

libfil_visual.a: fil_visual.o
	$(LD) -o $@ -r $^
