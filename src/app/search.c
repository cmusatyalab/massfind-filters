/*
 * FatFind: A Diamond application for adipocyte image exploration
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <stdlib.h>
#include <math.h>

#include "massfind.h"
#include "search.h"
#include "diamond_interface.h"

GtkListStore *found_items;

static GdkPixbuf *i_pix;
static GdkPixbuf *i_pix_scaled;

static ls_search_handle_t dr;
static guint search_idle_id;

static void stop_search(void) {
  if (dr != NULL) {
    printf("terminating search\n");
    g_source_remove(search_idle_id);
    ls_terminate_search(dr);
    dr = NULL;
  }
}

static void draw_investigate_offscreen_items(gint allocation_width,
					     gint allocation_height) {
  // clear old scaled pix
  if (i_pix_scaled != NULL) {
    g_object_unref(i_pix_scaled);
    i_pix_scaled = NULL;
  }

  // is something selected?
  if (i_pix) {
  	// scale the image to the allocation size
	float p_aspect = (float) gdk_pixbuf_get_width(i_pix) /
  					(float) gdk_pixbuf_get_height(i_pix);
	int sw = allocation_width;
	int sh = allocation_height;
	float w_aspect = (float) sw / (float) sh;
	
	/* is window wider than pixbuf? */
	if (p_aspect < w_aspect) {
 		/* then calculate width from height */
		sw = sh * p_aspect;
	} else {
		/* else calculate height from width */
  		sh = sw / p_aspect;
	}
	i_pix_scaled = gdk_pixbuf_scale_simple(i_pix,
				   sw, sh,
				   GDK_INTERP_BILINEAR);
 	g_debug("image %x: new scaled %x", i_pix, i_pix_scaled);
 
  }
}

static void foreach_select_result(GtkIconView *icon_view,
					 GtkTreePath *path,
					 gpointer data) {
  GError *err = NULL;

  GtkTreeIter iter;
  GtkTreeModel *m = gtk_icon_view_get_model(icon_view);
  GtkWidget *w;
  gchar *title;
  
  // get the full size image stored for this thumbnail
  gtk_tree_model_get_iter(m, &iter, path);
  gtk_tree_model_get(m, &iter, 
  					1, &title, 
  					2, &i_pix, 
  					-1);
  g_debug("Image %s selected (%x)", title, i_pix);
 
  w = glade_xml_get_widget(g_xml, "selectedResult");
  gtk_widget_queue_draw(w);  
}

gboolean on_selectedResult_expose_event (GtkWidget *d,
					 GdkEventExpose *event,
					 gpointer user_data) {

	g_debug("selected result expose event");
 
  	if (i_pix) {	
  		g_debug("drawing image %x, scaled %x", i_pix, i_pix_scaled);
		gdk_draw_pixbuf(d->window,
		    d->style->fg_gc[GTK_WIDGET_STATE(d)],
		    i_pix_scaled,
		    0, 0, 
		    d->allocation.x, d->allocation.y,
		    -1, -1,
		    GDK_RGB_DITHER_NORMAL,
		    0, 0);
  	}

  	return TRUE;
}



void on_clearSearch_clicked (GtkButton *button,
			     gpointer   user_data) {
  // buttons
  GtkWidget *stopSearch = glade_xml_get_widget(g_xml, "stopSearch");
  GtkWidget *startSearch = glade_xml_get_widget(g_xml, "startSearch");

  gtk_widget_set_sensitive(stopSearch, FALSE);
  gtk_widget_set_sensitive(startSearch, TRUE);

  // stop
  stop_search();

  // clear search thumbnails
  gtk_list_store_clear(found_items);

  // clear result
  if (i_pix != NULL) {
    g_object_unref(i_pix);
    i_pix = NULL;
  }
  gtk_widget_queue_draw(glade_xml_get_widget(g_xml, "selectedResult"));
}

void on_stopSearch_clicked (GtkButton *button,
			    gpointer user_data) {
  GtkWidget *stopSearch = glade_xml_get_widget(g_xml, "stopSearch");
  GtkWidget *startSearch = glade_xml_get_widget(g_xml, "startSearch");
  gtk_widget_set_sensitive(stopSearch, FALSE);
  gtk_widget_set_sensitive(startSearch, TRUE);

   // stop
  stop_search();
}

void on_startSearch_clicked (GtkButton *button,
			     gpointer   user_data) {
  // get the selected search
  GtkTreeIter s_iter;
  GtkTreeSelection *selection =
    gtk_tree_view_get_selection(GTK_TREE_VIEW(glade_xml_get_widget(g_xml,
								   "definedSearches")));
  GtkTreeModel *model = GTK_TREE_MODEL(saved_search_store);

  g_debug("selection: %p", selection);

  if (gtk_tree_selection_get_selected(selection,
				      &model,
				      &s_iter)) {
    gdouble threshold;
  	int metric;
  	int n;
    float *f;
    GtkWidget *stopSearch = glade_xml_get_widget(g_xml, "stopSearch");


    g_debug("saved_search_store: %p", model);
    gtk_tree_model_get(model,
		       &s_iter,
		       1, &metric,
		       2, &threshold,
		       3, &n,
		       4, &f,
		       -1);

	g_debug("starting search, metric %d, #features = %d, threshold %f", 
			metric, n, threshold);

    // reset stats
    total_objects = 0;
    processed_objects = 0;
    dropped_objects = 0;

    // diamond
	dr = diamond_similarity_search(metric, (int) threshold, n, f);

    // take the handle, put it into the idle callback to get
    // the results?
    search_idle_id = g_idle_add(diamond_result_callback, dr);

    // activate the stop search button
    gtk_widget_set_sensitive(stopSearch, TRUE);

    // deactivate our button
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  }
}

void on_reorderResults_clicked (GtkButton *button,
			    gpointer user_data) {
	g_debug("reorder results button clicked");
  	
  	GtkWidget *results = glade_xml_get_widget(g_xml, "searchResults");
}


void on_searchResults_selection_changed (GtkIconView *view,
					 gpointer user_data) {
  GtkWidget *w;

  // load the image
  gtk_icon_view_selected_foreach(view, foreach_select_result, NULL);
  
 // draw the offscreen items
  w = glade_xml_get_widget(g_xml, "selectedResult");
  draw_investigate_offscreen_items(w->allocation.width,
				   w->allocation.height);  
}

