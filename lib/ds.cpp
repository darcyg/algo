#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds.h"

typedef struct stTableItem {
	const char *name;
	const char *type;
	int					len;
}stTableItem_t;

typedef struct stTableOperaion {
	const char		*tblname;
	int						itemcnt;
	stTableItem_t	*items;
	const char *create;
	const char *insert;
	const char *update;
	const char *search;
	const char *remove;
}stTableOperaion_t;

#define RECORD_BASEINFO(x)  \
		((stBasicInfo_t*)x)->uuid,\
		((stBasicInfo_t*)x)->reluuid,\
		((stBasicInfo_t*)x)->state,\
		((stBasicInfo_t*)x)->fwxxdz,\
		((stBasicInfo_t*)x)->area_code,\
		((stBasicInfo_t*)x)->areatype,\
		((stBasicInfo_t*)x)->ip,\
		((stBasicInfo_t*)x)->mac,\
		((stBasicInfo_t*)x)->sysversion,\
		((stBasicInfo_t*)x)->appversion,\
		((stBasicInfo_t*)x)->adminname,\
		((stBasicInfo_t*)x)->adminpass,\
		((stBasicInfo_t*)x)->capability,\
		((stBasicInfo_t*)x)->lurumode
static stTableItem_t baseinfo_items[] = {
	{"uuid",			"char", 32},
	{"reluuid",		"char", 32},
	{"state",			"int",	1},
	{"fwxxdz",		"char", 256},
	{"area_code",	"char", 60},
	{"areatype",	"int",	1},
	{"ip",				"char", 16},
	{"mac",				"char", 32},
	{"sysversion","char", 64},
	{"appversion","char", 64},
	{"adminname", "char", 32},
	{"adminpass", "char", 32},
	{"capability","int",	1},
	{"lurumode",	"int",	1},
};

#define RECORD_PERSON(x) \
		((stPerson_t*)x)->uuid,\
		((stPerson_t*)x)->state,\
		((stPerson_t*)x)->name,\
		((stPerson_t*)x)->sex,\
		((stPerson_t*)x)->idcard,\
		((stPerson_t*)x)->flow,\
		((stPerson_t*)x)->family,\
		((stPerson_t*)x)->owner,\
		((stPerson_t*)x)->patrol,\
		((stPerson_t*)x)->finger1,\
		((stPerson_t*)x)->finger2,\
		((stPerson_t*)x)->finger1type,\
		((stPerson_t*)x)->finger2type

static stTableItem_t person_items[] = {
	{"uuid",			"char",	32},
	{"state",			"int",	1},
	{"name",			"char", 50},
	{"sex",				"char",	1},
	{"idcard",		"char",	18},
	{"flow",			"char",	1},
	{"family",		"char",	1},
	{"owner",			"char", 1},
	{"patrol",		"char", 1},
	{"finger1",		"char",32},
	{"finger2",			"char",32},
	{"finger1type",	"int",1},
	{"finger2type",	"int",1},
};

#define RECORD_VCARD(x) \
	((stVcard_t*)x)->uuid,\
	((stVcard_t*)x)->vtype,\
	((stVcard_t*)x)->vcardid,\
	((stVcard_t*)x)->state,\
	((stVcard_t*)x)->stime,\
	((stVcard_t*)x)->etime
static stTableItem_t vcard_items[] = {
	{"uuid",			"char", 32},
	{"vtype",			"int",	1},
	{"vcardid",		"char",	32},
	{"state",			"int",	1},
	{"stime",			"int",	1},
	{"etime",			"int",	1},
};

#define RECORD_DEVICE(x) \
	((stDevice_t*)x)->uuid,\
	((stDevice_t*)x)->name,\
	((stDevice_t*)x)->mac,\
	((stDevice_t*)x)->type,\
	((stDevice_t*)x)->gw_uuid,\
	((stDevice_t*)x)->capability
static stTableItem_t device_items[] = {
	{"uuid",			"char",	32},
	{"name",			"char",	64},
	{"mac",				"char",	32},
	{"type",			"int",	1},
	{"gw_uuid",		"char",	32},
	{"capability","int",	1},
};

#define RECORD_VCARD_DEVICE(x) \
	((stVcardDevice_t*)x)->vcard_uuid,\
	((stVcardDevice_t*)x)->dev_uuid,\
	((stVcardDevice_t*)x)->state,\
	((stVcardDevice_t*)x)->stime,\
	((stVcardDevice_t*)x)->etime,\
	((stVcardDevice_t*)x)->capability
