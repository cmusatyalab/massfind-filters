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

#include "massfind.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <glade/glade.h>
#include <stdlib.h>

#include <math.h>
#include <sys/param.h>
#include <string.h>

#include "diamond_consts.h"

#include "select.h"
#include "define.h"
#include "roimap.h" 
#include "drawutil.h"
#include "roi_features.h"
#include "upmc_features.h"

extern gboolean show_masses;

GdkPixbuf *s_pix = NULL;
GdkPixbuf *s_pix_scaled = NULL;
GdkPixbuf *s_pix_full = NULL;
gfloat scale;
gfloat scale_full;
roi_t *roi = NULL;

void write_case_data(roi_t *roi) {
	
   char textLabel[80];
   GtkWidget *w;
   
   w = glade_xml_get_widget(g_xml, "caseId");
   strcpy(textLabel, roi->case_name);
   gtk_label_set_text(GTK_LABEL(w), textLabel);
   
   w = glade_xml_get_widget(g_xml, "caseAge");
   sprintf(textLabel, "%d", roi->age);
   gtk_label_set_text(GTK_LABEL(w), textLabel);
   
   w = glade_xml_get_widget(g_xml, "caseShape");
   switch (roi->shape) {
   		case ROUND:
   			strcpy(textLabel, "round");
   		  	break;
   		case OVAL:
   			strcpy(textLabel, "oval");
   			break;
   		case LOBULATED:
   			strcpy(textLabel, "lobulated");
   			break;
   		case IRREGULAR:
   		   	strcpy(textLabel, "irregular");
   			break;
   }
   gtk_label_set_text(GTK_LABEL(w), textLabel);
   
   w = glade_xml_get_widget(g_xml, "caseMargin");
   switch (roi->shape) {
   		case SPICULATED:
   		    strcpy(textLabel, "spiculated");
   		  	break;
   		case ILLDEFINED:
   		   	strcpy(textLabel, "ill-defined");
   			break;
   		case MICROLOBULATED:
   		    strcpy(textLabel, "microlobulated");
   			break;
   		case CIRCUMSCRIBED:
   		    strcpy(textLabel, "circumscribed");
   			break;
   		case OBSCURED:
   		    strcpy(textLabel, "obscured");
   			break;
   }
   gtk_label_set_text(GTK_LABEL(w), textLabel);
   
   w = glade_xml_get_widget(g_xml, "caseBirad");
   sprintf(textLabel, "%d    1 (benign) - 5 (malignant)", roi->birad);
   gtk_label_set_text(GTK_LABEL(w), textLabel);

   w = glade_xml_get_widget(g_xml, "caseDensity");
   sprintf(textLabel, "%d    1 (fat) - 4  (dense)", roi->density);
   gtk_label_set_text(GTK_LABEL(w), textLabel);
   
   w = glade_xml_get_widget(g_xml, "caseSubtlety");
   sprintf(textLabel, "%d    1 (difficult) - 5 (easy)", roi->subtlety);
   gtk_label_set_text(GTK_LABEL(w), textLabel);
}


void clear_case_data() {
   GtkWidget *w;
   
   w = glade_xml_get_widget(g_xml, "caseId");
   gtk_label_set_text(GTK_LABEL(w), "");
   
   w = glade_xml_get_widget(g_xml, "caseAge");
   gtk_label_set_text(GTK_LABEL(w), "");
  
   w = glade_xml_get_widget(g_xml, "caseShape");
   gtk_label_set_text(GTK_LABEL(w), "");
   
   w = glade_xml_get_widget(g_xml, "caseMargin");
   gtk_label_set_text(GTK_LABEL(w), "");
   
   w = glade_xml_get_widget(g_xml, "caseBirad");
   gtk_label_set_text(GTK_LABEL(w), "");

   w = glade_xml_get_widget(g_xml, "caseDensity");
   gtk_label_set_text(GTK_LABEL(w), "");
   
   w = glade_xml_get_widget(g_xml, "caseSubtlety");
   gtk_label_set_text(GTK_LABEL(w), "");
}


