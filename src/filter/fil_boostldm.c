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
#include "fil_boostldm.h"

#define MAXFEATURELEN 80

float alpha[] = {
	0.073741,
 	0.037224,
	0.027064,
	-0.013949,
	0.016688,
	0.014573,
	-0.006509,
	0.009755,
	0.004959,
	-0.010046,
	0.008374,
	-0.011017,
	0.012997,
	-0.008609,
	0.007056,
	-0.008419,
	0.007921,
	-0.008986,
	0.008272,
	-0.007504,
	0.007661,
	0.006753,
	-0.007724,
	-0.013358,
	0.012828,
	-0.009652,
	0.015934,
	0.002467,
	-0.008975,
	-0.008448,
	0.010927,
	0.005829,
	-0.013997,
	-0.004940,
	0.005753,
	-0.007851,
	0.004847,
	0.002199,
	-0.008542,
	-0.004077,
	0.005491,
	0.002508,
	0.003249,
	-0.007911,
	0.005953,
	-0.008888,
	0.007674,
	0.008976,
	0.012050,
	1.000000,
	1.000000,
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
	char fname[7];
	
	lf_log(LOGL_TRACE, "f_eval_boostldm: enter");
	
	// extract the features for this object
	err = lf_read_attr(ohandle, "numbdmf", &featureLen, featureStr);
	assert(err == 0);
	numFeatures = atoi((char *)featureStr);
	assert(numFeatures == fconfig->numFeatures);

// use first three features to avoid going negative
	for(i=0; i<numFeatures; i++) {
//	for (i = 0; i < 3; i++) {
		sprintf(fname, "bdmf%02d", i);
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



