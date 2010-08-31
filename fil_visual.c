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
 */
 
/*
 * fil_visual.c - filtering based on visual features
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <glib.h>

#include "lib_filter.h"
#include "upmc_features.h"
#include "roi_features.h"
#include "fil_visual.h"

#define MAX_ATTR_NAME           128
#define MAX_ATTR_VALUE          4096

int f_init_visual(int numarg, const char * const *args, int blob_len,
                    const void *blob, const char *fname, void **data)
{
	visual_config_t *fconfig;
	int i;

	/*
	 * save the features for the source object
	 */
	fconfig = (visual_config_t *)malloc(sizeof(*fconfig));
	assert(fconfig);

    /* Hardcoded value???  */
	fconfig->numFeatures = numarg-6;
	fconfig->features = (float *) malloc(sizeof(float) * fconfig->numFeatures);
    for (i = 0; i < fconfig->numFeatures; i++) {
    	fconfig->features[i] = atof(args[i]);
    }
    fconfig->size_mult_lower = atof(args[numarg-6]);
    fconfig->size_mult_upper = atof(args[numarg-5]);
    fconfig->circ_mult_lower = atof(args[numarg-4]);
    fconfig->circ_mult_upper = atof(args[numarg-3]);
    fconfig->sfr_mult_lower = atof(args[numarg-2]);
    fconfig->sfr_mult_upper = atof(args[numarg-1]);

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
	size_t featureLen = MAX_ATTR_VALUE;
	unsigned char featureStr[MAX_ATTR_VALUE];
	float size, circularity, shapefactor;
	float r_min, r_max;
	char fname[MAX_ATTR_NAME];
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
		featureLen = MAX_ATTR_VALUE;  // reset, o.w. could be too small
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

    /* Shape factor ratio */
	if (fconfig->sfr_mult_lower > 0) {

		// read the object's shape factor ratio
		sprintf(fname, "%s%02d", UPMC_PREFIX, UPMC_SHAPE_FACTOR_RATIO);
		err = lf_read_attr(ohandle, fname, &featureLen, featureStr);
		assert(err == 0);
		shapefactor = atof((char *)featureStr);
		
		// compare to query shape factor ratio
		r_max = fconfig->features[UPMC_SHAPE_FACTOR_RATIO] * 
				fconfig->sfr_mult_upper;
		r_min = fconfig->features[UPMC_SHAPE_FACTOR_RATIO] * 
				fconfig->sfr_mult_lower;
		if (shapefactor < r_min || shapefactor > r_max) 
			return 0;
	}

	return 1;
}
