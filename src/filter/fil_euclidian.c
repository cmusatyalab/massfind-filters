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

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "lib_filter.h"
#include "roi_features.h"
#include "fil_euclidian.h"


int f_init_euclidian(int numarg, char **args, int blob_len,
                    void *blob, const char *fname, void **data)
{
	euclidian_config_t *fconfig;
	int i;

	/*
	 * save the features for the source object
	 */
	fconfig = (euclidian_config_t *)malloc(sizeof(*fconfig));
	assert(fconfig);

	fconfig->numFeatures = numarg;
	fconfig->features = (float *) malloc(sizeof(float) * fconfig->numFeatures);
    for (i = 0; i < fconfig->numFeatures; i++) {
    	fconfig->features[i] = atof(args[i]);
     }

	/*
	 * save the data pointer 
	 */
	*data = (void *) fconfig;
	
	return (0);
}

int f_fini_euclidian(void *data)
{
	euclidian_config_t *fconfig = (euclidian_config_t *) data;
	free(fconfig->features);
	free(fconfig);
	
	return (0);
}


int f_eval_euclidian(lf_obj_handle_t ohandle, void *f_data)
{
	int err;
	int i;
	euclidian_config_t *fconfig = (euclidian_config_t *) f_data;
	size_t featureLen = MAXFEATURELEN;
	unsigned char featureStr[MAXFEATURELEN];
	int numFeatures;
	float f;
	float distance = 0;
	char fname[MAXFNAMELEN];
	
	lf_log(LOGL_TRACE, "f_eval_euclidian: enter");
	
	// extract the features for this object
	err = lf_read_attr(ohandle, NUM_EDMF, &featureLen, featureStr);
	assert(err == 0);
	numFeatures = atoi((char *)featureStr);
	assert(numFeatures == fconfig->numFeatures);

	for(i=0; i<numFeatures; i++) {
		sprintf(fname, "%s%02d", EDMF_PREFIX, i);
		featureLen = MAXFEATURELEN;  // reset, o.w. could be too small
		err = lf_read_attr(ohandle, fname, &featureLen, featureStr);
		assert(err == 0);
		f = atof((char *)featureStr);
		distance=distance+pow((f-fconfig->features[i]),2);
	}
	int similarity = 100*exp(-distance);
	
	// save results as attributes
	err = lf_write_attr(ohandle, "similarity", sizeof(int), 
	   					(unsigned char *) &similarity);
	assert(err == 0);

	return similarity;
}
