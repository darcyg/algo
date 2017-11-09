#include "trig.h"
#include "task.h"
#include "comm.h"
#include "ds.h"

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

	timer_set(te.th, &te.simu_timer, TRIG_DELT);
}

void trig_step() {
	timer_cancel(te.th, &te.step_timer);
	timer_set(te.th, &te.step_timer, 10);
}
void trig_push(emTaskType_t type, void *data, int len) {

	stTask_t *t = MALLOC(sizeof(stTask_t) + len);
	if (t == NULL) {
		 return ;
	}
	t->type = type;
	t->data = (void *)(t+1);
	t->len = len;
	if (t->len > 0) {
		memcpy(t->data, data, t->len);
	}

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

		stTableRecord_t tr;
		int ret = ds_search_record_simple("basicinfo", &tr, "limit 0, 1");
		if (ret != 1) {
			log_info("Can't Search baseinfo");
		} else {
			stAccessRecord_t a;
			memset(&a, 0, sizeof(a));
			strcpy(a.mac, tr.basic.mac);
			strcpy(a.dev_uuid, tr.basic.uuid);
			strcpy(a.dev_number, tr.basic.dev_number);
			task_push(t, (void *)&a, sizeof(stAccessRecord_t));
		}
	} else if (i == 1) {
		t = TT_RPT_ALARM;

		stTableRecord_t tr;
		int ret = ds_search_record_simple("basicinfo", &tr, "limit 0, 1");
		if (ret != 1) {
			log_info("Can't Search baseinfo");
		} else {
			stAlarm_t a;
			memset(&a, 0, sizeof(a));
			strcpy(a.mac, tr.basic.mac);
			strcpy(a.device_uuid, tr.basic.uuid);

			task_push(t, (void *)&a, sizeof(stAlarm_t));
		}

	} 

	task_step();
	i = (i + 1) % 2;

	timer_set(te.th, &te.simu_timer, TRIG_DELT);
}

void trig_in(void *arg, int fd) {
	;
}

int  trig_getfd() {
	return -1;
}


