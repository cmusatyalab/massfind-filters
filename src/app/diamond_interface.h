/*
 * MassFind: A Diamond application for exploration of breast tumors
 *
 * Copyright (c) 2006-2008 Carnegie Mellon University. All rights reserved.
 * Copyright (c) 2006-2007 Intel Corporation. All rights reserved.
 * Additional copyrights may be listed below.
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0 which accompanies this
 * distribution in the file named LICENSE.
 *
 * Technical and financial contributors are listed in the file named
 * CREDITS.
 */

#ifndef DIAMOND_INTERFACE_H
#define DIAMOND_INTERFACE_H

#include "diamond_consts.h"
#include "diamond_types.h"
#include "lib_searchlet.h"

#include <stdio.h>
#include <gtk/gtk.h>

#define MAX_ALBUMS 32

typedef struct {
  int num_gids;
  groupid_t gids[MAX_ALBUMS];
} gid_list_t;

struct collection_t
{
  char *name;
  //int id;
  int active;
};

typedef struct 
{
	gchar *name;
	int searchType;
	gdouble threshold;
	gdouble sizeUpper;
	gdouble sizeLower;
	gdouble circUpper;
	gdouble circLower;
	gdouble sfrUpper;
	gdouble sfrLower;
 	int numvf;
 	float *vfeatures;
 	int numsf;
   	float *sfeatures;
} search_desc_t;

extern int total_objects;
extern int processed_objects;
extern int dropped_objects;

#ifdef __cplusplus
extern "C" {
#endif

  ls_search_handle_t diamond_similarity_search(search_desc_t *desc);

  gboolean diamond_result_callback(gpointer data);

#ifdef __cplusplus
}
#endif


#endif
