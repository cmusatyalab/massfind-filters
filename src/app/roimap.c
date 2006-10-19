
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

#include <gtk/gtk.h>

#include "roimap.h"

#define MAPFILENAME "Truthfile.txt"

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
      g_debug("image_name %s", next_token);
      // image name
      image_name = next_token;
      next_token = strtok(NULL, SEPARATORS);
    }
    if (next_token != NULL) {
      g_debug("center_x %s", next_token);
      if (isdigit(next_token[0])) {
	center_x = atoi(next_token);
      }
      // center y
      next_token = strtok(NULL, SEPARATORS);
    }
    if (next_token != NULL) {
      g_debug("center_y %s", next_token);
      if (isdigit(next_token[0])) {
	center_y = atoi(next_token);
	record_complete = TRUE;
      }
    }
    
    if (!record_complete)
      continue;

    // check study id for match
    if (strncmp(image_name, studyID, strlen(studyID)) == 0) {
      g_debug("found record for study %s: (x,y)=%d,%d", 
	      studyID, center_x, center_y);
      roi = (roi_t *) malloc(sizeof(roi_t));
      roi->full_image_name = NULL;
      roi->roi_image_name = NULL;
      roi->center_x = center_x;
      roi->center_y = center_y;
      roi->pixbuf = NULL;
      roi->attrs = NULL;
      break;
    }
  }

  fclose(fp);

  return(roi);
}


roi_t *get_roi(char *path) {

  // find a mass ROI by file name
  char imageName[MAXPATHLEN];
  char studyID[MAXNAMLEN];
  char mapFileName[MAXPATHLEN];
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
	    roi->attrs = g_hash_table_new();
		break;
      }
    }
    closedir(dp);
  }

  return(roi);
}

void free_roi(roi_t *r) {
    g_object_unref(r->pixbuf);
    free(r->roi_image_name);
    free(r->full_image_name);
    free(r);
}


