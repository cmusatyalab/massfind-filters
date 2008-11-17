/*
 * MassFind: A Diamond application for exploration of breast tumors
 *
 * Copyright (c) 2006-2008 Carnegie Mellon University. All rights reserved.
 * Copyright (c) 2006-2007 Intel Corporation. All rights reserved.
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
#include <math.h>
#include <sys/param.h>

#include "massfind.h"
#include "diamond_interface.h"
#include "roimap.h"
#include "drawutil.h"
#include "roi_features.h"
#include "upmc_features.h"
#include "lib_scope.h"

#define QUERY_FEATURE_SUFFIX "q"
#define RESULT_FEATURE_SUFFIX "r"

extern roi_t *roi;

GtkListStore *found_items;
GHashTable *result_features;

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

static void foreach_select_result(GtkIconView *icon_view,
					 GtkTreePath *path,
					 gpointer data) {
  GError *err = NULL;

  GtkTreeIter iter;
  GtkTreeModel *m = gtk_icon_view_get_model(icon_view);
  GtkWidget *w;
  gchar *title;
  char roiName[MAXPATHLEN];
  
  // get the full size image stored for this thumbnail
  gtk_tree_model_get_iter(m, &iter, path);
  g_debug("selected path is %s", gtk_tree_path_to_string(path));
  gtk_tree_model_get(m, &iter, 
  					1, &title, 
  					2, &i_pix, 
  					3, &i_pix_scaled,
  					-1);
  g_debug("Image %s selected (%x)", title, i_pix);
  
  // get features for this ROI
  strcpy(roiName, title);
  char *space = strrchr(roiName, ' ');
  if (space != NULL)
  	space[0] = '\0';
  if (result_features != NULL) {
  	   g_hash_table_foreach_remove(result_features, remove_roi_attrs, NULL);
  	   g_hash_table_destroy(result_features);
  }
  result_features = g_hash_table_new(g_str_hash, g_str_equal);
  get_roi_features(roiName, result_features);
 
  w = glade_xml_get_widget(g_xml, "selectedResult");
  gtk_widget_queue_draw(w);  
}

gboolean on_selectedResult_expose_event (GtkWidget *d,
					 GdkEventExpose *event,
					 gpointer user_data) {

	g_debug("selected result expose event");
 
  	if (i_pix) {	
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

gboolean on_selectedResult_configure_event (GtkWidget *widget,
					   GdkEventConfigure *event,
					   gpointer          user_data) {
	if (i_pix) {
		gfloat scale;
		draw_scaled_image(event->width, event->height,
  							i_pix, &i_pix_scaled, &scale);
	}
  	return TRUE;
}

gboolean on_queryImage_expose_event (GtkWidget *d,
					 GdkEventExpose *event,
					 gpointer user_data) {

	g_debug("query image expose event");
 
  	if (roi && roi->pixbuf_scaled) {	
		gdk_draw_pixbuf(d->window,
		    d->style->fg_gc[GTK_WIDGET_STATE(d)],
		    roi->pixbuf_scaled,
		    0, 0, 
		    d->allocation.x, d->allocation.y,
		    -1, -1,
		    GDK_RGB_DITHER_NORMAL,
		    0, 0);
  	}

  	return TRUE;
}

gboolean on_queryImage_configure_event (GtkWidget *widget,
					   GdkEventConfigure *event,
					   gpointer          user_data) {
					   	
	if (roi && roi->pixbuf) {
  		gfloat scale;
  		draw_scaled_image(event->width, event->height,
  						  roi->pixbuf, &roi->pixbuf_scaled, &scale);
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

void on_stopSearch_clicked (GtkButton *button, gpointer user_data) {
  GtkWidget *stopSearch = glade_xml_get_widget(g_xml, "stopSearch");
  GtkWidget *startSearch = glade_xml_get_widget(g_xml, "startSearch");
  gtk_widget_set_sensitive(stopSearch, FALSE);
  gtk_widget_set_sensitive(startSearch, TRUE);

   // stop
  stop_search();
}

void on_startSearch_clicked (GtkButton *button, gpointer user_data) {
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
    search_desc_t *desc;
    GtkWidget *stopSearch = glade_xml_get_widget(g_xml, "stopSearch");

    g_debug("saved_search_store: %p", model);
    gtk_tree_model_get(model, &s_iter, 1, &desc, -1);
	g_debug("starting search %s, type %d", desc->name, desc->searchType);

    // reset stats
    total_objects = 0;
    processed_objects = 0;
    dropped_objects = 0;

    // diamond
	dr = diamond_similarity_search(desc);
	g_assert(dr);

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
  	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(found_items),
                                         4, GTK_SORT_DESCENDING);
    
    // don't reorder new items, if any, until the button is clicked again
  	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(found_items),
  								GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID,
                                GTK_SORT_DESCENDING);
}


void on_searchResults_selection_changed (GtkIconView *view,
					 gpointer user_data) {
  GtkWidget *w;

  // load the image
  gtk_icon_view_selected_foreach(view, foreach_select_result, NULL);
}

void write_feature_data(GHashTable *features, char *suffix) {

   char labelName[10], fstr[10];
   char textLabel[80];
   GtkWidget *w;
   int i;
   
   for (i = 0; i < NUM_UPMC_FEATURES; i++) {
   	   	sprintf(labelName, "upmc%02d%s", i, suffix);
   	   	w = glade_xml_get_widget(g_xml, labelName);
  		sprintf(fstr, "%s%02d", UPMC_PREFIX, i);
  		sprintf(textLabel, "%s", g_hash_table_lookup(features, fstr));
  		gtk_label_set_text(GTK_LABEL(w), textLabel);
   }
}

void clear_feature_data(char *suffix) {
	char labelName[10];
    GtkWidget *w;
    int i;
  	 
    for (i = 0; i < NUM_UPMC_FEATURES; i++) {
		sprintf(labelName, "upmc%02d%s", i, suffix);
   	   	w = glade_xml_get_widget(g_xml, labelName);
		gtk_label_set_text(GTK_LABEL(w), "");
	}
}


gboolean on_queryImageEventBox_button_press_event(GtkWidget *widget,
					 GdkEventButton *event,
					 gpointer        user_data) {
  g_debug("query image button press event");

  write_feature_data(roi->attrs, QUERY_FEATURE_SUFFIX);
  gtk_widget_show_all(glade_xml_get_widget(g_xml, "queryFeatureWindow"));
  
  return TRUE;

}

gboolean on_selectedResultEventBox_button_press_event(GtkWidget *widget,
					 GdkEventButton *event,
					 gpointer        user_data) {
  g_debug("selected result button press event");
  
  write_feature_data(result_features, RESULT_FEATURE_SUFFIX);
  gtk_widget_show_all(glade_xml_get_widget(g_xml, "resultFeatureWindow"));
  
  return TRUE;

}

void on_defineScope_clicked (GtkButton *button,
			     gpointer user_data) {
  ls_define_scope();
}
