#ifndef __TRIG_H_
#define __TRIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "ayla/log.h"
#include "ayla/timer.h"
#include "ayla/file_event.h"
#include "lockqueue.h"

#include "comm.h"

typedef struct stTrigEnv {
	//struct file_event_table		*fet;
	//struct timer_head					*th;
	void		*fet;
	void		*th;

	struct timer	simu_timer;

	struct timer	step_timer;
	stLockQueue_t	msgq;

}stTrigEnv_t;



void trig_init(void *_th, void *_fet);
void trig_step();
void trig_push(emTaskType_t type, void *data, int len);
void trig_run(struct timer *timer);
void simu_run(struct timer *timer);
void trig_in(void *arg, int fd);
int  trig_getfd();

//#define TRIG_DELT (1000*60*15)
#define TRIG_DELT (1000*60)


#endif