static stTableItem_t vcard_device_items[] = {
	{"vcard_uuid",	"char",	32},
	{"dev_uuid",		"char",	32},
	{"state",				"int",	1},
	{"stime",				"int",	1},
	{"etime",				"int",	1},
	{"capability",	"int",	1},
};

#define RECORD_VCARD_PERSON(x) \
	((stVcardPerson_t*)x)->vcard_uuid,\
	((stVcardPerson_t*)x)->person_uuid
static stTableItem_t vcard_person_items[] = {
	{"vcard_uuid",			"char",	32},
	{"person_uuid",			"char",	32},
};

#define RECORD_DEVICE_STATUS(x) \
	((stDeviceStatus_t*)x)->dev_uuid,\
	((stDeviceStatus_t*)x)->status,\
	((stDeviceStatus_t*)x)->hwversion,\
	((stDeviceStatus_t*)x)->sfversion,\
	((stDeviceStatus_t*)x)->battery,\
	((stDeviceStatus_t*)x)->temperature,\
	((stDeviceStatus_t*)x)->onoff,\
	((stDeviceStatus_t*)x)->workmode,\
	((stDeviceStatus_t*)x)->powermode
static stTableItem_t device_status_items[] = {
	{"dev_uuid",		"char",	32},
	{"status",			"int",	1},
	{"hwversion",		"char",	32},
	{"sfversion",		"char",	32},
	{"battery",			"int",	1},
	{"temperature",	"int",	1},
	{"onoff",				"int",	1},
	{"workmode",		"int",	1},
	{"powermode",		"int",	1},
};

#define RECORD_LOCK_RECORD(x) \
	((stLockRecord_t*)x)->vcard_uuid,\
	((stLockRecord_t*)x)->person_uuid,\
	((stLockRecord_t*)x)->timestamp,\
	((stLockRecord_t*)x)->mac,\
	((stLockRecord_t*)x)->opentype,\
	((stLockRecord_t*)x)->area_uuid,\
	((stLockRecord_t*)x)->vcardid,\
	((stLockRecord_t*)x)->area_code
static stTableItem_t lock_record_items[] = {
	{"vcard_uuid",	"char",	32},
	{"person_uuid",	"char",	32},
	{"timestamp",		"int",	1},
	{"mac",					"char",	32},
	{"opentype",		"int",	1},
	{"area_uuid",		"char",	32},
	{"vcardid",			"char",	32},
	{"area_code",		"char",	60},
};

#define RECORD_DEVICE_ALARM(x) \
	((stDeviceAlarm_t*)x)->dev_uuid,\
	((stDeviceAlarm_t*)x)->timestamp,\
	((stDeviceAlarm_t*)x)->mac,\
	((stDeviceAlarm_t*)x)->type,\
	((stDeviceAlarm_t*)x)->area_uuid,\
	((stDeviceAlarm_t*)x)->vcardid
static stTableItem_t device_alarm_items[] = {
	{"dev_uuid",		"char",	32},
	{"timestamp",		"int",	1},
	{"mac",					"char",	32},
	{"type",				"int",	1},
	{"area_uuid",		"char",	32},
	{"vcardid",			"char",	32},
};

#define RECORD_LOG(x) \
	((stLog_t*)x)->module,\
	((stLog_t*)x)->level,\
	((stLog_t*)x)->content
static stTableItem_t log_items[] = {
	{"module",			"char",	32},
	{"level",				"int",	1},
	{"content",			"char",	256},
};


#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])
static stTableOperaion_t tblops[] = {
	{"baseinfO",			ARRAY_SIZE(baseinfo_items),			baseinfo_items,			NULL, NULL, NULL, NULL, NULL},
	{"person",				ARRAY_SIZE(person_items),				person_items,				NULL, NULL, NULL, NULL, NULL},
	{"device",				ARRAY_SIZE(device_items),				device_items,				NULL, NULL, NULL, NULL, NULL},
	{"vcard",					ARRAY_SIZE(vcard_items),				vcard_items,				NULL, NULL, NULL, NULL, NULL},
	{"vcard_person_", ARRAY_SIZE(vcard_person_items),	vcard_person_items,	NULL, NULL, NULL, NULL, NULL},
	{"vcard_device_", ARRAY_SIZE(vcard_device_items),	vcard_device_items,	NULL, NULL, NULL, NULL, NULL},
	{"device_status", ARRAY_SIZE(device_status_items),device_status_items,NULL, NULL, NULL, NULL, NULL},
	{"lock_record",		ARRAY_SIZE(lock_record_items),	lock_record_items,	NULL, NULL, NULL, NULL, NULL},
	{"device_alarm",	ARRAY_SIZE(device_alarm_items),	device_alarm_items,	NULL, NULL, NULL, NULL, NULL},
	{"log",						ARRAY_SIZE(log_items),					log_items,					NULL, NULL, NULL, NULL, NULL},
};


