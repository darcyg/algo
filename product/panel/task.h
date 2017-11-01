#ifndef __TASK_H_
#define __TASK_H_

#include "common.h"
#include "ayla/log.h"
#include "ayla/timer.h"
#include "ayla/file_event.h"
#include "lockqueue.h"


typedef enum emDevState {
	DS_UNREGISTER = 0,
	DS_REGISTER		= 1,
}emDevState_t;

typedef enum emTaskType {
	TT_REGISTER	= 0,
	TT_RPT_DEV_STATUS = 1,
	TT_RPT_ALARM = 2,
	TT_RPT_RECORD = 3, 
}emTaskType_t;

typedef struct stTask {
	emTaskType_t	type;
	int  len;
	void *data;
}stTask_t;

typedef struct stTaskEnv {
	void					*fet;
	void					*th;
	emDevState_t	state;
	struct timer	step_timer;
	struct timer	stat_timer;

	stLockQueue_t	msgq;
}stTaskEnv_t;

typedef void (*THANDLER)(stTask_t*);
typedef stTaskHandler {
	int				type;
	THANDLER	*handler;
}stTaskHandler_t;

void task_handler_task(stTask_t *t);

void task_init(void *_th, void *_fet);
void task_step();
void task_push(void *task);
void task_run(void *timer);
void task_in(void *arg, int fd);
int  task_getfd();




#endif
