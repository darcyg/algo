#include "trig.h"
#include "task.h"

static stTrigEnv_t te = {
	.fet = NULL,
	.th = NULL,
};

static stTaskHandler_t ths[] = {
};

void trig_init(void *_th, void *_fet) {
	te.th			= _th;
	te.fet		= _fet;

	timer_init(&te.simu_timer,		simu_run);

	timer_init(&te.step_timer,		trig_run);
	lockqueue_init(&te.msgq);

	timer_set(te.th, &te.simu_timer, 1000*60*15);
}

void trig_step() {
	timer_cancel(te.th, &te.step_timer);
	timer_set(te.th, &te.step_timer, 10);
}
void trig_push(emTaskType_t type, void *data, int len) {

	stTask_t *t = MALLOC(sizeof(stTask_t));
	if (t == NULL) {
		 return ;
	}
	t->type = type;
	t->data = data;
	t->len = len;

	lockqueue_push(&te.msgq, t);

	trig_step();
}
void trig_run(struct timer *timer) {
	stTask_t *t = NULL;
	if (lockqueue_pop(&te.msgq, (void **)&t) && t != NULL) {
		task_handler_task(t, ths, sizeof(ths)/sizeof(ths[0]));
		FREE(t);
		trig_step();
	}
}
void simu_run(struct timer *timer) {
	static int i = 0;
	emTaskType_t t = 0;
	void *data = NULL;
	int len = 0;
	
	if (i == 0) {
		t = TT_RPT_RECORD;
		len = sizeof(stAccessRecord_t);
		data = (stAccessRecord_t*)malloc(len);
	} else if (i == 1) {
		t = TT_RPT_ALARM;
		len = sizeof(stAlarm_t);
		data = (stAlarm_t*)malloc(len);
	} 

	task_push(t, data,len);
	task_step();
	i = (i + 1) % 2;

	timer_set(te.th, &te.simu_timer, 1000*60*15);
}

void trig_in(void *arg, int fd) {
	;
}

int  trig_getfd() {
	return -1;
}


