#ifndef FIL_QALDM_H_
#define FIL_QALDM_H_


#include "lib_filter.h"

typedef struct qaldm_config
{
  int  numFeatures;
  float   *features;  /* features computed on object */
} qaldm_config_t;

#endif /*FIL_QALDM_H_*/