static void foreach_selection(GtkIconView *icon_view,
								GtkTreePath *path,
					      		gpointer data) {
  gchar *pix;

  GError *err = NULL;

  GtkTreeIter iter;
  GtkTreeModel *m = gtk_icon_view_get_model(icon_view);
  GtkWidget *w;

  FILE *f;

  // get the file name
  gtk_tree_model_get_iter(m, &iter, path);
  gtk_tree_model_get(m, &iter, 1, &pix,-1);

  if (s_pix != NULL) {
    g_object_unref(s_pix);
  }

  s_pix = gdk_pixbuf_new_from_file(pix, &err);
  if (err != NULL) {
    g_critical("error: %s", err->message);
    g_error_free(err);
  }

  w = glade_xml_get_widget(g_xml, "selection");
  gtk_widget_queue_draw(w);

  // set the selection frame on the define tab
  GtkWidget *s = glade_xml_get_widget(g_xml, "selectionFullSize");
  gtk_widget_queue_draw(s);
  draw_define_offscreen_items(s->allocation.width, s->allocation.height);
 
  // find a mass ROI by file name
  if (roi != NULL) {
  	free_roi(roi);
  	roi = NULL;
  }
  roi = get_roi(pix);

   // set case data on define tab
   if (roi == NULL) {
   	clear_case_data();
   } else {
   	write_case_data(roi);
   }
}


void on_mammograms_selection_changed (GtkIconView *view,
					     gpointer user_data) {
  GtkWidget *w;
//  g_debug("mammograms selection changed");
 
  // load the image
  gtk_icon_view_selected_foreach(view, foreach_selection, NULL);

  // draw the offscreen items
  w = glade_xml_get_widget(g_xml, "selection");
  draw_scaled_image(w->allocation.width, w->allocation.height,
  								s_pix, &s_pix_scaled, &scale);
  if (roi != NULL) {
  	gfloat pscale;
  	w = glade_xml_get_widget(g_xml, "queryImage");
  	draw_scaled_image(w->allocation.width, w->allocation.height,
  								roi->pixbuf, &roi->pixbuf_scaled, &pscale);
  }
}


gboolean on_selection_configure_event (GtkWidget *widget,
					   GdkEventConfigure *event,
					   gpointer          user_data) {
  draw_scaled_image(event->width, event->height, s_pix, &s_pix_scaled, &scale);
  return TRUE;
}


gboolean on_selection_expose_event (GtkWidget *d,
				    GdkEventExpose *event,
				    gpointer user_data) {
  
//  g_debug("selected image expose event");
  if (s_pix) {
     gdk_draw_pixbuf(d->window,
		    d->style->fg_gc[GTK_WIDGET_STATE(d)],
		    s_pix_scaled,
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
		draw_roi_contour(d, npoints, xvals, yvals, scale);
		free(xvals);
		free(yvals);

		sprintf(fstr, "%s%02d", UPMC_PREFIX, UPMC_CENTER_X);
		value = g_hash_table_lookup(roi->attrs, fstr);
		int center_x = atoi(value)*4;
		sprintf(fstr, "%s%02d", UPMC_PREFIX, UPMC_CENTER_Y);
		value = g_hash_table_lookup(roi->attrs, fstr);
		int center_y = atoi(value)*4;    
		double frame_x = center_x - (gdk_pixbuf_get_width(roi->pixbuf)/2);
		double frame_y = center_y - (gdk_pixbuf_get_height(roi->pixbuf)/2);
		draw_roi_border(d, frame_x, frame_y, 
						gdk_pixbuf_get_width(roi->pixbuf), 
						gdk_pixbuf_get_height(roi->pixbuf),
						scale);
  	}    
  }
  
  return TRUE;
}


gboolean on_selection_button_press_event(GtkWidget *widget,
					 GdkEventButton *event,
					 gpointer        user_data) {
//  g_debug("selection button press event");
  
  // read and draw the mass ROI
  GtkWidget *w = glade_xml_get_widget(g_xml, "massWindow");
  GtkWidget *m = glade_xml_get_widget(g_xml, "massImage");

  // clear previous contents
  gtk_widget_queue_draw(w);

  if (roi) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(m), roi->pixbuf);
  } else {
    gtk_image_clear(GTK_IMAGE(m));
  }

  gtk_widget_show_all(w);

  return TRUE;

}

