#ifndef FIL_BOOSTLDM_H_
#define FIL_BOOSTLDM_H_


#include "lib_filter.h"

typedef struct boostldm_config
{
  int  numFeatures;
  float   *features;  /* features computed on object */
} boostldm_config_t;


#endif /*FIL_BOOSTLDM_H_*/
