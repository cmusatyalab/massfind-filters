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

static gdouble prescale;
static gdouble display_scale = 1.0;

static ls_search_handle_t dr;
static guint search_idle_id;

static GtkTreePath *current_result_path;

static gboolean is_adding;
static gint x_add_start;
static gint y_add_start;
static gint x_add_current;
static gint y_add_current;


static void stop_search(void) {
  if (dr != NULL) {
    printf("terminating search\n");
    g_source_remove(search_idle_id);
    ls_terminate_search(dr);
    dr = NULL;
  }
}


static void foreach_select_investigation(GtkIconView *icon_view,
					 GtkTreePath *path,
					 gpointer data) {
  GError *err = NULL;

  GtkTreeIter iter;
  GtkTreeModel *m = gtk_icon_view_get_model(icon_view);
  GtkWidget *w;

  // save path
  if (current_result_path != NULL) {
    gtk_tree_path_free(current_result_path);
  }
  current_result_path = gtk_tree_path_copy(path);

  gtk_tree_model_get_iter(m, &iter, path);

  w = glade_xml_get_widget(g_xml, "selectedResult");
  gtk_widget_queue_draw(w);
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

	g_debug("starting search, metric %d, #features = %d, threshold %d", 
			metric, n, threshold);

    // reset stats
    total_objects = 0;
    processed_objects = 0;
    dropped_objects = 0;

    // diamond
	dr = diamond_similarity_search(metric, threshold, n, f);

    // take the handle, put it into the idle callback to get
    // the results?
    search_idle_id = g_idle_add(diamond_result_callback, dr);

    // activate the stop search button
    gtk_widget_set_sensitive(stopSearch, TRUE);

    // deactivate our button
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  }
}

void on_searchResults_selection_changed (GtkIconView *view,
					 gpointer user_data) {
  GtkWidget *w;

  // load the image
  gtk_icon_view_selected_foreach(view, foreach_select_investigation, NULL);

}
