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

#include "select.h"
#include "roimap.h" 
#include "drawutil.h"

static GdkPixbuf *s_pix;
static GdkPixbuf *s_pix_scaled;
static gfloat scale;
roi_t *roi = NULL;

static void draw_select_offscreen_items(gint allocation_width,
					gint allocation_height) {
  // clear old scaled pix
  if (s_pix_scaled != NULL) {
    g_object_unref(s_pix_scaled);
    s_pix_scaled = NULL;
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
      scale = (float) allocation_height
	/ (float) gdk_pixbuf_get_height(s_pix);
    } else {
      /* else calculate height from width */
      h = w / p_aspect;
      scale = (float) allocation_width
	/ (float) gdk_pixbuf_get_width(s_pix);
    }

    s_pix_scaled = gdk_pixbuf_scale_simple(s_pix,
					   w, h,
					   GDK_INTERP_BILINEAR);
  }
}


void write_case_data(roi_t *roi) {
	
   char textLabel[80];
   GtkWidget *w;
   
   w = glade_xml_get_widget(g_xml, "caseId");
   strcpy(textLabel, roi->case_name);
   gtk_label_set_text(GTK_LABEL(w), textLabel);
   
   w = glade_xml_get_widget(g_xml, "caseAge");
   sprintf(textLabel, "%d", roi->age);
   gtk_label_set_text(GTK_LABEL(w), textLabel);
  
   w = glade_xml_get_widget(g_xml, "caseBiopsy");
   strcpy(textLabel, "MALIGNANT");
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
  
   w = glade_xml_get_widget(g_xml, "caseBiopsy");
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
  draw_select_offscreen_items(s->allocation.width,
			      			s->allocation.height);
  gtk_image_set_from_pixbuf(GTK_IMAGE(s), s_pix_scaled);
  
  // find a mass ROI by file name
  if (roi != NULL) {
  	free_roi(roi);
  	roi = NULL;
  }
  roi = get_roi(pix);

/*  
  if (roi && show_masses) {
  		draw_pixbuf_roi_center(s_pix_scaled, roi->center_x, roi->center_y, scale);
		double frame_x = roi->center_x - (gdk_pixbuf_get_width(roi->pixbuf)/2);
		double frame_y = roi->center_y - (gdk_pixbuf_get_height(roi->pixbuf)/2);
		draw_pixbuf_roi_border(s_pix_scaled, frame_x, frame_y, 
						gdk_pixbuf_get_width(roi->pixbuf), 
						gdk_pixbuf_get_height(roi->pixbuf),
						scale);
  }
  */

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
  g_debug("mammograms selection changed");
 
  // load the image
  gtk_icon_view_selected_foreach(view, foreach_selection, NULL);

  // draw the offscreen items
  w = glade_xml_get_widget(g_xml, "selection");
  draw_select_offscreen_items(w->allocation.width,
			      w->allocation.height);

}



gboolean on_selection_expose_event (GtkWidget *d,
				    GdkEventExpose *event,
				    gpointer user_data) {
  g_debug("selected image expose event");
  if (s_pix) {
     gdk_draw_pixbuf(d->window,
		    d->style->fg_gc[GTK_WIDGET_STATE(d)],
		    s_pix_scaled,
		    0, 0, 0, 0,
		    -1, -1,
		    GDK_RGB_DITHER_NORMAL,
		    0, 0);
	if (roi && show_masses) {
  		// draw mass roi seed and border on (scaled) full image
		draw_roi_center(d, roi->center_x, roi->center_y, scale);
		double frame_x = roi->center_x - (gdk_pixbuf_get_width(roi->pixbuf)/2);
		double frame_y = roi->center_y - (gdk_pixbuf_get_height(roi->pixbuf)/2);
		draw_roi_border(d, frame_x, frame_y, 
						gdk_pixbuf_get_width(roi->pixbuf), 
						gdk_pixbuf_get_height(roi->pixbuf),
						scale);
  	}    
  }
  

  return TRUE;
}


gboolean on_selection_button_press_event(GtkWidget      *widget,
					 GdkEventButton *event,
					 gpointer        user_data) {
  g_debug("selection button press event");
  
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

