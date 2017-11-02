#ifndef __TASK_H_
#define __TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "ayla/log.h"
#include "ayla/timer.h"
#include "ayla/file_event.h"
#include "lockqueue.h"

#include "comm.h"

typedef enum emDevState {
	DS_UNREGISTER = 0,
	DS_REGISTER		= 1,
}emDevState_t;

typedef struct stTaskEnv {
	//struct file_event_table		*fet;
	//struct timer_head					*th;

	void		*fet;
	void		*th;
	emDevState_t							state;
	struct timer							step_timer;
	struct timer							stat_timer;

	stLockQueue_t							msgq;
}stTaskEnv_t;


void task_init(void *_th, void *_fet);
void task_step();
void task_push(emTaskType_t type, void *data, int len);
void task_run(struct timer *timer);
void stat_run(struct timer *timer);
void task_in(void *arg, int fd);
int  task_getfd();




#endif
