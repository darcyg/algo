#ifndef __DS_H_
#define __DS_H_


#ifdef __cplusplus 
extern "C" {
#endif




int ds_init(const char *path, const char *name, int option);

int ds_set_max_num(const char *tbl, int num);

int ds_select();
int ds_insert();
int ds_update();
int ds_delete();

int ds_unlink();










#ifdef __cplusplus
}
#endif

#endif
