#ifndef __TRIG_H_
#define __TRIG_H_






void trig_init(void *_th, void *_fet);
void trig_step();
void trig_push(void *task);
void trig_run(void *timer);
void trig_in(void *arg, int fd);
int  trig_getfd();




#endif
