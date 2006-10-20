#ifndef FIL_BOOSTLDM_H_
#define FIL_BOOSTLDM_H_


#include "lib_filter.h"

typedef struct boostldm_config
{
  int  numFeatures;
  float   *features;  /* features computed on object */
} boostldm_config_t;

int f_init_boostldm(int numarg, char **args, int blob_len,
                    void *blob, const char *fname, void **data);
int f_eval_boostldm(lf_obj_handle_t ohandle, void *f_data);
int f_fini_boostldm(void *data);

#endif /*FIL_BOOSTLDM_H_*/
