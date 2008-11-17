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

#ifndef DRAWUTIL_H_
#define DRAWUTIL_H_

#include <glib.h>
#include <gtk/gtk.h>

void compute_thumbnail_scale(double *scale, gint *w, gint *h);

void compute_proportional_scale(gint *w, gint *h, float p_aspect);

void draw_thumbnail_border(GdkPixbuf *pix, gint w, gint h, 
							float r, float g, float b);

void draw_roi_border(GtkWidget *w, double x, double y, 
					double wi, double ht, double scale);
					
void draw_pixbuf_roi_border(GdkPixbuf *p, double x, double y, 
					double wi, double ht, double scale);

void draw_roi_contour(GtkWidget *w, int n, 
					double *x, double *y, double scale);

void draw_pixbuf_roi_contour(GdkPixbuf *p, int n, 
					double *x, double *y, double scale);

void draw_scaled_image(gint width, gint height,
						GdkPixbuf *pix, GdkPixbuf **spix, gfloat *sc);

#endif /*DRAWUTIL_H_*/
