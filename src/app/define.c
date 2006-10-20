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
#include <string.h>

#include "massfind.h"
#include "define.h"
#include "roimap.h"

extern roi_t *roi;

GtkListStore *saved_search_store;
static GdkPixbuf *c_pix_scaled;
static float scale;
int distanceMetric = EUCLIDIAN;
double threshold;

extern GdkPixbuf *s_pix;
extern GdkPixbuf *roi_pix;


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



