FIL_BUILD_COMMAND=$(CC) -O2 -g -fPIC -shared $$(pkg-config glib-2.0 opendiamond --cflags) -m32 -o $@ $< $(CPPFLAGS) $(CFLAGS) $(INCLUDES)

FILTERS := libfil_euclidian.so libfil_boostldm.so libfil_qaldm.so libfil_visual.so

all: $(FILTERS)

clean:
	$(RM) $(FILTERS)

libfil_euclidian.so: fil_euclidian.c fil_euclidian.h roi_features.h
	$(FIL_BUILD_COMMAND)
libfil_boostldm.so: fil_boostldm.c fil_boostldm.h roi_features.h
	$(FIL_BUILD_COMMAND)
libfil_qaldm.so: fil_qaldm.c fil_qaldm.h roi_features.h
	$(FIL_BUILD_COMMAND)
libfil_visual.so: fil_visual.c fil_visual.h upmc_features.h
	$(FIL_BUILD_COMMAND)
