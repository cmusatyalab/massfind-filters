
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/param.h>

#include "roimap.h"

#define MAPFILENAME "Truthfile.txt"
#define MAXLINELEN 256
#define MAXATTRLEN 32
#define SEPARATORS " \t"
#define ATTR_SEPS "\"=\n"

/*
 *  get a region of interest record
 *  studyID - full size image name without suffix
 */
roi_t *get_roi_record(char *mapFileName, char *studyID) {

  char line[MAXLINELEN];
  char *next_token = NULL;
  char *image_name;
  int center_x;
  int center_y;
  int birad;
  int density;
  int subtlety;
  int age;
  int biopsy;
  int shape;
  int margin;
  
  roi_t *roi = NULL;

  // look for file names with the study ID
  FILE *fp = fopen(mapFileName, "r");
  while (fgets(line, MAXLINELEN, fp) != NULL) {
    int record_complete = FALSE;

    next_token = strtok(line, SEPARATORS);
    if (next_token != NULL) {
      // is this a case number?
      if (isdigit(next_token[0])) {
		// skip past the case number 
		next_token = strtok(NULL, SEPARATORS);
      }
    }
    if (next_token != NULL) {
 //     g_debug("image_name %s", next_token);
      // image name
      image_name = next_token;
      next_token = strtok(NULL, SEPARATORS);
    }
    if (next_token != NULL) {
//      g_debug("center_x %s", next_token);
      if (isdigit(next_token[0])) {
		center_x = atoi(next_token);
      }
      // center y
      next_token = strtok(NULL, SEPARATORS);
    }
    if (next_token != NULL) {
//      g_debug("center_y %s", next_token);
      if (isdigit(next_token[0])) {
		center_y = atoi(next_token);
 	    next_token = strtok(NULL, SEPARATORS);
     }
    }
    if (next_token != NULL) {
 //   	g_debug("birad %s", next_token);
 	    if (isdigit(next_token[0])) {
 	    	birad = atoi(next_token);
 		    next_token = strtok(NULL, SEPARATORS);
     	}
    }
    if (next_token != NULL) {
 //   	g_debug("subtlety %s", next_token);
 	    if (isdigit(next_token[0])) {
 	    	subtlety = atoi(next_token);
 		    next_token = strtok(NULL, SEPARATORS);
     	}
    }
    if (next_token != NULL) {
 //   	g_debug("density %s", next_token);
 	    if (isdigit(next_token[0])) {
 	    	density = atoi(next_token);
 		    next_token = strtok(NULL, SEPARATORS);
     	}
    }
    if (next_token != NULL) {
 //   	g_debug("age %s", next_token);
 	    if (isdigit(next_token[0])) {
 	    	age = atoi(next_token);
 		    next_token = strtok(NULL, SEPARATORS);
     	}
    }
    if (next_token != NULL) {
 //   	g_debug("biopsy %s", next_token);
 	    if (strcmp(next_token, "MALIGNANT") == 0) {
 	    	biopsy = MALIGNANT;
 	    } else {
 	    	biopsy = BENIGN;
 	    }
 		next_token = strtok(NULL, SEPARATORS);
    }
    if (next_token != NULL) {
//    	g_debug("shape %s", next_token);    	
 	    if (strcmp(next_token, "ROUND") == 0) {
 	    	shape = ROUND;
 	    } else if (strcmp(next_token, "OVAL") == 0) {
 	    	shape = OVAL;
 	    } else if (strcmp(next_token, "LOBULATED") == 0) {
 	    	shape = LOBULATED;
 	    } else {
 	    	shape = IRREGULAR;
 	    }
 		next_token = strtok(NULL, SEPARATORS);
    }
    if (next_token != NULL) {
 //   	g_debug("margin %s", next_token);    	
 	    if (strcmp(next_token, "SPICULATED") == 0) {
 	    	margin = SPICULATED;
 	    } else if (strcmp(next_token, "ILLDEINFED") == 0) {
 	    	margin = ILLDEFINED;
 	    } else if (strcmp(next_token, "MICROLOBULATED") == 0) {
 	    	margin = MICROLOBULATED;
 	    } else if (strcmp(next_token, "CIRCUMSCRIBED") == 0){
 	    	margin = CIRCUMSCRIBED;
 	    } else {
 	    	margin = OBSCURED;
 	    }
 		record_complete = TRUE;
    }

   if (!record_complete)
      continue;

    // check study id for match
    if (strncmp(image_name, studyID, strlen(studyID)) == 0) {
 //     g_debug("found record for study %s: (x,y)=%d,%d", 
//	      studyID, center_x, center_y);
      roi = (roi_t *) malloc(sizeof(roi_t));
      roi->case_name = malloc(strlen(studyID)+1);
      strcpy(roi->case_name, studyID);
      roi->full_image_name = NULL;
      roi->roi_image_name = NULL;
      roi->center_x = center_x;
      roi->center_y = center_y;
      roi->subtlety = subtlety;
      roi->birad = birad;
      roi->density = density;
      roi->age = age;
      roi->biopsy = biopsy;
      roi->shape = shape;
      roi->margin = margin;
      roi->pixbuf = NULL;
      roi->attrs = NULL;
      break;
    }
  }

  fclose(fp);

  return(roi);
}

