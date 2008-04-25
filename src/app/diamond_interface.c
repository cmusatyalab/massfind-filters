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

#include <glib.h>

#include "diamond_interface.h"
#include "massfind.h"
#include "define.h"
#include "drawutil.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <sys/param.h>
#include <string.h>

#include "lib_filter.h"
#include "lib_dconfig.h"

int total_objects;
int processed_objects;
int dropped_objects;

static ls_search_handle_t diamond_init(void) {
  ls_search_handle_t diamond_handle;

  diamond_handle = ls_init_search();
  return(diamond_handle);
}

static ls_search_handle_t generic_search (char *filter_spec_name) {
  static ls_search_handle_t diamond_handle = NULL;
  int f1, f2;
  int i;
  int j;
  int err;
  void *cookie;
  char *name;
  int num_devices;
  gid_list_t diamond_gid_list;
  struct collection_t collections[MAX_ALBUMS+1] = { { NULL } };
  ls_dev_handle_t dev_list[16];

  char buf[1];

  if (diamond_handle == NULL) {
  	  diamond_handle = diamond_init();
  }

  // scope
  printf("reading collections...\n");
  {
    int pos = 0;
    err = nlkup_first_entry(&name, &cookie);
    while(!err && pos < MAX_ALBUMS)
      {
	collections[pos].name = name;
	collections[pos].active = pos ? 0 : 1; /* first one active */
	pos++;
	err = nlkup_next_entry(&name, &cookie);
      }
    collections[pos].name = NULL;
  }

  diamond_gid_list.num_gids = 0;
  for (i=0; i<MAX_ALBUMS && collections[i].name; i++) {
    if (collections[i].active) {
      int err;
      int num_gids = MAX_ALBUMS;
      groupid_t gids[MAX_ALBUMS];
      err = nlkup_lookup_collection(collections[i].name, &num_gids, gids);
      g_assert(!err);
      for (j=0; j < num_gids; j++) {
		g_debug("adding gid: %lld\n", gids[j]);
		diamond_gid_list.gids[diamond_gid_list.num_gids++] = gids[j];
      }
    }
  }

  err = ls_set_searchlist(diamond_handle, 1, diamond_gid_list.gids);
  g_assert(!err);

  num_devices = 16;
  err = ls_get_dev_list(diamond_handle, dev_list, &num_devices);
  g_assert(!err);
  g_debug("Searching on %d devices", num_devices);


  // append our stuff
  f1 = g_open(MASSFIND_FILTERDIR "/rgb-filter.txt", O_RDONLY);
  if (f1 == -1) {
    perror("can't open " MASSFIND_FILTERDIR "/rgb-filter.txt");
    return NULL;
  }

  f2 = g_open(filter_spec_name, O_WRONLY | O_APPEND);
  if (f2 == -1) {
    printf("can't open %s", filter_spec_name);
    perror("");
    return NULL;
  }

  while (read(f1, buf, 1) > 0) {
    write(f2, buf, 1);   // PERF
  }

  close(f1);
  close(f2);

  err = ls_set_searchlet(diamond_handle, DEV_ISA_IA32,
			 DIAMOND_FILTERDIR "/fil_rgb.so",
			 filter_spec_name);
  g_assert(!err);

  return diamond_handle;
}


static void update_stats(ls_search_handle_t dr) {
  int num_dev;
  int i, err, len;
  dev_stats_t *dstats;
  int tobj = 0, sobj = 0, dobj = 0;
  GtkLabel *stats_label = GTK_LABEL(glade_xml_get_widget(g_xml, "statsLabel"));
  ls_dev_handle_t dev_list[16];

  guchar *tmp;

  dstats = (dev_stats_t *) g_malloc(DEV_STATS_SIZE(32));

  num_dev = 16;

  err = ls_get_dev_list(dr, dev_list, &num_dev);
  if (err != 0) {
    g_error("update stats: %d", err);
  }

  for (i = 0; i < num_dev; i++) {
    len = DEV_STATS_SIZE(32);
    err = ls_get_dev_stats(dr, dev_list[i], dstats, &len);
    if (err) {
      g_error("Failed to get dev stats for device %d", i);
    }
    tobj += dstats->ds_objs_total;
    sobj += dstats->ds_objs_processed;
    dobj += dstats->ds_objs_dropped;
  }
  g_free(dstats);

  total_objects = tobj;
  processed_objects = sobj;
  dropped_objects = dobj;
  tmp =
    g_strdup_printf("Total objects: %d, Processed objects: %d, Dropped objects: %d",
		    total_objects, processed_objects, dropped_objects);
  gtk_label_set_text(stats_label, tmp);
  g_free(tmp);
}


