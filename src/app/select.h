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

#ifndef SELECT_H
#define SELECT_H

#include <gtk/gtk.h>

void on_fullImages_selection_changed (GtkIconView *view,
					     gpointer user_data);
					     
gboolean on_selectedImage_expose_event (GtkWidget *d,
					GdkEventExpose *event,
					gpointer user_data);
					     
gboolean on_selectedImage_button_press_event(GtkWidget      *widget,
                                            GdkEventButton *event,
                                            gpointer        user_data);
									 

#endif