void get_roi_attrs(char *attrFileName, GHashTable *ht) {

  char line[MAXLINELEN];
  char *attr = NULL;
  char *value = NULL;
  char *k, *v;

  FILE *fp = fopen(attrFileName, "r");
  while (fgets(line, MAXLINELEN, fp) != NULL) {
    attr = strtok(line, ATTR_SEPS);
    value = strtok(NULL, ATTR_SEPS);
    if (attr != NULL && value != NULL) {
    	k = malloc(MAXATTRLEN);
    	v = malloc(MAXATTRLEN);
    	strncpy(k, attr, MAXATTRLEN);
    	strncpy(v, value, MAXATTRLEN);
 //   	g_debug("attribute %s = %s", k, v);
    	g_hash_table_insert(ht, k, v);
    }
  }

  fclose(fp);
  return;
}


roi_t *get_roi(char *path) {

  // find a mass ROI by file name
  char imageName[MAXPATHLEN];
  char studyID[MAXNAMLEN];
  char mapFileName[MAXPATHLEN];
  char attrFileName[MAXPATHLEN];
  roi_t *roi = NULL;
  GError *err = NULL;
  
  strcpy(imageName, path);
  char *lastcomp = strrchr(imageName,'/');
  if (lastcomp != NULL)
    lastcomp[1] = '\0';
  else
    imageName[0] = '\0';
  
  strcpy(mapFileName, imageName);
  strcat(mapFileName, MAPFILENAME);

  lastcomp = strrchr(path, '/');
  if (lastcomp != NULL) 
    strcpy(studyID, lastcomp+1);
  else 
    strcpy(studyID, path);

  char *suffix = strchr(studyID, '.');
  if (suffix != NULL)
    suffix[0] = '\0';
  
  if (roi = get_roi_record(mapFileName, studyID)) {

    // look for file names with the study ID
    DIR *dp = opendir(imageName);
    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
      if (strstr(de->d_name, studyID) &&
	 	  strncmp(de->d_name, studyID, strlen(studyID))) {
		strcat(imageName, de->d_name);

		roi->roi_image_name = (char *) malloc(strlen(imageName)+1);
      	strcpy(roi->roi_image_name, imageName);
		roi->full_image_name = (char *) malloc(strlen(path)+1);
		strcpy(roi->full_image_name, path);
	
	    g_debug("loading mass ROI from %s", roi->roi_image_name);
	    roi->pixbuf = gdk_pixbuf_new_from_file(roi->roi_image_name, &err);
	    if (err != NULL) {
 	     g_critical("error: %s", err->message);
 	     g_error_free(err);
	    }	
	    
	    // load features from attr file
	    strcpy(attrFileName, roi->roi_image_name);
   		strcat(attrFileName, ".text_attr");	    
	    g_debug("loading ROI attributes from %s", attrFileName);
	    roi->attrs = g_hash_table_new(g_str_hash, g_str_equal);
	    get_roi_attrs(attrFileName, roi->attrs);
	    if (g_hash_table_size(roi->attrs) == 0) {
	    	g_critical("no attributes found for %s", roi->roi_image_name);
	    }
	    
	    g_debug("loaded %d ROI attributes from %s", 
	    		g_hash_table_size(roi->attrs),
	    		attrFileName);
		break;
      }
    }
    closedir(dp);
  }

  return(roi);
}


gboolean remove_roi_attrs(gpointer key, gpointer value, gpointer user_data) {
	free((char *) key);
	free((char *) value);
	return TRUE;
}


void free_roi(roi_t *r) {
	free(r->case_name);
    free(r->roi_image_name);
    free(r->full_image_name);
    g_object_unref(r->pixbuf);
    g_hash_table_foreach_remove(r->attrs, remove_roi_attrs, NULL);
    g_hash_table_destroy(r->attrs);
    free(r);
}