/*
 *  mass ROI names are coded to indicate benign or malignant
 * it would be nice to have an attribute that says this instead...
 */
gboolean is_malignant_result(char *name) {
	gboolean result = FALSE;
	if (strncmp(name, "TM", 2) == 0)
		result = TRUE;
	
	return result;
}


gboolean diamond_result_callback(gpointer g_data) {
  // this gets 0 or 1 result from diamond and puts it into the
  // icon view

  ls_obj_handle_t obj;
  void *data;
  unsigned int len;
  int err;
  int w, origW;
  int h, origH;
  GdkPixbuf *pix, *pix2, *pix3;

  static time_t last_time;
  int i;
  gchar *title;
  GtkTreeIter iter;
  GtkTreePath *path;
  GtkWidget *widget;
  double scale;
  int similarity;
  ls_search_handle_t dr;

  // get handle
  dr = (ls_search_handle_t) g_data;

  // get handle
  err = ls_next_object(dr, &obj, LSEARCH_NO_BLOCK);

  // XXX should be able to use select()
  if (err == EWOULDBLOCK) {
    // no results right now
    time_t now = time(NULL);
    if (now > last_time) {
 		update_stats(dr);
    }
    last_time = now;

    return TRUE;
  } else if (err == ENOENT) {
    // no more results
    GtkWidget *stopSearch = glade_xml_get_widget(g_xml, "stopSearch");
    GtkWidget *startSearch = glade_xml_get_widget(g_xml, "startSearch");
    gtk_widget_set_sensitive(stopSearch, FALSE);
    gtk_widget_set_sensitive(startSearch, TRUE);

    ls_terminate_search(dr);
    return FALSE;
  } else if (err) {
    g_warning("ls_next_object: %d", err);
    GtkWidget *stopSearch = glade_xml_get_widget(g_xml, "stopSearch");
    GtkWidget *startSearch = glade_xml_get_widget(g_xml, "startSearch");
    gtk_widget_set_sensitive(stopSearch, FALSE);
    gtk_widget_set_sensitive(startSearch, TRUE);

    ls_abort_search(dr);
    return FALSE;
  }

  // thumbnail
  err = lf_ref_attr(obj, "_cols.int", &len, (unsigned char **) &data);
  g_assert(!err);
  origW = w = *((int *) data);

  err = lf_ref_attr(obj, "_rows.int", &len, (unsigned char **) &data);
  g_assert(!err);
  origH = h = *((int *) data);

  err = lf_ref_attr(obj, "_rgb_image.rgbimage", &len, (unsigned char **) &data);
  g_assert(!err);
  pix = gdk_pixbuf_new_from_data(data, GDK_COLORSPACE_RGB,
				 TRUE, 8, w, h, w*4, NULL, NULL);
 
  err = lf_ref_attr(obj, "similarity", &len, (unsigned char **) &data);
  g_assert(!err);
  similarity = *((int *) data);
 
  // get the title - really a file name
  err = lf_ref_attr(obj, "name", &len, (unsigned char **) &data);     
  g_assert(!err);
  title = g_strdup_printf("%s (%d)", (char *) data, similarity);
  
  // create a thumbnail
  compute_thumbnail_scale(&scale, &w, &h);
  pix2 = gdk_pixbuf_scale_simple(pix, w, h, GDK_INTERP_BILINEAR);
  if (is_malignant_result((char *) data)) {
  	draw_thumbnail_border(pix2, w, h, 1.0, 0, 0);
  } else {
  	draw_thumbnail_border(pix2, w, h, 0, 0, 1.0);
  }
  
  // create a mid-sized version for search panel
  widget = glade_xml_get_widget(g_xml, "selectedResult");
  w = widget->allocation.width;
  h = widget->allocation.height;
  float aspect = (float) gdk_pixbuf_get_width(pix) /
      			(float) gdk_pixbuf_get_height(pix);
  compute_proportional_scale(&w, &h, aspect);
  pix3 = gdk_pixbuf_scale_simple(pix, w, h, GDK_INTERP_BILINEAR);
  
//  g_debug("got returned result %s, similarity %d", title, similarity);

  // store
  gtk_list_store_append(found_items, &iter);
  gtk_list_store_set(found_items, &iter,
		     0, pix2,
		     1, title,
		     2, pix,
		     3, pix3,
		     4, similarity,
		     -1);
  
  path = gtk_tree_model_get_path(GTK_TREE_MODEL(found_items), &iter);
  
  g_object_unref(pix);
  g_object_unref(pix2);
  g_object_unref(pix3);

  err = ls_release_object(dr, obj);
  g_assert(!err);

  return TRUE;
}


