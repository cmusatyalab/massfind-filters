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
#include "fil_boostldm.h"

float alpha[] = {
	0.007374,
	0.006751,
	0.006085,
	0.005392,
	0.004878,
	0.005169,
	0.004278,
	0.004753,
	0.004315,
	0.004213,
	0.003821,
	0.003895,
	0.003388,
	0.003294,
	0.003173,
	0.002754,
	0.003023,
	0.002458,
	0.002927,
	0.002564,
	0.002354,
	0.002558,
	0.001994,
	0.002199,
	0.002301,
	0.002255,
	0.002086,
	0.001783,
	0.001675,
	0.001647,
	0.001723,
	0.001441,
	0.001490,
	0.001626,
	0.001322,
	0.001355,
	0.001623,
	0.001425,
	0.001265,
	0.001204,
	0.001601,
	0.001090,
	0.001482,
	0.000988,
	0.001376,
	0.000895,
	0.001256,
	0.001163,
	0.000722,
	0.000724,
};


int f_init_boostldm(int numarg, char **args, int blob_len,
                    void *blob, const char *fname, void **data)
{
	boostldm_config_t *fconfig;
	int i;

	/*
	 * save the features for the source object
	 */
	fconfig = (boostldm_config_t *)malloc(sizeof(*fconfig));
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

int f_fini_boostldm(void *data)
{
	boostldm_config_t *fconfig = (boostldm_config_t *) data;
	free(fconfig->features);
	free(fconfig);
	
	return (0);
}


int f_eval_boostldm(lf_obj_handle_t ohandle, void *f_data)
{
	int err;
	int i;
	boostldm_config_t *fconfig = (boostldm_config_t *) f_data;
	size_t featureLen = MAXFEATURELEN;
	unsigned char featureStr[MAXFEATURELEN];
	int numFeatures;
	float f;
	float distance = 0;
	char fname[MAXFNAMELEN];
	
	lf_log(LOGL_TRACE, "f_eval_boostldm: enter");
	
	// extract the features for this object
	err = lf_read_attr(ohandle, NUM_BDMF, &featureLen, featureStr);
	assert(err == 0);
	numFeatures = atoi((char *)featureStr);
	assert(numFeatures == fconfig->numFeatures);

	for(i=0; i<numFeatures; i++) {
		sprintf(fname, "%s%02d", BDMF_PREFIX, i);
		featureLen = MAXFEATURELEN;  // reset, o.w. could be too small
		err = lf_read_attr(ohandle, fname, &featureLen, featureStr);
		assert(err == 0);
		f = atof((char *)featureStr);

		// Given two objects x1[] and x2[], each with 51 bits
		// The weights (alpha) for each bit are given in a file called attr/alpha.
		// We can hardcode them into the searchlet.
	    distance += alpha[i] * (fconfig->features[i] != f);
	}
	
	// scale distance to spread out results
	int similarity = 100*exp(-distance);
	
	// save results as attributes
	err = lf_write_attr(ohandle, "similarity", sizeof(int), 
	   					(unsigned char *) &similarity);
	assert(err == 0);

	return similarity;
}



