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

#include "massfind.h"
#include "define.h"
#include "roimap.h"
#include "drawutil.h"

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
// center scrolled window on mass ROI?

void on_saveSearchButton_clicked (GtkButton *button,
				  gpointer   user_data) {
  GtkTreeIter iter;
  float *features = NULL;
  int num_features;
  int i;
  const char uprefix[] = "upmc";
  const char bprefix[] = "bdmf";
  char *prefix;
  char nf[8] = "num";
  char fstr[7];
  
  const gchar *save_name =
    gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(g_xml, "searchName")));
  gdouble t = gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(g_xml, "distanceThreshold")));
 
  g_debug("making new search: %s", save_name);
  
  // pick out the relevant ROI features, depending on the search
  if (distanceMetric == EUCLIDIAN || 
  	  distanceMetric == QALDM) {  // both of these euclidian for now
  	prefix = uprefix;
  } else {  // boost LDM
  	prefix = bprefix;
  }
  
  num_features = atoi(g_hash_table_lookup(roi->attrs, strcat(nf, prefix)));
  g_debug("adding %d features for %s", num_features, prefix);
  features = malloc(num_features*sizeof(float));
  for (i = 0; i < num_features; i++) {
  	sprintf(fstr, "%s%02d", prefix, i);
  	features[i] = atof(g_hash_table_lookup(roi->attrs, fstr));
  }
    
  gtk_list_store_append(saved_search_store, &iter);
  gtk_list_store_set(saved_search_store, &iter,
		     0, save_name,
		     1, distanceMetric,
		     2, t,
		     3, num_features,
		     4, features,
		     -1);
}

void on_euclidianbutton_toggled(GtkToggleButton *togglebutton,
                                gpointer         user_data) {

  if (gtk_toggle_button_get_active(togglebutton)) {
    distanceMetric = EUCLIDIAN;
    g_debug("using Euclidian distance");
  }
}
                                
void on_boostedldmbutton_toggled(GtkToggleButton *togglebutton,
                                 gpointer         user_data) {

  if (gtk_toggle_button_get_active(togglebutton)) {
    distanceMetric = BOOSTLDM;
    g_debug("using boosted LDM");
  }
}
                                 
void on_qaldmbutton_toggled(GtkToggleButton *togglebutton,
                            gpointer         user_data) {
                            	
  if (gtk_toggle_button_get_active(togglebutton)) {
    distanceMetric = QALDM;
    g_debug("using query-adaptive LDM");
  }
}

void on_define_search_value_changed (GtkRange *range,
				     gpointer  user_data) {
	
  threshold = gtk_range_get_value(range);
  g_debug("threshold value changed to %f", threshold);
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
  		// draw mass roi seed and border on (scaled) full image
		draw_roi_center(d, roi->center_x, roi->center_y, scale_full);
		double frame_x = roi->center_x - (gdk_pixbuf_get_width(roi->pixbuf)/2);
		double frame_y = roi->center_y - (gdk_pixbuf_get_height(roi->pixbuf)/2);
		draw_roi_border(d, frame_x, frame_y, 
						gdk_pixbuf_get_width(roi->pixbuf), 
						gdk_pixbuf_get_height(roi->pixbuf),
						scale_full);
  	}    
 }
}