ls_search_handle_t diamond_similarity_search(search_desc_t *desc) {
	
  ls_search_handle_t dr;
  int fd;
  int i;
  FILE *f;
  gchar *name_used;
  int err;
  char filter_name[MAXPATHLEN];

  // temporary file for filter specification
  fd = g_file_open_tmp(NULL, &name_used, NULL);
  g_assert(fd >= 0);
  f = fdopen(fd, "a");
  g_return_val_if_fail(f, NULL);
  
  // do we need the visual filter?
  if (desc->vfeatures != NULL) {
	  fprintf(f, "\n\n"
		  "FILTER visual\n"
		  "THRESHOLD 1\n"
		  "EVAL_FUNCTION  f_eval_visual\n"
		  "INIT_FUNCTION  f_init_visual\n"
		  "FINI_FUNCTION  f_fini_visual\n");
	 
	  // write the features from the source image
	  for (i = 0; i < desc->numvf; i++) {
	  	fprintf(f, "ARG  %f  # feature %i\n", desc->vfeatures[i], i);
	  }
	  fprintf(f, "ARG  %f  # lower size range multiplier\n", 
	  			desc->sizeLower);
	  fprintf(f, "ARG  %f  # upper size range multiplier\n", 
	  			desc->sizeUpper);
	  fprintf(f, "ARG  %f  # lower circularity range multiplier\n", 
	  			desc->circLower);
	  fprintf(f, "ARG  %f  # upper circularity range multiplier\n", 
	  			desc->circUpper);
	  fprintf(f, "ARG  %f  # lower shape factor ratio multiplier\n", 
	  			desc->sfrLower);
	  fprintf(f, "ARG  %f  # upper shape factor ratio multiplier\n", 
	  			desc->sfrUpper);
  }
 
  strcpy(filter_name, DIAMOND_FILTERDIR);
  switch (desc->searchType) {
  	case EUCLIDIAN:
  	fprintf(f, "\n\n"
	  "FILTER euclidian\n"
	  "THRESHOLD %d\n"
	  "EVAL_FUNCTION  f_eval_euclidian\n"
	  "INIT_FUNCTION  f_init_euclidian\n"
	  "FINI_FUNCTION  f_fini_euclidian\n",
	  (int) desc->threshold);
	strcat(filter_name, "/libfil_euclidian.so");
	break;
	
  	case BOOSTLDM:
   	fprintf(f, "\n\n"
	  "FILTER boostldm\n"
	  "THRESHOLD %d\n"
	  "EVAL_FUNCTION  f_eval_boostldm\n"
	  "INIT_FUNCTION  f_init_boostldm\n"
	  "FINI_FUNCTION  f_fini_boostldm\n",
	  (int) desc->threshold);
 	strcat(filter_name, "/libfil_boostldm.so");
  	break;
  	
  	case QALDM:
   	fprintf(f, "\n\n"
	  "FILTER qaldm\n"
	  "THRESHOLD %d\n"
	  "EVAL_FUNCTION  f_eval_qaldm\n"
	  "INIT_FUNCTION  f_init_qaldm\n"
	  "FINI_FUNCTION  f_fini_qaldm\n",
	  (int) desc->threshold);
	strcat(filter_name, "/libfil_qaldm.so");
 	break;
  }
  
  // write the features from the source image
  for (i = 0; i < desc->numsf; i++) {
  	fprintf(f, "ARG  %f  # feature %i\n", desc->sfeatures[i], i);
  }
  
  // write dependencies
  fprintf(f, "REQUIRES  RGB # dependencies\n"
	  "MERIT  50 # some relative cost\n");
	  
  fflush(f);

  // initialize with generic search
  dr = generic_search(name_used);
 
  err = ls_add_filter_file(dr, DEV_ISA_IA32, filter_name);
  g_assert(!err);

  if (desc->vfeatures != NULL) {
	  strcpy(filter_name, DIAMOND_FILTERDIR);
	  strcat(filter_name, "/libfil_visual.so");
	  err = ls_add_filter_file(dr, DEV_ISA_IA32, filter_name);
	  g_assert(!err);
  }
  
  // now close
  fclose(f);
  g_free(name_used);

  // start search
  err = ls_start_search(dr);
  g_assert(!err);

  // return
  return dr;
}

