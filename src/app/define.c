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

#include "massfind.h"
#include "define.h"

GtkListStore *saved_search_store;
static GdkPixbuf *c_pix_scaled;
static float scale;
enum distanceMetricType distanceMetric = EUCLIDIAN;
double threshold;

extern GdkPixbuf *s_pix;
extern GdkPixbuf *roi_pix;


// center scrolled window on mass ROI?

void on_saveSearchButton_clicked (GtkButton *button,
				  gpointer   user_data) {
  GtkTreeIter iter;

  const gchar *save_name =
    gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(g_xml, "searchName")));

  g_debug("making new search: %s", save_name);
  gtk_list_store_append(saved_search_store, &iter);
  gtk_list_store_set(saved_search_store, &iter,
		     0, save_name, -1);
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



