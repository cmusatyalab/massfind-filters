#ifndef FIL_EUCLIDIAN_H_
#define FIL_EUCLIDIAN_H_


#include "lib_filter.h"

typedef struct euclidian_config
{
  int  numFeatures;
  float   *features;  /* features computed on object */
} euclidian_config_t;

int f_init_euclidian(int numarg, char **args, int blob_len,
                    void *blob, const char *fname, void **data);
int f_eval_euclidian(lf_obj_handle_t ohandle, void *f_data);
int f_fini_euclidian(void *data);

#endif /*FIL_EUCLIDIAN_H_*/
