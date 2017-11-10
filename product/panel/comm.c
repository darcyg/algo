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

const char *osa_get_error_string(int err) {
	if (err == OSA_STATUS_OK) {
		return "OSA_STATUS_OK";
	} else if (err == OSA_STATUS_EINVAL) {
		return "OSA_STATUS_EINVAL";
	} else if (err == OSA_STATUS_ENOMEM) {
		return "OSA_STATUS_ENOMEM";
	} else if (err == OSA_STATUS_EEXIST) {
		return "OSA_STATUS_EEXIST";
	} else if (err == OSA_STATUS_ENOENT) {
		return "OSA_STATUS_ENOENT";
	} else {
		return "OSA_STATUS_UNKNOWN";
	}
}
