#include "task.h"

#include "jansson.h"
#include "json_parser.h"
#include "cloud.h"
#include "ds.h"

static stTaskEnv_t te = {
	.fet = NULL,
	.th = NULL,
	.state = DS_UNREGISTER,
};

static void tt_register_handler(stTask_t *t);
static void tt_rpt_dev_status(stTask_t *t);
static void tt_rpt_alarm(stTask_t *t);
static void tt_rpt_record(stTask_t *t);
static char *tt_util_int2str(int x);



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
	stTask_t *t = MALLOC(sizeof(stTask_t) + len);
	if (t == NULL) {
		 return ;
	}
	t->type = type;
	t->data = (void *)(t + 1);
	t->len = len;
	if (t->len > 0) {
		memcpy(t->data, data, len);
	}

	lockqueue_push(&te.msgq, t);

	task_step();
}
void task_run(struct timer *timer) {
	stTask_t *t = NULL;
	if (lockqueue_pop(&te.msgq, (void **)&t) && t != NULL) {
		log_info("[Task]:====================================>");
		task_handler_task(t, ths, sizeof(ths)/sizeof(ths[0]));
		FREE(t);
		task_step();
	}
}
void stat_run(struct timer *timer) {
	//timer_cancel(te.th, &env.stat_timer);

	emTaskType_t t = 0;
	int delt = 0;

	if (te.state == DS_UNREGISTER) {
		t = TT_REGISTER;
		delt = 1000*30;

		stTableRecord_t tr;
		int ret = ds_search_record_simple("basicinfo", &tr, "limit 0, 1");
		if (ret != 1) {
			log_info("Can't Search baseinfo");
		} else {
			stRegister_t r;
			strcpy(r.mac, tr.basic.mac);
			strcpy(r.uuid, tr.basic.uuid);
			task_push(t, (void *)&r, sizeof(stRegister_t));
		}
	} else {

		t = TT_RPT_DEV_STATUS;
		//delt = 1000*60*5;
		delt = 1000*60;

		stTableRecord_t tr;
		int ret = ds_search_record_simple("basicinfo", &tr, "limit 0, 1");
		if (ret != 1) {
			log_info("Can't Search baseinfo");
		} else {
			stStatus_t s;
			memset(&s, 0, sizeof(s));
			strcpy(s.mac, tr.basic.mac);
			strcpy(s.dev_uuid, tr.basic.uuid);

			task_push(t, (void *)&s, sizeof(stStatus_t)); 
		}
	}
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
	log_info("[%s]...", __func__);

	//const char *url = "http://101.132.153.33:5000/api/dev_dbm/register_device";
	const char *url = "http://101.132.153.33/api/dev_dbm/register_device";

	json_t *jarg = json_object();
	if (jarg == NULL) {
		log_warn("[%s] no memory!", __func__);
		return;
	}

	stRegister_t *s = (stRegister_t*)t->data;
#if 0
	static char mac[32]  = "0102030405060708";
	static char uuid[36] = "48473644ea594e9789c447526a50de8d";
	static char dev_number[36];
	static char key[32];
#endif

	json_object_set_new(jarg, "mac",	json_string(s->mac));
	json_object_set_new(jarg, "uuid", json_string(s->uuid));
	char *sarg = json_dumps(jarg, 0);
	if (sarg == NULL) {
		log_warn("[%s] no memory!", __func__);
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
		log_warn("[%s] failed : no response!", __func__);
		return;
	}

	log_info("resp:\n\tlen:[%d]\n\tcontent:[%s]", cr.len, (char*)cr.data);

	json_error_t error;
	json_t *jret = json_loads(cr.data, 0, &error);
	if (jret == NULL) {
		log_warn("[%s ] failed : response is not json!", __func__);
		return;
	}
	int status = -1;			 
	if (json_get_int(jret, "status", &status) != 0) {
		log_warn("[%s] failed : no status", __func__);
		json_decref(jret);
		return;
	}
	if (status != 0) {
		log_warn("[%s] failed : %s", __func__, osa_get_error_string(status));
		json_decref(jret);
		return;
	}
		
	json_t *jpayload = json_object_get(jret, "payload");
	if (jpayload == NULL) {
		log_warn("[%s ] failed : response does not have payload!", __func__);
		json_decref(jret);
		return;
	}

	const char *rmac				= json_get_string(jpayload, "mac");
	const char *ruuid				= json_get_string(jpayload, "uuid");
	const char *rdev_number	= json_get_string(jpayload, "dev_number");
	const char *rkey				= json_get_string(jpayload, "key");

	if (rmac == NULL || ruuid == NULL || rdev_number == NULL || rkey == NULL) {
		log_warn("[%s] failed argments error!", __func__);
		json_decref(jret);
		return;
	}

	//log_debug("ruuid:%s, rdev_number:%s, rkey:%s, s->mac:%s\n", ruuid, rdev_number, rkey, s->mac);
	if (ds_update_record_simple("basicinfo", "set uuid = '%s', dev_number = '%s', key = '%s' where mac = '%s'",
				ruuid, rdev_number, rkey, s->mac) != 0) {
		log_warn("[%s] update basicinfo failed!", __func__);
		json_decref(jret);
		return;
	}

	te.state = DS_REGISTER;
	json_decref(jret);
	log_info("[%s] ok!", __func__);
}
static void tt_rpt_dev_status(stTask_t *t) {
	log_info("[%s]...", __func__);

	//const char *url = "http://101.132.153.33:5000/api/dev_dbm/report_device_status";
	const char *url = "http://101.132.153.33/api/dev_dbm/report_device_status";

	json_t *jarg = json_object();
	if (jarg == NULL) {
		log_warn("[%s] no memory!", __func__);
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
	json_object_set_new(jarg, "battery", json_string(tt_util_int2str(s->battery)));
	json_object_set_new(jarg, "temperature", json_string(tt_util_int2str(s->temperature)));
	json_object_set_new(jarg, "signal_", json_string(tt_util_int2str(s->signal_)));
	json_object_set_new(jarg, "card_capacity", json_string(tt_util_int2str(s->card_capacity)));
	json_object_set_new(jarg, "whitelist_count", json_string(tt_util_int2str(s->whitelist_count)));
	json_object_set_new(jarg, "finger_capacity", json_string(tt_util_int2str(s->finger_capacity)));
	json_object_set_new(jarg, "finger_count", json_string(tt_util_int2str(s->finger_count)));
	json_object_set_new(jarg, "opened", json_string(tt_util_int2str(s->opened)));
	json_object_set_new(jarg, "work_mode", json_string(tt_util_int2str(s->work_mode)));
	json_object_set_new(jarg, "power_mode", json_string(tt_util_int2str(s->power_mode)));
	char *sarg = json_dumps(jarg, 0);
	if (sarg == NULL) {
		log_warn("[%s] no memory!", __func__);
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
		log_warn("[%s] failed : no response!", __func__);
		return;
	}

	log_info("resp:\n\tlen:[%d]\n\tcontent:[%s]", cr.len, (char*)cr.data);

	json_error_t error;
	json_t *jret = json_loads(cr.data, 0, &error);
	if (jret == NULL) {
		log_warn("[%s ] failed : response is not json!", __func__);
		return;
	}
	int status = -1;			 
	if (json_get_int(jret, "status", &status) != 0) {
		log_warn("[%s] failed : no status", __func__);
		json_decref(jret);
		return;
	}
	if (status != 0) {
		log_warn("[%s] failed : %s", __func__, osa_get_error_string(status));
		json_decref(jret);
		return;
	}
		
	json_decref(jret);

	log_info("[%s] ok!", __func__);
}
static void tt_rpt_alarm(stTask_t *t) {
	log_info("[%s]...", __func__);

	//const char *url = "http://101.132.153.33:5000/api/dev_dbm/report_alarm";
	const char *url = "http://101.132.153.33/api/dev_dbm/report_alarm";

	json_t *jarg = json_object();
	if (jarg == NULL) {
		log_warn("[%s] no memory!", __func__);
		return;
	}

	stAlarm_t *s = (stAlarm_t*)t->data;
	/*
	stTableRecord tr;
	char *tblname = "basicinfo";
	stReq_t r = {server, req, tblname, NULL};
	ds_search_record(tblname, db_test_search_status, &r, "limit 0, 1");
	*/

	json_object_set_new(jarg, "occur_date",		json_string(s->occur_date));
	json_object_set_new(jarg, "type_",				json_string(tt_util_int2str(s->type_)));
	json_object_set_new(jarg, "mac",					json_string(s->mac));
	json_object_set_new(jarg, "device_uuid",	json_string(s->device_uuid));
	json_object_set_new(jarg, "cardno",				json_string(s->cardno));

	char *sarg = json_dumps(jarg, 0);
	if (sarg == NULL) {
		log_warn("[%s] no memory!", __func__);
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
		log_warn("[%s] failed : no response!", __func__);
		return;
	}

	log_info("resp:\n\tlen:[%d]\n\tcontent:[%s]", cr.len, (char*)cr.data);

	json_error_t error;
	json_t *jret = json_loads(cr.data, 0, &error);
	if (jret == NULL) {
		log_warn("[%s ] failed : response is not json!", __func__);
		return;
	}
	int status = -1;			 
	if (json_get_int(jret, "status", &status) != 0) {
		log_warn("[%s] failed : no status", __func__);
		json_decref(jret);
		return;
	}
	if (status != 0) {
		log_warn("[%s] failed : %s", __func__, osa_get_error_string(status));
		json_decref(jret);
		return;
	}
		
	json_decref(jret);

	log_info("[%s] ok!", __func__);

}
static void tt_rpt_record(stTask_t *t) {
	log_info("[%s]...", __func__);

	//const char *url = "http://101.132.153.33:5000/api/dev_dbm/report_access";
	const char *url = "http://101.132.153.33/api/dev_dbm/report_access";

	json_t *jarg = json_object();
	if (jarg == NULL) {
		log_warn("[%s] no memory!", __func__);
		return;
	}

	stAccessRecord_t *s = (stAccessRecord_t*)t->data;
	/*
	stTableRecord tr;
	char *tblname = "basicinfo";
	stReq_t r = {server, req, tblname, NULL};
	ds_search_record(tblname, db_test_search_status, &r, "limit 0, 1");
	*/

	json_object_set_new(jarg, "cardno",			json_string(s->cardno));
	json_object_set_new(jarg, "mac",				json_string(s->mac));
	json_object_set_new(jarg, "opentype",		json_string(tt_util_int2str(s->opentype)));
	json_object_set_new(jarg, "slide_date", json_string(s->slide_date));
	json_object_set_new(jarg, "dev_uuid",		json_string(s->dev_uuid));
	json_object_set_new(jarg, "dev_number", json_string(s->dev_number));

	char *sarg = json_dumps(jarg, 0);
	if (sarg == NULL) {
		log_warn("[%s] no memory!", __func__);
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
		log_warn("[%s] failed : no response!", __func__);
		return;
	}

	log_info("resp:\n\tlen:[%d]\n\tcontent:[%s]", cr.len, (char*)cr.data);

	json_error_t error;
	json_t *jret = json_loads(cr.data, 0, &error);
	if (jret == NULL) {
		log_warn("[%s ] failed : response is not json!", __func__);
		return;
	}
	int status = -1;			 
	if (json_get_int(jret, "status", &status) != 0) {
		log_warn("[%s] failed : no status", __func__);
		json_decref(jret);
		return;
	}
	if (status != 0) {
		log_warn("[%s] failed : %s", __func__, osa_get_error_string(status));
		json_decref(jret);
		return;
	}
		
	json_decref(jret);

	log_info("[%s] ok!", __func__);

}



static char *tt_util_int2str(int x) {
	static char sss[256];
	sprintf(sss, "%d", x);
	return sss;
}

