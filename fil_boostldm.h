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
 
#ifndef FIL_BOOSTLDM_H_
#define FIL_BOOSTLDM_H_


#include "lib_filter.h"

typedef struct boostldm_config
{
  int  numFeatures;
  float   *features;  /* features computed on object */
} boostldm_config_t;

#endif /*FIL_BOOSTLDM_H_*/
