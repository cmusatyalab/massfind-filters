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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <stdlib.h>
#include <string.h>

#include "diamond_consts.h"

#include "massfind.h"
#include "define.h"
#include "roimap.h"
#include "drawutil.h"
#include "roi_features.h"
#include "upmc_features.h"
#include "diamond_interface.h"

extern GdkPixbuf *s_pix;
extern roi_t *roi;
extern gboolean show_masses;

GtkListStore *saved_search_store;
int distanceMetric = EUCLIDIAN;
double threshold;
GdkPixbuf *s_pix_full;
gfloat scale_full;


void draw_define_offscreen_items(gint allocation_width, gint allocation_height) {
	
  // clear old scaled pix
  if (s_pix_full != NULL) {
    g_object_unref(s_pix_full);
    s_pix_full = NULL;
  }

  // if something selected?
  if (s_pix) {
    float p_aspect =
      (float) gdk_pixbuf_get_width(s_pix) /
      (float) gdk_pixbuf_get_height(s_pix);
    int w = allocation_width;
    int h = allocation_height;
    float w_aspect = (float) w / (float) h;

    g_debug("w: %d, h: %d, p_aspect: %g, w_aspect: %g",
    	    w, h, p_aspect, w_aspect);

    /* is window wider than pixbuf? */
    if (p_aspect < w_aspect) {
      /* then calculate width from height */
      w = h * p_aspect;
      scale_full = (float) allocation_height
	/ (float) gdk_pixbuf_get_height(s_pix);
    } else {
      /* else calculate height from width */
      h = w / p_aspect;
      scale_full = (float) allocation_width
	/ (float) gdk_pixbuf_get_width(s_pix);
    }

    s_pix_full = gdk_pixbuf_scale_simple(s_pix,
					   w, h,
					   GDK_INTERP_BILINEAR);
  }
}


void on_saveSearchButton_clicked (GtkButton *button,
				  gpointer   user_data) {
  GtkTreeIter iter;
  search_desc_t *desc = NULL;
  int i;
  char prefix[MAX_ATTR_NAME];
  char nf[MAX_ATTR_NAME];
  char fstr[MAX_ATTR_NAME];
  
  desc = (search_desc_t *) malloc(sizeof(search_desc_t));
  desc->numsf = 0;
  desc->numvf = 0;
  desc->sfeatures = NULL;
  desc->vfeatures = NULL;
  desc->name = gtk_entry_get_text(
  	GTK_ENTRY(glade_xml_get_widget(g_xml, "searchName")));
  g_debug("making new search: %s", desc->name);
  
  // pick out the relevant ROI features, depending on the search
  desc->searchType = distanceMetric;
  if (distanceMetric == EUCLIDIAN || 
  	  distanceMetric == QALDM) {  // both of these euclidian for now
  	strcpy(prefix, EDMF_PREFIX);
  	strcpy(nf, NUM_EDMF);
  } else {  // boost LDM
  	strcpy(prefix, BDMF_PREFIX);
  	strcpy(nf, NUM_BDMF);
  }
  
  // get the similarity search threshold
  desc->threshold = gtk_range_get_value(
  	GTK_RANGE(glade_xml_get_widget(g_xml, "distanceThreshold")));

  // check for visual feature constraints on search
  gboolean sizeButtonPressed = gtk_toggle_button_get_active(
  	GTK_TOGGLE_BUTTON(glade_xml_get_widget(g_xml, "sizeButton")));
  desc->sizeUpper = RANGE_VALUE_UNDEFINED;
  desc->sizeLower = RANGE_VALUE_UNDEFINED;
  if (sizeButtonPressed) {
	desc->sizeUpper = gtk_range_get_value(
		GTK_RANGE(glade_xml_get_widget(g_xml, "sizeRangeUpper")));
	desc->sizeLower = gtk_range_get_value(
		GTK_RANGE(glade_xml_get_widget(g_xml, "sizeRangeLower")));
  }

  gboolean circButtonPressed = gtk_toggle_button_get_active(
  	GTK_TOGGLE_BUTTON(glade_xml_get_widget(g_xml, "circularityButton")));
  desc->circUpper = RANGE_VALUE_UNDEFINED;
  desc->circLower = RANGE_VALUE_UNDEFINED;
  if (circButtonPressed) {
  	desc->circUpper = gtk_range_get_value(
  		GTK_RANGE(glade_xml_get_widget(g_xml, "circularityRangeUpper")));
  	desc->circLower = gtk_range_get_value(
  		GTK_RANGE(glade_xml_get_widget(g_xml, "circularityRangeLower")));
  }

  if (sizeButtonPressed || circButtonPressed) {
	  // save features for visual similarity search (non-normalized)
	  desc->numvf = atoi(g_hash_table_lookup(roi->attrs, NUM_UPMC));
	  g_debug("adding %d features for %s", desc->numvf, UPMC_PREFIX);
	  desc->vfeatures = (float *) malloc(desc->numvf*sizeof(float));
	  for (i = 0; i < desc->numvf; i++) {
	  	sprintf(fstr, "%s%02d", UPMC_PREFIX, i);
	  	desc->vfeatures[i] = atof(g_hash_table_lookup(roi->attrs, fstr));
	  }
  }
  
  // save features for algorithmic similarity search
  desc->numsf = atoi(g_hash_table_lookup(roi->attrs, nf));
  g_debug("adding %d features for %s", desc->numsf, prefix);
  desc->sfeatures = (float *) malloc(desc->numsf*sizeof(float));
  for (i = 0; i < desc->numsf; i++) {
  	sprintf(fstr, "%s%02d", prefix, i);
  	desc->sfeatures[i] = atof(g_hash_table_lookup(roi->attrs, fstr));
  }
    
  gtk_list_store_append(saved_search_store, &iter);
  gtk_list_store_set(saved_search_store, &iter, 0, desc->name, 1, desc, -1);
  return;
}

