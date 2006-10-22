/*
 * MASSFIND: A Diamond application for adipocyte image exploration
 *
 * Copyright (c) 2006 Carnegie Mellon University. All rights reserved.
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
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <sys/param.h>
#include <string.h>

#include <cairo.h>

#include "lib_filter.h"
#include "lib_dconfig.h"

static struct collection_t collections[MAX_ALBUMS+1] = { { NULL } };
static gid_list_t diamond_gid_list;

int total_objects;
int processed_objects;
int dropped_objects;

void compute_thumbnail_scale(double *scale, gint *w, gint *h) {
  float p_aspect = (float) *w / (float) *h;

  if (p_aspect < 1) {
    /* then calculate width from height */
    *scale = 150.0 / (double) *h;
    *h = 150;
    *w = *h * p_aspect;
  } else {
    /* else calculate height from width */
    *scale = 150.0 / (double) *w;
    *w = 150;
    *h = *w / p_aspect;
  }
}

void convert_cairo_argb32_to_pixbuf(guchar *pixels,
				    gint w, gint h, gint stride) {
  gint x, y;

  // swap around the data
  // XXX also handle pre-multiplying?
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      // XXX check endian
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
      guchar *p = pixels + (stride * y) + (4 * x);

      guchar r = p[2];
      guchar b = p[0];

      p[0] = r;
      p[2] = b;
#else
      guint *p = (guint *) (pixels + (stride * y) + (4 * x));
      *p = GUINT32_SWAP_LE_BE(*p);
#endif
    }
  }
}

void draw_thumbnail_border(GdkPixbuf *pix, gint w, gint h, 
							float r, float g, float b) {
	guchar *pixels = gdk_pixbuf_get_pixels(pix);
  	int stride = gdk_pixbuf_get_rowstride(pix);
  	cairo_surface_t *surface = cairo_image_surface_create_for_data(pixels,
								 CAIRO_FORMAT_ARGB32,
								 w, h, stride);
 	cairo_t *cr = cairo_create(surface);
 	cairo_rectangle (cr, 0, 0, w, h);
 	cairo_set_line_width(cr, 10.0);
 	cairo_set_source_rgb (cr, r, g, b); 
 	cairo_stroke (cr);

  	cairo_destroy(cr);
  	cairo_surface_destroy(surface);

  	convert_cairo_argb32_to_pixbuf(pixels, w, h, stride);
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


static void diamond_init(void) {
  int i;
  int j;
  int err;
  void *cookie;
  char *name;

  if (diamond_gid_list.num_gids != 0) {
    // already initialized
    return;
  }

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
}

static ls_search_handle_t generic_search (char *filter_spec_name) {
  ls_search_handle_t diamond_handle;
  int f1, f2;
  int err;

  char buf[1];

  diamond_init();

  diamond_handle = ls_init_search();

  err = ls_set_searchlist(diamond_handle, 1, diamond_gid_list.gids);
  g_assert(!err);

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
			 DIAMOND_FILTERDIR "/fil_rgb.a",
			 filter_spec_name);
  g_assert(!err);


  return diamond_handle;
}


static void update_stats(ls_search_handle_t dr) {
  int num_dev;
  ls_dev_handle_t dev_list[16];
  int i, err, len;
  dev_stats_t *dstats;
  int tobj = 0, sobj = 0, dobj = 0;
  GtkLabel *stats_label = GTK_LABEL(glade_xml_get_widget(g_xml, "statsLabel"));

  guchar *tmp;

  dstats = (dev_stats_t *) g_malloc(DEV_STATS_SIZE(32));

  num_dev = 16;

  err = ls_get_dev_list(dr, dev_list, &num_dev);
  if (err != 0) {
    g_error("update states: %d", err);
  }

  for (i = 0; i < num_dev; i++) {
    len = DEV_STATS_SIZE(32);
    g_debug("getting dev %d", i);

    err = ls_get_dev_stats(dr, dev_list[i], dstats, &len);
    if (err) {
      g_error("Failed to get dev stats");
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


gboolean diamond_result_callback(gpointer g_data) {
  // this gets 0 or 1 result from diamond and puts it into the
  // icon view

  ls_obj_handle_t obj;
  void *data;
  unsigned int len;
  int err;
  int w, origW;
  int h, origH;
  GdkPixbuf *pix, *pix2;

  static time_t last_time;
  int i;
  gchar *title;
  GtkTreeIter iter;
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
 //     update_stats(dr);
    }
    last_time = now;

    return TRUE;
  } else if (err) {
    // no more results
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
  
  g_debug(" got object %s", title);

  // store
  gtk_list_store_append(found_items, &iter);
  gtk_list_store_set(found_items, &iter,
		     0, pix2,
		     1, title,
		     2, pix,
		     3, similarity,
		     -1);

  g_object_unref(pix);
  g_object_unref(pix2);

  err = ls_release_object(dr, obj);
  g_assert(!err);

  return TRUE;
}


ls_search_handle_t diamond_similarity_search(int searchType, 
											 double threshold,
											 int numFeatures, 
											 float *features) {
  ls_search_handle_t dr;
  int fd;
  int i;
  FILE *f;
  gchar *name_used;
  int err;
  char filter_name[MAXPATHLEN];

  // temporary file
  fd = g_file_open_tmp(NULL, &name_used, NULL);
  g_assert(fd >= 0);

  // write out file for similarity search
  f = fdopen(fd, "a");
  g_return_val_if_fail(f, NULL);
  strcpy(filter_name, DIAMOND_FILTERDIR);
  
  switch (searchType) {
  	case EUCLIDIAN:
  	fprintf(f, "\n\n"
	  "FILTER euclidian\n"
	  "THRESHOLD %d\n"
	  "EVAL_FUNCTION  f_eval_euclidian\n"
	  "INIT_FUNCTION  f_init_euclidian\n"
	  "FINI_FUNCTION  f_fini_euclidian\n",
	  (int) threshold);
	strcat(filter_name, "/libfil_euclidian.a");
	break;
	
  	case BOOSTLDM:
   	fprintf(f, "\n\n"
	  "FILTER boostldm\n"
	  "THRESHOLD %d\n"
	  "EVAL_FUNCTION  f_eval_boostldm\n"
	  "INIT_FUNCTION  f_init_boostldm\n"
	  "FINI_FUNCTION  f_fini_boostldm\n",
	  (int) threshold);
 	strcat(filter_name, "/libfil_boostldm.a");
  	break;
  	
  	case QALDM:
   	fprintf(f, "\n\n"
	  "FILTER qaldm\n"
	  "THRESHOLD %d\n"
	  "EVAL_FUNCTION  f_eval_qaldm\n"
	  "INIT_FUNCTION  f_init_qaldm\n"
	  "FINI_FUNCTION  f_fini_qaldm\n",
	  (int) threshold);
	strcat(filter_name, "/libfil_qaldm.a");
 	break;
  }
  
  // write the features from the source image
  for (i = 0; i < numFeatures; i++) {
  	fprintf(f, "ARG  %f  # feature %i\n", features[i], i);
  }
  
  fprintf(f, "REQUIRES  RGB # dependencies\n"
	  "MERIT  50 # some relative cost\n");
	  
  fflush(f);

  // initialize with generic search
  dr = generic_search(name_used);
 
  err = ls_add_filter_file(dr, DEV_ISA_IA32, filter_name);
  g_assert(!err);

  // now close
  fclose(f);
  g_free(name_used);

  // start search
  ls_start_search(dr);

  // return
  return dr;
}

