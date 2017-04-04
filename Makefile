FIL_BUILD_COMMAND=$(CC) -Wall -O2 -g $$(pkg-config glib-2.0 opendiamond --cflags) $$(pkg-config opendiamond --libs) -lm -o $@ $< $(CPPFLAGS) $(CFLAGS) $(INCLUDES)

FILTERS := fil_euclidian fil_boostldm fil_qaldm fil_visual

all: $(FILTERS)

clean:
	$(RM) $(FILTERS)

fil_euclidian: fil_euclidian.c fil_euclidian.h roi_features.h
	$(FIL_BUILD_COMMAND)
fil_boostldm: fil_boostldm.c fil_boostldm.h roi_features.h
	$(FIL_BUILD_COMMAND)
fil_qaldm: fil_qaldm.c fil_qaldm.h roi_features.h
	$(FIL_BUILD_COMMAND)
fil_visual: fil_visual.c fil_visual.h upmc_features.h
	$(FIL_BUILD_COMMAND)
