#include "task.h"

#include "jansson.h"
#include "json_parser.h"
#include "cloud.h"

static stTaskEnv_t te = {
	.fet = NULL,
	.th = NULL,
};

static void tt_register_handler(stTask_t *t);
static void tt_rpt_dev_status(stTask_t *t);
static void tt_rpt_alarm(stTask_t *t);
static void tt_rpt_record(stTask_t *t);

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

	lockqueue_init(&te.msgq);
}
void task_step() {
	timer_cancel(te.th, &te.step_timer);
	timer_set(te.th, &te.step_timer, 10);
}
void task_push(emTaskType_t type, void *data, int len) {
	stTask_t *t = MALLOC(sizeof(stTask_t));
	if (t == NULL) {
		 return ;
	}
	t->type = type;
	t->data = data;
	t->len = len;

	lockqueue_push(&te.msgq, t);

	task_step();
}
void task_run(struct timer *timer) {
	stTask_t *t = NULL;
	if (lockqueue_pop(&te.msgq, (void **)&t) && t != NULL) {
		task_handler_task(t, ths, sizeof(ths)/sizeof(ths[0]));
		FREE(t);
		task_step();
	}
}
void stat_run(struct timer *timer) {
	//timer_cancel(te.th, &env.stat_timer);

	emTaskType_t t = 0;
	int delt = 0;
	void *data = NULL;
	int len = 0;

	if (te.state == DS_UNREGISTER) {
		t = TT_REGISTER;
		len = sizeof(stRegister_t);
		data = malloc(len);
		delt = 1000*30;
	} else {
		t = TT_RPT_DEV_STATUS;
		len = sizeof(stStatus_t);
		data = malloc(len);
		delt = 1000*60*5;
	}
	task_push(t, data, len);
	task_step();

	timer_set(te.th, &te.stat_timer, delt);
}
void task_in(void *arg, int fd) {
	;
}
int  task_getfd() {
	return -1;
}

