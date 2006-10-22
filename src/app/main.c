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
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "massfind.h"

static void setup_saved_search_store(void) {
  GtkTreeView *v = GTK_TREE_VIEW(glade_xml_get_widget(g_xml,
						      "definedSearches"));
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  saved_search_store = gtk_list_store_new(5,
					  G_TYPE_STRING,
					  G_TYPE_INT,
					  G_TYPE_DOUBLE,
					  G_TYPE_INT,
					  G_TYPE_POINTER);

  gtk_tree_view_set_model(v, GTK_TREE_MODEL(saved_search_store));


  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes ("Name",
						     renderer,
						     "text", 0,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (v), column);
}

static void setup_thumbnails(GtkIconView *g, gchar *file) {
  GtkListStore *s;
  GtkTreeIter iter;
  gchar buf[1024];
  gchar *dirname = g_path_get_dirname(file);

  // get index
  FILE *f = fopen(file, "r");
  if (f == NULL) {
    perror("Error getting index");
    exit(1);
  }

  // create the model
  s = gtk_list_store_new(3, GDK_TYPE_PIXBUF, 
			 G_TYPE_STRING, 
			 G_TYPE_STRING);

  // get all the thumbnails
  while (1) {
    GError *err = NULL;
    gchar *filename;
    GdkPixbuf *pix;

    int result;

    result = fscanf(f, "%1024s", buf);
    if (result == EOF) {
      break;
    } else if (result != 1) {
      perror("bad result");
    }

    filename = g_build_filename(dirname, buf, NULL);

    pix = gdk_pixbuf_new_from_file_at_size(filename,
					   150,
					   -1,
					   &err);
    if (err != NULL) {
      g_critical("error: %s", err->message);
      g_error_free(err);
    }

    // get the case name
    char *suffix = strchr(buf, '.');
    if (suffix != NULL)
      suffix[0] = '\0';

    gtk_list_store_append(s, &iter);
    gtk_list_store_set(s, &iter,
		       0, pix,
		       1, filename,
		       2, buf,
		       -1);


    g_object_unref(pix);
    g_free(filename);
  }

  // set it up
  gtk_icon_view_set_model(g, GTK_TREE_MODEL(s));
  gtk_icon_view_set_pixbuf_column(g, 0);
  gtk_icon_view_set_text_column(g, 2);

  fclose(f);
  g_free(dirname);
}


static void setup_results_store(GtkIconView *g) {

  found_items =
    gtk_list_store_new(4,
		       GDK_TYPE_PIXBUF,
		       G_TYPE_STRING,
		       GDK_TYPE_PIXBUF,
		       G_TYPE_INT);

  gtk_icon_view_set_model(g, GTK_TREE_MODEL(found_items));
  gtk_icon_view_set_pixbuf_column(g, 0);
  gtk_icon_view_set_text_column(g, 1);
}


void on_about1_activate (GtkMenuItem *menuitem, gpointer user_data) {
  gtk_widget_show(glade_xml_get_widget(g_xml, "aboutdialog1"));
}

void on_zoom_in_activate (GtkMenuItem *menuitem, gpointer user_data) {
}

void on_zoom_out_activate (GtkMenuItem *menuitem, gpointer user_data) {
}

void on_best_fit_activate (GtkMenuItem *menuitem, gpointer user_data) {
}

void on_normal_size_activate (GtkMenuItem *menuitem, gpointer user_data) {
}

void on_show_masses_activate (GtkMenuItem *menuitem, gpointer user_data) {
  // get the setting here;
}


GladeXML *g_xml;

int
main (int argc, char *argv[])
{
  GtkWidget *massfind;

  gtk_init(&argc,&argv);
  g_xml = glade_xml_new(MASSFIND_GLADEDIR "/massfind.glade",NULL,NULL);
  g_assert(g_xml != NULL);

  if (argc != 2) {
    g_critical("No image index given on command line");
    return;
  }

  glade_xml_signal_autoconnect(g_xml);
  massfind = glade_xml_get_widget(g_xml,"massfind");


  // setup the thumbnails
  setup_thumbnails(GTK_ICON_VIEW(glade_xml_get_widget(g_xml,
						      "mammograms")),
		  						argv[1]);

  // init saved searches
  setup_saved_search_store();

  // init results
  setup_results_store(GTK_ICON_VIEW(glade_xml_get_widget(g_xml,
							 "searchResults")));


  gtk_widget_show_all(massfind);
  
  signal(SIGPIPE, SIG_IGN);

  gtk_main();
  return 0;
}

