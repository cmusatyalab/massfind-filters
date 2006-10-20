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

#ifndef ROIMAP_H
#define ROIMAP_H

#include <gdk/gdk.h>
#include <glib.h>

typedef struct {
  char *full_image_name;
  char *roi_image_name;
  int center_x;
  int center_y;
  int subtlety;
  int birad;
  int density;
  int age;
  int biopsy;
  int shape;
  int margin;
  GdkPixbuf *pixbuf;
  GHashTable *attrs;
} roi_t;

enum diagnosis {MALIGNANT, BENIGN};
enum mass_shape {ROUND, OVAL, LOBULATED, IRREGULAR};
enum mass_margin {SPICULATED, ILLDEFINED, MICROLOBULATED, CIRCUMSCRIBED, OBSCURED};

roi_t *get_roi(char *path);
void free_roi(roi_t *r);



#endif
