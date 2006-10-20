#ifndef FIL_QALDM_H_
#define FIL_QALDM_H_


#include "lib_filter.h"

typedef struct qaldm_config
{
  int  numFeatures;
  float   *features;  /* features computed on object */
} qaldm_config_t;

int f_init_qaldm(int numarg, char **args, int blob_len,
                    void *blob, const char *fname, void **data);
int f_eval_qaldm(lf_obj_handle_t ohandle, void *f_data);
int f_fini_qaldm(void *data);

#endif /*FIL_QALDM_H_*/
