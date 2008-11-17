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

#ifndef MASSFIND_H
#define MASSFIND_H

#include <glade/glade.h>
#include <gtk/gtk.h>

extern GladeXML *g_xml;

extern GtkListStore *saved_search_store;
extern GtkListStore *found_items;

enum zoomAction { NONE, ZOOM_IN, ZOOM_OUT, NORMAL_SIZE, BEST_FIT };
// need zoom factors

void get_data_directory(char **dirp);

#endif
