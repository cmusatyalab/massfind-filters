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


#ifndef FIL_VISUAL_H_
#define FIL_VISUAL_H_


typedef struct visual_config
{
  int  numFeatures;
  float   *features;  /* features computed on object */
  float size_mult_lower;
  float size_mult_upper;
  float circ_mult_lower;
  float circ_mult_upper;
  float sfr_mult_lower;
  float sfr_mult_upper;
} visual_config_t;

int f_init_visual(int numarg, char **args, int blob_len,
                    void *blob, const char *fname, void **data);
int f_eval_visual(lf_obj_handle_t ohandle, void *f_data);
int f_fini_visual(void *data);

#endif /*FIL_VISUAL_H_*/
