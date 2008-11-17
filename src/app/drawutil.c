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
 
#include <glib.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>
#include "drawutil.h"


void compute_thumbnail_scale(double *scale, gint *w, gint *h) {
  float p_aspect = (float) *w / (float) *h;

  if (p_aspect < 1) {
    /* then calculate width from height */
    *scale = 100.0 / (double) *h;
    *h = 100;
    *w = *h * p_aspect;
  } else {
    /* else calculate height from width */
    *scale = 100.0 / (double) *w;
    *w = 100;
    *h = *w / p_aspect;
  }
}

void compute_proportional_scale(gint *w, gint *h, float p_aspect) {
  float w_aspect = (float) *w / (float) *h;
	
	/* is window wider than pixbuf? */
	if (p_aspect < w_aspect) {
		/* then calculate width from height */
		*w = *h * p_aspect;
	} else {
		/* else calculate height from width */
		*h = *w / p_aspect;
	}
}

void convert_cairo_argb32_to_pixbuf(guchar *pixels,
				    gint w, gint h, gint stride) {
  gint x, y;

  // swap around the data
  // XXX also handle pre-multiplying?
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      // XXX check endian
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
      guchar *p = pixels + (stride * y) + (4 * x);

      guchar r = p[2];
      guchar b = p[0];

      p[0] = r;
      p[2] = b;
#else
      guint *p = (guint *) (pixels + (stride * y) + (4 * x));
      *p = GUINT32_SWAP_LE_BE(*p);
#endif
    }
  }
}

void draw_thumbnail_border(GdkPixbuf *pix, gint w, gint h, 
							float r, float g, float b) {
	guchar *pixels = gdk_pixbuf_get_pixels(pix);
  	int stride = gdk_pixbuf_get_rowstride(pix);
  	cairo_surface_t *surface = cairo_image_surface_create_for_data(pixels,
								 CAIRO_FORMAT_ARGB32,
								 w, h, stride);
 	cairo_t *cr = cairo_create(surface);
 	cairo_rectangle (cr, 0, 0, w, h);
 	cairo_set_line_width(cr, 10.0);
 	cairo_set_source_rgb (cr, r, g, b); 
 	cairo_stroke (cr);

  	cairo_destroy(cr);
  	cairo_surface_destroy(surface);

  	convert_cairo_argb32_to_pixbuf(pixels, w, h, stride);
}


void draw_roi_border(GtkWidget *w, double x, double y, 
					double wi, double ht, double scale) {

	cairo_t *cr = gdk_cairo_create(w->window);

	cairo_scale(cr, scale, scale);
  	cairo_rectangle (cr, x, y, wi, ht);
 	cairo_set_line_width(cr, 10.0);
 	cairo_set_source_rgb (cr, 1.0, 0, 0);  // in red!
 	cairo_stroke (cr);
	
	cairo_destroy(cr);	
}

void draw_pixbuf_roi_border(GdkPixbuf *p, double x, double y, 
						double wi, double ht, double scale) {

	guchar *pixels = gdk_pixbuf_get_pixels(p);
  	cairo_surface_t *surface = cairo_image_surface_create_for_data(pixels,
								 CAIRO_FORMAT_ARGB32,
								 gdk_pixbuf_get_width(p), 
								 gdk_pixbuf_get_height(p), 
								 gdk_pixbuf_get_rowstride(p));
 	cairo_t *cr = cairo_create(surface);
	cairo_scale(cr, scale, scale);
  	cairo_rectangle (cr, x, y, wi, ht);
 	cairo_set_line_width(cr, 10.0);
 	cairo_set_source_rgb (cr, 1.0, 0, 0);  // in red!
 	cairo_stroke (cr);
	
	cairo_destroy(cr);	
}

void draw_roi_contour(GtkWidget *w, int npoints, 
					double *xvals, double *yvals, double scale) {
	int i;
	cairo_t *cr = gdk_cairo_create(w->window);

	cairo_scale(cr, scale, scale);
	cairo_move_to(cr, xvals[0], yvals[0]);
	for (i = 1; i < npoints; i++) {
		cairo_line_to(cr, xvals[i], yvals[i]);
	}
	cairo_close_path(cr);
 	cairo_set_line_width(cr, 2.0);
 	cairo_set_source_rgb (cr, 1.0, 0, 0);  // in red!
 	cairo_stroke (cr);
	
	cairo_destroy(cr);	
}

void draw_pixbuf_roi_contour(GdkPixbuf *p, int npoints, 
						double *xvals, double *yvals, double scale) {
	int i;
	
	guchar *pixels = gdk_pixbuf_get_pixels(p);
  	cairo_surface_t *surface = cairo_image_surface_create_for_data(pixels,
								 CAIRO_FORMAT_ARGB32,
								 gdk_pixbuf_get_width(p), 
								 gdk_pixbuf_get_height(p), 
								 gdk_pixbuf_get_rowstride(p));
 	cairo_t *cr = cairo_create(surface);
	cairo_scale(cr, scale, scale);
 	cairo_move_to(cr, xvals[0], yvals[0]);
	for (i = 1; i < npoints; i++) {
		cairo_line_to(cr, xvals[i], yvals[i]);
	}
	cairo_close_path(cr);
	cairo_set_line_width(cr, 2.0);
 	cairo_set_source_rgb (cr, 1.0, 0, 0);  // in red!
 	cairo_stroke (cr);
	
	cairo_destroy(cr);	
}

void draw_scaled_image(gint allocation_width, gint allocation_height,
						GdkPixbuf *pix, GdkPixbuf **spix, gfloat *sc) {
  // clear old scaled pix
  if (*spix != NULL) {
    g_object_unref(*spix);
    *spix = NULL;
  }

  // if something selected?
  if (pix != NULL) {
    float p_aspect =
      (float) gdk_pixbuf_get_width(pix) /
      (float) gdk_pixbuf_get_height(pix);
    int w = allocation_width;
    int h = allocation_height;
    float w_aspect = (float) w / (float) h;

    g_debug("w: %d, h: %d, p_aspect: %g, w_aspect: %g",
    	    w, h, p_aspect, w_aspect);

    /* is window wider than pixbuf? */
    if (p_aspect < w_aspect) {
      /* then calculate width from height */
      w = h * p_aspect;
      *sc = (float) allocation_height
				/ (float) gdk_pixbuf_get_height(pix);
    } else {
      /* else calculate height from width */
      h = w / p_aspect;
      *sc = (float) allocation_width
				/ (float) gdk_pixbuf_get_width(pix);
    }

    *spix = gdk_pixbuf_scale_simple(pix, w, h,
					   GDK_INTERP_BILINEAR);
  }
}