void on_euclidianbutton_toggled(GtkToggleButton *togglebutton,
                                gpointer         user_data) {

  if (gtk_toggle_button_get_active(togglebutton)) {
    distanceMetric = EUCLIDIAN;
    g_debug("using Euclidian distance");
  }
  return;
}
                                
void on_boostedldmbutton_toggled(GtkToggleButton *togglebutton,
                                 gpointer         user_data) {

  if (gtk_toggle_button_get_active(togglebutton)) {
    distanceMetric = BOOSTLDM;
    g_debug("using boosted LDM");
  }
  return;
}
                                 
void on_qaldmbutton_toggled(GtkToggleButton *togglebutton,
                            gpointer         user_data) {
                            	
  if (gtk_toggle_button_get_active(togglebutton)) {
    distanceMetric = QALDM;
    g_debug("using query-adaptive LDM");
  }
  return;
}

gboolean on_selectionFullSize_configure_event (GtkWidget *widget,
					   GdkEventConfigure *event,
					   gpointer          user_data) {
  draw_define_offscreen_items(event->width, event->height);
  return TRUE;
}

gboolean on_selectionFullSize_expose_event (GtkWidget *d,
				    GdkEventExpose *event,
				    gpointer user_data) {
    g_debug("selected full size image expose event");
	if (s_pix) {
     gdk_draw_pixbuf(d->window,
		    d->style->fg_gc[GTK_WIDGET_STATE(d)],
		    s_pix_full,
		    0, 0, 0, 0,
		    -1, -1,
		    GDK_RGB_DITHER_NORMAL,
		    0, 0);
	  if (roi && roi->pixbuf && show_masses) {
	  	int i;
		char *xstr, *ystr, *delim;
	  	char fstr[MAX_ATTR_NAME];
	  	
	    // get the contour points
		char *value = g_hash_table_lookup(roi->attrs, NUM_CONTOUR);
		int npoints = atoi(value);
		char *xpts = g_hash_table_lookup(roi->attrs, CONTOUR_X);
		char *ypts = g_hash_table_lookup(roi->attrs, CONTOUR_Y);
		double *xvals = (double *)malloc(npoints*sizeof(double));
		double *yvals = (double *)malloc(npoints*sizeof(double));	
		xstr = xpts;
		ystr = ypts;
		for (i = 0; i < npoints; i++) {	
			delim = index(xstr, CONTOUR_DELIM);
			strncpy(fstr, xstr, delim-xstr);
			xstr = delim+1;
			xvals[i] = atof(fstr);
			
			delim = index(ystr, CONTOUR_DELIM);
			strncpy(fstr, ystr, delim-ystr);
			ystr = delim+1;
			yvals[i] = atof(fstr);
		}
		draw_roi_contour(d, npoints, xvals, yvals, scale_full);
		free(xvals);
		free(yvals);

		sprintf(fstr, "%s%02d", UPMC_PREFIX, UPMC_CENTER_X);
		value = g_hash_table_lookup(roi->attrs, fstr);
		int center_x = atoi(value)*4;
		sprintf(fstr, "%s%02d", UPMC_PREFIX, UPMC_CENTER_Y);
		value = g_hash_table_lookup(roi->attrs, fstr);
		int center_y = atoi(value)*4;    

  		// draw mass roi border on (scaled) full image
		double frame_x = center_x - (gdk_pixbuf_get_width(roi->pixbuf)/2);
		double frame_y = center_y - (gdk_pixbuf_get_height(roi->pixbuf)/2);
		draw_roi_border(d, frame_x, frame_y, 
						gdk_pixbuf_get_width(roi->pixbuf), 
						gdk_pixbuf_get_height(roi->pixbuf),
						scale_full);
  	}    
 }
 return;
}


