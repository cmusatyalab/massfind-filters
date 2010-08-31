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
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "lib_filter.h"
#include "roi_features.h"
#include "fil_qaldm.h"

#define MAX_ATTR_NAME           128
#define MAX_ATTR_VALUE          4096

int f_init_qaldm(int numarg, char **args, int blob_len,
                    void *blob, const char *fname, void **data)
{
	qaldm_config_t *fconfig;
	int i;

	/*
	 * save the features for the source object
	 */
	fconfig = (qaldm_config_t *)malloc(sizeof(*fconfig));
	assert(fconfig);

	fconfig->numFeatures = numarg-1;
	fconfig->features = (float *) malloc(sizeof(float) * fconfig->numFeatures);
    for (i = 0; i < fconfig->numFeatures; i++) {
    	fconfig->features[i] = atof(args[i+1]);
     }

	/*
	 * save the data pointer 
	 */
	*data = (void *) fconfig;
	
	return (0);
}

int f_fini_qaldm(void *data)
{
	qaldm_config_t *fconfig = (qaldm_config_t *) data;
	free(fconfig->features);
	free(fconfig);
	
	return (0);
}


int f_eval_qaldm(lf_obj_handle_t ohandle, void *f_data)
{
	int err;
	int i;
	qaldm_config_t *fconfig = (qaldm_config_t *) f_data;
	size_t featureLen = MAX_ATTR_VALUE;
	unsigned char featureStr[MAX_ATTR_VALUE];
	int numFeatures;
	float f;
	float distance = 0;
	
	lf_log(LOGL_TRACE, "f_eval_qaldm: enter");
	
	// extract the features for this object
	err = lf_read_attr(ohandle, "NUMFEATURES", &featureLen, featureStr);
	assert(err == 0);
	numFeatures = atoi((char *)featureStr);
	assert(numFeatures == fconfig->numFeatures);

	for(i=0; i<numFeatures; i++) {
		err = lf_read_attr(ohandle, "NUMFEATURES", &featureLen, featureStr);
		assert(err == 0);
		f = atof((char *)featureStr);
		distance=distance+pow((f-fconfig->features[i]),2);
	}

	return (int) distance;
}
