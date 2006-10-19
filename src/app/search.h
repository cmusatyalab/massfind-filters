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

#ifndef SEARCH_H
#define SEARCH_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif
  void on_clearSearch_clicked (GtkButton *button,
			       gpointer   user_data);

  void on_stopSearch_clicked (GtkButton *button,
			      gpointer   user_data);

  void on_startSearch_clicked (GtkButton *button,
			       gpointer   user_data);

  void on_searchResults_selection_changed (GtkIconView *view,
					   gpointer user_data);


#ifdef __cplusplus
}
#endif

#endif
