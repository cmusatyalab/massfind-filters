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

#ifndef DEFINE_H
#define DEFINE_H

#include <gtk/gtk.h>

enum distanceMetricType { EUCLIDIAN, BOOSTLDM, QALDM };

void on_define_search_value_changed (GtkRange *range,
				     gpointer  user_data);

void on_saveSearchButton_clicked (GtkButton *button,
				  gpointer   user_data);
				  
void on_euclidianbutton_toggled(GtkToggleButton *togglebutton,
                                gpointer         user_data);
                                
void on_boostedldmbutton_toggled(GtkToggleButton *togglebutton,
                                 gpointer         user_data);
                                 
void on_qaldmbutton_toggled(GtkToggleButton *togglebutton,
                            gpointer         user_data);
                                
#endif