#define SPRINTF(buf, op, tblname, record) do {\
	if (strcmp(tblname, "baseinfo") == 0) {\
		sprintf(buf, op, RECORD_BASEINFO(record));\
	} else if (strcmp(tblname, "person") == 0) {\
		sprintf(buf, op, RECORD_PERSON(record));\
	} else if (strcmp(tblname, "device") == 0) {\
		sprintf(buf, op, RECORD_DEVICE(record));\
	} else if (strcmp(tblname, "vcard") == 0) {\
		sprintf(buf, op, RECORD_VCARD(record));\
	} else if (strcmp(tblname, "vcard_person") == 0) {\
		sprintf(buf, op, RECORD_VCARD_PERSON(record));\
	} else if (strcmp(tblname, "vcard_device") == 0) {\
		sprintf(buf, op, RECORD_VCARD_DEVICE(record));\
	} else if (strcmp(tblname, "device_status") == 0) {\
		sprintf(buf, op, RECORD_DEVICE_STATUS(record));\
	} else if (strcmp(tblname, "lock_record") == 0) {\
		sprintf(buf, op, RECORD_LOCK_RECORD(record));\
	} else if (strcmp(tblname, "device_alarm") == 0) {\
		sprintf(buf, op, RECORD_DEVICE_ALARM(record));\
	} else if (strcmp(tblname, "log") == 0) {\
		sprintf(buf, op, RECORD_LOG(record));\
	} \
} while (0);


static stDataStorage_t ds = {
	"/etc/config/algo/algo.db",
};

static int ds_valid_db_pathname(const char *pathname) {
	return 0;
}

static int ds_db_exsit() {
	return 0;
}

static int ds_db_create() {
	return 0;
}

static int ds_db_open() {
	return 0;
}

static int ds_db_close() {
	return 0;
}

static int ds_db_prepare() {
	return 0;
}

static stTableOperaion_t *ds_search_table_operation(const char *tblname) {
	return NULL;
}

static int ds_sqlite3_exec(const char *buf) {
	return 0;
}



#define log_error(...)
#define log_warn(...)
#define log_info(...)

//////////////////////////////////////////////////////////////////////////

int ds_init(const char *pathname, int option) {
	if (!ds_valid_db_pathname(pathname)) {
		log_error("Invalid database name: %s\n", pathname);
		return -1;
	}

	strcpy(ds.pathname, pathname);
	memset(ds.records, 0, sizeof(ds.records));
	
	if (!ds_db_exsit()) {
		log_info("Create database : %s\n", ds.pathname);
		if (ds_db_create() != 0) {
			log_error("Can't create database: %s\n", ds.pathname);
			return -2;
		}
		log_info("Create database : %s success!\n", ds.pathname);
	}

	if (ds_db_open() != 0) {
		log_error("Open database failed: %s\n", ds.pathname);
		return -3;
	}

	ds_db_prepare();

	return 0;
}

int ds_create_table(const char *tblname) {
	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	sprintf(buf, to->create, tblname);

	int ret = ds_sqlite3_exec(buf);

	return ret;
}



int ds_insert_record(const char *tblname, stTableRecord_t *record) {
	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	SPRINTF(buf, to->insert, tblname, record);

	int ret = ds_sqlite3_exec(buf);

	return ret;
}

int ds_update_record(const char *tblname, stTableRecord_t *record) {
	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	SPRINTF(buf, to->update, tblname, record);

	int ret = ds_sqlite3_exec(buf);

	return ret;
}

int ds_delete_record(const char *tblname, const char *where) {
	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	sprintf(buf, to->remove, tblname, where);

	int ret = ds_sqlite3_exec(buf);

	return ret;
}

int ds_search_record(const char *tblname, 
										 int compare(stTableRecord_t *, void *), 
										 void *arg,
										 const char *where,
										 stTableRecord_t *retult) {
	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	sprintf(buf, to->search, tblname, where);

	int ret = ds_sqlite3_exec(buf);

	/* compare and fill result */

	return ret;
}


