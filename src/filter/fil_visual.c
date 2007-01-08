/*
 * MassFind: A Diamond application for exploration of breast tumors
 *
 * Copyright (c) 2006 Intel Corporation. All rights reserved.
 * Additional copyrights may be listed below.
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0 which accompanies this
 * distribution in the file named LICENSE.
 *
 * Technical and financial contributors are listed in the file named
 * CREDITS.
 */
 
/*
 * fil_visual.c - filtering based on visual features
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "lib_filter.h"
#include "upmc_features.h"
#include "roi_features.h"
#include "fil_visual.h"


int f_init_visual(int numarg, char **args, int blob_len,
                    void *blob, const char *fname, void **data)
{
	visual_config_t *fconfig;
	int i;

	/*
	 * save the features for the source object
	 */
	fconfig = (visual_config_t *)malloc(sizeof(*fconfig));
	assert(fconfig);

	fconfig->numFeatures = numarg-4;
	fconfig->features = (float *) malloc(sizeof(float) * fconfig->numFeatures);
    for (i = 0; i < fconfig->numFeatures; i++) {
    	fconfig->features[i] = atof(args[i]);
    }
    fconfig->size_mult_lower = atof(args[numarg-4]);
    fconfig->size_mult_upper = atof(args[numarg-3]);
    fconfig->circ_mult_lower = atof(args[numarg-2]);
    fconfig->circ_mult_upper = atof(args[numarg-1]);

	/*
	 * save the data pointer 
	 */
	*data = (void *) fconfig;
	
	return (0);
}

int f_fini_visual(void *data)
{
	visual_config_t *fconfig = (visual_config_t *) data;
	free(fconfig->features);
	free(fconfig);
	
	return (0);
}


int f_eval_visual(lf_obj_handle_t ohandle, void *f_data)
{
	int err;
	int i;
	visual_config_t *fconfig = (visual_config_t *) f_data;
	size_t featureLen = MAXFEATURELEN;
	unsigned char featureStr[MAXFEATURELEN];
	float size, circularity;
	float r_min, r_max;
	char fname[7];
	int inRange;
	
	lf_log(LOGL_TRACE, "f_eval_visual: enter");
	
	if (fconfig->size_mult_lower > 0) {
		// read the object's region size
		sprintf(fname, "%s%02d", UPMC_PREFIX, UPMC_REGION_SIZE);
		err = lf_read_attr(ohandle, fname, &featureLen, featureStr);
		assert(err == 0);
		size = atof((char *)featureStr);
		
		// compare to query image region size
		r_max = fconfig->features[UPMC_REGION_SIZE] * 
				fconfig->size_mult_upper;
		r_min = fconfig->features[UPMC_REGION_SIZE] * 
				fconfig->size_mult_lower;
		if (size < r_min || size > r_max) 
			return 0;
	}
	
	if (fconfig->circ_mult_lower > 0) {
		// read the object's circularity
		featureLen = MAXFEATURELEN;  // reset, o.w. could be too small
		sprintf(fname, "%s%02d", UPMC_PREFIX, UPMC_REGION_CIRCULARITY);
		err = lf_read_attr(ohandle, fname, &featureLen, featureStr);
		assert(err == 0);
		circularity = atof((char *)featureStr);
		
		// compare to query image region circularity
		r_max = fconfig->features[UPMC_REGION_CIRCULARITY] * 
				fconfig->circ_mult_upper;
		r_min = fconfig->features[UPMC_REGION_CIRCULARITY] * 
				fconfig->circ_mult_lower;
		if (circularity < r_min || circularity > r_max) 
			return 0;
	}

	return 1;
}
