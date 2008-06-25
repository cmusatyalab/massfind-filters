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

#include "diamond_consts.h"
#include "lib_filter.h"
#include "roi_features.h"
#include "fil_boostldm.h"

float alpha[] = {
		0.000990,
		0.000953,
		0.000903,
		0.000887,
		0.000867,
		0.000830,
		0.000874,
		0.000940,
		0.001491,
		0.001329,
		0.001395,
		0.001146,
		0.001513,
		0.001060,
		0.001295,
		0.001171,
		0.001373,
		0.001244,
		0.001256,
		0.001062,
		0.001554,
		0.001347,
		0.001172,
		0.001288,
		0.001495,
		0.001421,
		0.001327,
		0.000851,
		0.001508,
		0.001404,
		0.001189,
		0.001464,
		0.001382,
		0.000968,
		0.001650,
		0.001441,
		0.000782,
		0.001505,
		0.001552,
		0.000801,
		0.001259,
		0.001406,
		0.001050,
		0.001598,
		0.000710,
		0.001546,
		0.001229,
		0.001120,
		0.001239,
		0.001387,
		0.001438,
		0.001195,
		0.001103,
		0.001459,
		0.001330,
		0.001463,
		0.000651,
		0.001472,
		0.001353,
		0.000672,
		0.001329,
		0.001304,
		0.001310,
		0.000850,
		0.001431,
		0.001452,
		0.001145,
		0.001501,
		0.000586,
		0.000849,
		0.001305,
		0.000751,
		0.001419,
		0.000557,
		0.001023,
		0.000634,
		0.000746,
		0.001157,
		0.000810,
		0.000280,
		0.001325,
		0.000239,
		0.001020,
		0.000237,
		0.001469,
		0.000201,
		0.000193,
		0.000322,
		0.000463,
		0.000971,
		0.000260,
		0.000499,
		0.000827,
		0.000807,
		0.000961,
		0.000789,
		0.001428,
		0.000354,
		0.000892,
		0.000732,
		0.000954,
		0.000459,
		0.001121,
		0.000933,
		0.000447,
		0.001219,
		0.000609,
		0.000880,
		0.001323,
		0.000341,
		0.001313,
		0.000320,
		0.000820,
		0.000403,
		0.000729,
		0.000980,
		0.000727,
		0.000829,
		0.000919,
		0.000758,
		0.000883,
		0.000826,
		0.000673,
		0.000960,
		0.000483,
		0.000649,
		0.000719,
		0.001040,
		0.000207,
		0.000112,
		0.000160,
		0.000561,
		0.000342,
		0.000693,
		0.000772,
		0.000718,
		0.000547,
		0.000599,
		0.000562,
		0.001138,
		0.000696,
		0.000890,
		0.000558,
		0.000837,
		0.000741,
		0.000752,
		0.000713,
		0.000691,
		0.000872,
		0.000780,
		0.000597,
		0.000685,
		0.001010,
		0.000792,
		0.000685,
		0.001038,
		0.000154,
		0.000822,
		0.000301,
		0.000268,
		0.000755,
		0.001054,
		0.000958,
		0.000498,
		0.001138,
		0.000635,
		0.000751,
		0.000450,
		0.001043,
		0.000742,
		0.000381,
		0.000835,
		0.000291,
		0.000724,
		0.000510,
		0.000548,
		0.000584,
		0.000834,
		0.000305,
		0.000386,
		0.000583,
		0.000747,
		0.000240,
		0.000737,
		0.000219,
		0.000504,
		0.000724,
		0.000821,
		0.000472,
		0.000669,
		0.000520,
		0.000358,
		0.000838,
		0.000652,
		0.000320,
		0.000937,
		0.000385,
		0.000865,
		0.000339,
		0.000679,
		0.000176,
		0.000470,
		0.000544,
		0.000674,
		0.000273,
		0.000593,
		0.000684,
		0.000866,
		0.000422,
		0.000260,
		0.000740,
		0.000336,
		0.000723,
		0.000436,
		0.000382,
		0.000608,
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
	size_t featureLen = MAX_ATTR_VALUE;
	unsigned char featureStr[MAX_ATTR_VALUE];
	int numFeatures;
	float f;
	float distance = 0;
	char fname[MAX_ATTR_NAME];
	
	lf_log(LOGL_TRACE, "f_eval_boostldm: enter");
	
	// extract the features for this object
	err = lf_read_attr(ohandle, NUM_BDMF, &featureLen, featureStr);
	assert(err == 0);
	numFeatures = atoi((char *)featureStr);
	assert(numFeatures == fconfig->numFeatures);

	for(i=0; i<numFeatures; i++) {
		sprintf(fname, "%s%02d", BDMF_PREFIX, i);
		featureLen = MAX_ATTR_VALUE;  // reset, o.w. could be too small
		err = lf_read_attr(ohandle, fname, &featureLen, featureStr);
		assert(err == 0);
		f = atof((char *)featureStr);

		// Given two objects x1[] and x2[], each with 51 bits
		// The weights (alpha) for each bit are given in a file called attr/alpha.
		// We can hardcode them into the searchlet.
	    distance += alpha[i] * (fconfig->features[i] != f);
	}
	
	// scale distance to spread out results
	int similarity = 100*expf(-10.0*distance);
	
	//printf("distance = %f, similarity %d\n", distance, similarity);
    		
	// save results as attributes
	err = lf_write_attr(ohandle, "similarity", sizeof(int), 
	   					(unsigned char *) &similarity);
	assert(err == 0);

	return similarity;
}



