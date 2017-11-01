#include "task.h"


static stTaskEnv_t te = {
	.fet = NULL;
	.th = NULL;
};

static tt_register_handler(stTask_t *t);
static tt_rpt_dev_status(stTask_t *t);
static tt_rpt_alarm(stTask_t *t);
static tt_rpt_record(stTask_t *t);
static stTaskHandler_t ths[] = {
	{TT_REGISTER ,			tt_register_handler},
	{TT_RPT_DEV_STATUS, tt_rpt_dev_status},
	{TT_RPT_ALARM,			tt_rpt_alarm},
	{TT_RPT_RECORD,			tt_rpt_record},
};

void task_init(void *_th, void *_fet) {
	te.th			= _th;
	te.fet		= _fet;
	te.state	= DS_UNREGISTER;

	timer_init(&te.step_timer, task_run);

	timer_init(&te.stat_timer, stat_run);
	timer_set(te.th, &te.stat_timer, 10);

	lockqueue_init(&env.msgq);
}
void task_step() {
	timer_cancel(te.th, &env.step_timer);
	timer_set(te.th, &env.step_timer, 10);
}
void task_push(emTaskType type, void *data, int len) {
	stTask_t *t = MALLOC(sizeof(stTask_t));
	if (t == NULL) {
		 return ;
	}
	t->type = type;
	t->data = data;
	t->len = len;

	lockqueue_push(&te.msgq, t);

	task_step();
	return 0;

}
void task_run(void *timer) {
	stTask_t *t = NULL;
	if (lockqueue_pop(&te.msgq, (void **)&t) && t != NULL) {
		task_handler_task(t);
		FREE(t);
		task_step();
	}

}
void stat_run(void *timer) {
	//timer_cancel(te.th, &env.stat_timer);

	emTaskType_t t = 0;
	int delt = 0;

	if (te.state == DS_UNREGISTER) {
		t = TT_REGISTER;
		delt = 1000*30;
	} else {
		t = TT_RPT_DEV_STATUS;
		delt = 1000*60*5;
	}
	task_push(t, NULL, 0);
	timer_set(te.th, &env.stat_timer, delt);
}
void task_in(void *arg, int fd) {
	;
}
int  task_getfd() {
}


void task_handler_task(stTask_t *t) {
	int count = sizeof(ths)/sizeof(ths[0]);
	int i = 0;
	for (i = 0; i < count; i++) {
		stTaskHandler_t *th = &ths[i];
		if (t->type == 
	}
}

static tt_register_handler(stTask_t *t) {
}
static tt_rpt_dev_status(stTask_t *t) {
}
static tt_rpt_alarm(stTask_t *t) {
}
static tt_rpt_record(stTask_t *t) {
}