////////////////////////////////////////////////////////////
static void tt_register_handler(stTask_t *t) {
	log_info("regiser... ");

	const char *url = "http://101.132.153.33:5000/api/dev_dbm/register_device";

	json_t *jarg = json_object();
	if (jarg == NULL) {
		log_warn("no memory!");
		return;
	}

	/*
	stTableRecord tr;
	char *tblname = "basicinfo";
	stReq_t r = {server, req, tblname, NULL};
	ds_search_record(tblname, db_test_search_status, &r, "limit 0, 1");
	*/


	static char mac[32]  = "0102030405060708";
	static char uuid[36] = "48473644ea594e9789c447526a50de8d";
	static char dev_number[36];
	static char key[32];

	json_object_set_new(jarg, "mac",	json_string(mac));
	json_object_set_new(jarg, "uuid", json_string(uuid));
	char *sarg = json_dumps(jarg, 0);
	if (sarg == NULL) {
		log_warn("no memory!");
		json_decref(jarg);
		return;
	}

	char param[1024];
	sprintf(param, "argments=%s", sarg);
	free(sarg);
	json_decref(jarg);

	stCloudRes_t cr;
	memset(&cr, 0, sizeof(cr));

	log_info("request:\n\turl:%s\n\tparam:%s", url, param);
	int ret = cloud_http_pst(url, param, &cr);
	if (ret != 0) {
		log_warn("register failed : no response!");
		return;
	}

	log_info("resp:\n\tlen:[%d]\n\tcontent:[%s]", cr.len, (char*)cr.data);

	json_error_t error;
	json_t *jret = json_loads(cr.data, 0, &error);
	if (jret == NULL) {
		log_warn("register failed : response is not json!");
		return;
	}
		
	json_t *jpayload = json_object_get(jret, "payload");
	if (jpayload == NULL) {
		json_decref(jret);
		log_warn("register failed : response does't has payload segment!");
	}

	const char *rmac				= json_get_string(jpayload, "mac");
	const char *ruuid				= json_get_string(jpayload, "uuid");
	const char *rdev_number	= json_get_string(jpayload, "dev_number");
	const char *rkey				= json_get_string(jpayload, "key");

	if (rmac == NULL || ruuid == NULL || rdev_number == NULL || rkey == NULL) {
		log_warn("register failed : response format error!");
		json_decref(jret);
		return;
	}
	strcpy(uuid, ruuid);
	strcpy(dev_number, rdev_number);
	strcpy(key, rkey);

	json_decref(jret);

	te.state = DS_REGISTER;
	log_info("register ok!");
}
static void tt_rpt_dev_status(stTask_t *t) {
	log_info("regiser dev status: ");

	const char *url = "http://101.132.153.33:5000/api/dev_dbm/report_device_status";

	json_t *jarg = json_object();
	if (jarg == NULL) {
		log_warn("no memory!");
		return;
	}

	stStatus_t *s = (stStatus_t*)t->data;
	/*
	stTableRecord tr;
	char *tblname = "basicinfo";
	stReq_t r = {server, req, tblname, NULL};
	ds_search_record(tblname, db_test_search_status, &r, "limit 0, 1");
	*/

	json_object_set_new(jarg, "dev_uuid", json_string(s->dev_uuid));
	json_object_set_new(jarg, "mac", json_string(s->mac));
	json_object_set_new(jarg, "hw_ver", json_string(s->hw_ver));
	json_object_set_new(jarg, "sf_ver", json_string(s->sf_ver));
	json_object_set_new(jarg, "imsi", json_string(s->imsi));
	json_object_set_new(jarg, "msisdn", json_string(s->msisdn));
	json_object_set_new(jarg, "battery", json_integer(s->battery));
	json_object_set_new(jarg, "temperature", json_integer(s->temperature));
	json_object_set_new(jarg, "signal_", json_integer(s->signal_));
	json_object_set_new(jarg, "capa_capacity", json_integer(s->capa_capacity));
	json_object_set_new(jarg, "whitelist_count", json_integer(s->whitelist_count));
	json_object_set_new(jarg, "finger_capacity", json_integer(s->finger_capacity));
	json_object_set_new(jarg, "finger_count", json_integer(s->finger_count));
	json_object_set_new(jarg, "opened", json_integer(s->opened));
	json_object_set_new(jarg, "work_mode", json_integer(s->work_mode));
	json_object_set_new(jarg, "power_mode", json_integer(s->power_mode));
	char *sarg = json_dumps(jarg, 0);
	if (sarg == NULL) {
		log_warn("no memory!");
		json_decref(jarg);
		return;
	}

	char param[1024];
	sprintf(param, "argments=%s", sarg);
	free(sarg);
	json_decref(jarg);

	stCloudRes_t cr;
	memset(&cr, 0, sizeof(cr));

	log_info("request:\n\turl:%s\n\tparam:%s", url, param);
	int ret = cloud_http_pst(url, param, &cr);
	if (ret != 0) {
		log_warn("register failed : no response!");
		return;
	}

	log_info("resp:\n\tlen:[%d]\n\tcontent:[%s]", cr.len, (char*)cr.data);

	json_error_t error;
	json_t *jret = json_loads(cr.data, 0, &error);
	if (jret == NULL) {
		log_warn("register failed : response is not json!");
		return;
	}
	const char *status			= json_get_string(
		
	json_t *jpayload = json_object_get(jret, "payload");
	if (jpayload == NULL) {
		json_decref(jret);
		log_warn("register failed : response does't has payload segment!");
	}

	const char *rmac				= json_get_string(jpayload, "mac");
	const char *ruuid				= json_get_string(jpayload, "uuid");
	const char *rdev_number	= json_get_string(jpayload, "dev_number");
	const char *rkey				= json_get_string(jpayload, "key");

	if (rmac == NULL || ruuid == NULL || rdev_number == NULL || rkey == NULL) {
		log_warn("register failed : response format error!");
		json_decref(jret);
		return;
	}
	strcpy(uuid, ruuid);
	strcpy(dev_number, rdev_number);
	strcpy(key, rkey);

	json_decref(jret);

	te.state = DS_REGISTER;
	log_info("register ok!");

}
static void tt_rpt_alarm(stTask_t *t) {
	//curl_http_post("http://101.132.153.33:5000/api/dev_dbm/report_alarm");
	log_info("[%d] \n", __LINE__);
}
static void tt_rpt_record(stTask_t *t) {
	//curl_http_post("http://101.132.153.33:5000/api/dev_dbm/report_access");
	log_info("[%d] \n", __LINE__);
}



