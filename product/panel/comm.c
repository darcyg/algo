#include "comm.h"

void task_handler_task(stTask_t *t, stTaskHandler_t *ths, int hcnt) {
	int count = hcnt;
	int i = 0;
	for (i = 0; i < count; i++) {
		stTaskHandler_t *th = &ths[i];
		if (t->type == th->type) {
			th->handler(t);
		}
	}
}
