#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "ds.h"
#include "sqlite3.h"

#define log_info(fmt, ...)	 printf(fmt, __VA_ARGS__)
#define log_warn(fmt, ...)	 printf(fmt, __VA_ARGS__)
#define log_err(fmt, ...)		 printf(fmt, __VA_ARGS__)
#define log_debug(fmt, ...)	 printf(fmt, __VA_ARGS__)
#define log_trace(fmt, ...)	 printf(fmt, __VA_ARGS__)


static int ds_create_table(const char *tblname);
static int ds_table_exsit(const char *tblname);

typedef struct stTableItem {
	const char *name;
	const char *type;
	int					len;
	int					primary;
}stTableItem_t;

typedef struct stTableOperaion {
	const char		*tblname;
	int						itemcnt;
	stTableItem_t	*items;
	char *create;
	char *insert;
	char *update;
	char *search;
	char *remove;
}stTableOperaion_t;

typedef struct stExecEnv {
	int		cbmode;
	void	*data;
	int		ret;
	void	*cb;
	void	*arg;
}stExecEnv_t;
typedef struct stDataStorage {
	char						pathname[256];

	void						*db;
	stTableRecord_t records[256];
}stDataStorage_t;


enum {
	CBMODE_CREATE = 0,
	CBMODE_INSERT = 1,
	CBMODE_UPDATE = 2,
	CBMODE_SEARCH = 3,
	CBMODE_DELETE = 4,
	CBMODE_EXIST	= 5,
	CBMODE_COUNT  = 6,
};


#define RECORD_BASEINFO(x)  \
		((stBasicInfo_t*)x)->uuid,\
		/*((stBasicInfo_t*)x)->reluuid,*/\
		((stBasicInfo_t*)x)->key,\
		((stBasicInfo_t*)x)->dev_number,\
		/*((stBasicInfo_t*)x)->state,*/\
		/*((stBasicInfo_t*)x)->fwxxdz,*/\
		/*((stBasicInfo_t*)x)->area_code,*/\
		/*((stBasicInfo_t*)x)->areatype,*/\
		((stBasicInfo_t*)x)->ip,\
		((stBasicInfo_t*)x)->mac,\
		((stBasicInfo_t*)x)->model,\
		((stBasicInfo_t*)x)->sysversion,\
		((stBasicInfo_t*)x)->appversion,\
		((stBasicInfo_t*)x)->adminname,\
		((stBasicInfo_t*)x)->adminpass,\
		((stBasicInfo_t*)x)->capability,\
		((stBasicInfo_t*)x)->lurumode
static stTableItem_t basicinfo_items[] = {
	{"uuid",			"char", 36, 1},
	//{"reluuid",		"char", 36, 0},
	//{"state",			"int",	1,	0},
	//{"fwxxdz",		"char", 256,0},
	//{"area_code",	"char", 60,	0},
	//{"areatype",	"int",	1,	0},
	{"key",				"char",	32,	0},
	{"dev_number","char", 36, 0},

	{"ip",				"char", 16,	0},
	{"mac",				"char", 32,	0},
	{"model",			"char", 64, 0},
	{"sysversion","char", 64,	0},
	{"appversion","char", 64,	0},
	{"adminname", "char", 32,	0},
	{"adminpass", "char", 32,	0},
	{"capability","int",	1,	0},
	{"lurumode",	"int",	1,	0},
};

#define RECORD_PERSON(x) \
		((stPerson_t*)x)->uuid,\
		/*((stPerson_t*)x)->state,*/\
		((stPerson_t*)x)->name,\
		((stPerson_t*)x)->sex
		/*((stPerson_t*)x)->idcard,*/\
		/*((stPerson_t*)x)->flow,*/\
		/*((stPerson_t*)x)->family,*/\
		/*((stPerson_t*)x)->owner,*/\
		/*((stPerson_t*)x)->patrol,*/\
		/*((stPerson_t*)x)->finger1,*/\
		/*((stPerson_t*)x)->finger2,*/\
		/*((stPerson_t*)x)->finger1type,*/\
		/*((stPerson_t*)x)->finger2type*/
static stTableItem_t person_items[] = {
	{"uuid",			"char",	36,	1},
	//{"state",			"int",	1,	0},
	{"name",			"char", 52,	0},
	{"sex",				"char",	1,	0},
	//{"idcard",		"char",	18,	0},
	//{"flow",			"char",	1,	0},
	//{"family",		"char",	1,	0},
	//{"owner",			"char", 1,	0},
	//{"patrol",		"char", 1,	0},
	//{"finger1",		"char",32,	0},
	//{"finger2",			"char",32,0},
	//{"finger1type",	"int",1,	0},
	//{"finger2type",	"int",1,	0},
};

#define RECORD_VCARD(x) \
	((stVcard_t*)x)->uuid,\
	((stVcard_t*)x)->vtype,\
	((stVcard_t*)x)->vcardid,\
	((stVcard_t*)x)->state,\
	((stVcard_t*)x)->stime,\
	((stVcard_t*)x)->etime
static stTableItem_t vcard_items[] = {
	{"uuid",			"char", 36,	1},
	{"vtype",			"int",	1,	0},
	{"vcardid",		"char",	52,	0},
	{"state",			"int",	1,	0},
	{"stime",			"char",	32,	0},
	{"etime",			"char",	32,	0},
};

/*
#define RECORD_DEVICE(x) \
	((stDevice_t*)x)->uuid,\
	((stDevice_t*)x)->name,\
	((stDevice_t*)x)->mac,\
	((stDevice_t*)x)->type,\
	((stDevice_t*)x)->gw_uuid,\
	((stDevice_t*)x)->capability
static stTableItem_t device_items[] = {
	{"uuid",			"char",	32,	1},
	{"name",			"char",	64,	0},
	{"mac",				"char",	32,	0},
	{"type",			"int",	1,	0},
	{"gw_uuid",		"char",	32,	0},
	{"capability","int",	1,	0},
};
*/

/*
#define RECORD_VCARD_DEVICE(x) \
	((stVcardDevice_t*)x)->vcard_uuid,\
	((stVcardDevice_t*)x)->dev_uuid,\
	((stVcardDevice_t*)x)->state,\
	((stVcardDevice_t*)x)->stime,\
	((stVcardDevice_t*)x)->etime,\
	((stVcardDevice_t*)x)->capability
static stTableItem_t vcard_device_items[] = {
	{"vcard_uuid",	"char",	32,	0},
	{"dev_uuid",		"char",	32,	0},
	{"state",				"int",	1,	0},
	{"stime",				"int",	1,	0},
	{"etime",				"int",	1,	0},
	{"capability",	"int",	1,	0},
};
*/

#define RECORD_VCARD_PERSON(x) \
	((stVcardPerson_t*)x)->vcard_uuid,\
	((stVcardPerson_t*)x)->person_uuid
static stTableItem_t vcard_person_items[] = {
	{"vcard_uuid",			"char",	36,	0},
	{"person_uuid",			"char",	36,	0},
};

#define RECORD_DEVICE_STATUS(x) \
	((stDeviceStatus_t*)x)->dev_uuid,\
	((stDeviceStatus_t*)x)->mac,\
	/*((stDeviceStatus_t*)x)->status,*/\
	((stDeviceStatus_t*)x)->hwversion,\
	((stDeviceStatus_t*)x)->sfversion,\
	((stDeviceStatus_t*)x)->imsi,\
	((stDeviceStatus_t*)x)->msisdn,\
	((stDeviceStatus_t*)x)->battery,\
	((stDeviceStatus_t*)x)->temperature,\
	((stDeviceStatus_t*)x)->signal,\
	((stDeviceStatus_t*)x)->card_capacity,\
	((stDeviceStatus_t*)x)->whitelist_count,\
	((stDeviceStatus_t*)x)->finger_capacity,\
	((stDeviceStatus_t*)x)->finger_count,\
	((stDeviceStatus_t*)x)->onoff,\
	((stDeviceStatus_t*)x)->workmode,\
	((stDeviceStatus_t*)x)->powermode
static stTableItem_t device_status_items[] = {
	{"dev_uuid",		"char",	36,	0},
	{"mac",					"char",	32, 0},
	//{"status",			"int",	1,	0},
	{"hwversion",		"char",	32,	0},
	{"sfversion",		"char",	32,	0},
	{"imsi",				"char", 32, 0},
	{"msisdn",			"char", 32, 0},
	{"battery",			"int",	1,	0},
	{"temperature",	"int",	1,	0},
	{"signal",					"int",	1,	0},
	{"card_capacity",		"int",	1,	0},
	{"whitelist_count",	"int",	1,	0},
	{"finger_capacity",	"int",	1,	0},
	{"finger_count",		"int",	1,	0},
	{"onoff",					"int",	1,	0},
	{"workmode",		"int",	1,	0},
	{"powermode",		"int",	1,	0},
};

#define RECORD_LOCK_RECORD(x) \
	/*((stLockRecord_t*)x)->vcard_uuid,*/\
	/*((stLockRecord_t*)x)->person_uuid,*/\
	/*((stLockRecord_t*)x)->timestamp,*/\
	((stLockRecord_t*)x)->vcardid,\
	((stLockRecord_t*)x)->mac,\
	((stLockRecord_t*)x)->opentype,\
	((stLockRecord_t*)x)->slide_date,\
	((stLockRecord_t*)x)->dev_uuid,\
	((stLockRecord_t*)x)->dev_number
	/*((stLockRecord_t*)x)->area_uuid,*/\
	/*((stLockRecord_t*)x)->vcardid,*/\
	/*((stLockRecord_t*)x)->area_code*/
static stTableItem_t lock_record_items[] = {
	{"vcardid",			"char",	36,	0},
	{"mac",					"char",	32,	0},
	{"opentype",		"int",	1,	0},
	{"slide_date",	"char",	32, 0},
	{"dev_uuid",		"char", 36, 0},
	{"dev_number",	"char", 36, 0},
	//{"vcard_uuid",	"char",	32,	0},
	//{"person_uuid",	"char",	32,	0},
	//{"timestamp",		"int",	1,	0},
	//{"area_uuid",		"char",	32,	0},
	//{"area_code",		"char",	60,	0},
};

#define RECORD_DEVICE_ALARM(x) \
	((stDeviceAlarm_t*)x)->occur_date,\
	((stDeviceAlarm_t*)x)->type,\
	((stDeviceAlarm_t*)x)->mac,\
	((stDeviceAlarm_t*)x)->dev_uuid,\
	((stDeviceAlarm_t*)x)->vcardid
	/*((stDeviceAlarm_t*)x)->slide_date,*/\
	/*((stDeviceAlarm_t*)x)->dev_number,*/\
	/*((stDeviceAlarm_t*)x)->dev_uuid,*/\
	/*((stDeviceAlarm_t*)x)->timestamp,*/\
	/*((stDeviceAlarm_t*)x)->area_uuid,*/
static stTableItem_t device_alarm_items[] = {
	{"occur_date",	"char",	32, 0},
	{"type",				"int",	1,	0},
	{"mac",					"char",	32,	0},
	{"dev_uuid",		"char",	36,	0},
	{"vcardid",			"char",	36,	0},
	//{"timestamp",		"int",	1,	0},
	//{"area_uuid",		"char",	32,	0},
	//{"dev_number",	"char",	36, 0},
};

#define RECORD_LOG(x) \
	((stLog_t*)x)->timestamp,\
	((stLog_t*)x)->module,\
	((stLog_t*)x)->level,\
	((stLog_t*)x)->content
static stTableItem_t log_items[] = {
	{"timestamp",		"int",	1,		0},
	{"module",			"char",	32,		0},
	{"level",				"int",	1,		0},
	{"content",			"char",	256,	0},
};


#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])
static stTableOperaion_t tblops[] = {
	{"basicinfo",			ARRAY_SIZE(basicinfo_items),			basicinfo_items,			NULL, NULL, NULL, NULL, NULL},
	{"person",				ARRAY_SIZE(person_items),				person_items,				NULL, NULL, NULL, NULL, NULL},
	//{"device",				ARRAY_SIZE(device_items),				device_items,				NULL, NULL, NULL, NULL, NULL},
	{"vcard",					ARRAY_SIZE(vcard_items),				vcard_items,				NULL, NULL, NULL, NULL, NULL},
	{"vcard_person", ARRAY_SIZE(vcard_person_items),	vcard_person_items,	NULL, NULL, NULL, NULL, NULL},
	//{"vcard_device", ARRAY_SIZE(vcard_device_items),	vcard_device_items,	NULL, NULL, NULL, NULL, NULL},
	{"device_status", ARRAY_SIZE(device_status_items),device_status_items,NULL, NULL, NULL, NULL, NULL},
	{"lock_record",		ARRAY_SIZE(lock_record_items),	lock_record_items,	NULL, NULL, NULL, NULL, NULL},
	{"device_alarm",	ARRAY_SIZE(device_alarm_items),	device_alarm_items,	NULL, NULL, NULL, NULL, NULL},
	{"log",						ARRAY_SIZE(log_items),					log_items,					NULL, NULL, NULL, NULL, NULL},
};


#define SPRINTF(buf, op, tblname, record) do {\
	if (strcmp(tblname, "basicinfo") == 0) {\
		sprintf(buf, op, RECORD_BASEINFO(record));\
	} else if (strcmp(tblname, "person") == 0) {\
		sprintf(buf, op, RECORD_PERSON(record));\
	/*} else if (strcmp(tblname, "device") == 0) {*/\
		/*sprintf(buf, op, RECORD_DEVICE(record));*/\
	} else if (strcmp(tblname, "vcard") == 0) {\
		sprintf(buf, op, RECORD_VCARD(record));\
	} else if (strcmp(tblname, "vcard_person") == 0) {\
		sprintf(buf, op, RECORD_VCARD_PERSON(record));\
	/*} else if (strcmp(tblname, "vcard_device") == 0) {*/\
		/*sprintf(buf, op, RECORD_VCARD_DEVICE(record));*/\
	} else if (strcmp(tblname, "device_status") == 0) {\
		sprintf(buf, op, RECORD_DEVICE_STATUS(record));\
	} else if (strcmp(tblname, "lock_record") == 0) {\
		sprintf(buf, op, RECORD_LOCK_RECORD(record));\
	} else if (strcmp(tblname, "device_alarm") == 0) {\
		sprintf(buf, op, RECORD_DEVICE_ALARM(record));\
	/*} else if (strcmp(tblname, "log") == 0) {*/\
		/*sprintf(buf, op, RECORD_LOG(record));*/\ 
	}\
} while (0);


static stDataStorage_t ds = {
	"/etc/config/algo/algo.db",
};

static int ds_valid_db_pathname(const char *pathname) {
	return 1;
}

static int ds_db_exsit() {
	if (access(ds.pathname, F_OK) != 0) {
		return 0;
	}
	return 1;
}

/*
static int ds_db_create() {
	sqlite3 **pdb = (sqlite3**)&ds.db;
	int rc = sqlite3_open(ds.pathname, pdb);
	if (rc) {
		log_err("Can't open database : %s\n", sqlite3_errmsg(*pdb));
		sqlite3_close(*pdb);
		return -1;
	}
	return 0;
}
*/

static int ds_db_open() {
	sqlite3 **pdb = (sqlite3**)&ds.db;
	int rc = sqlite3_open(ds.pathname, pdb);
	if (rc) {
		log_err("Can't open database : %s\n", sqlite3_errmsg(*pdb));
		sqlite3_close(*pdb);
		return -1;
	}
	return 0;
}

static int ds_db_close() {
	sqlite3_close((sqlite3*)ds.db);
	return 0;
}

static int ds_db_prepare(stTableRecord_t *tr) {
	int count = ARRAY_SIZE(tblops);
	int i = 0;
	for (i = 0; i < count; i++) {
		if (!ds_table_exsit(tblops[i].tblname)) {
			ds_create_table(tblops[i].tblname);

			if (strcmp(tblops[i].tblname, "basicinfo") == 0 && tr != NULL) {
				ds_insert_record("basicinfo",  tr);
			}
		}
	}

	return 0;
}

static stTableOperaion_t *ds_search_table_operation(const char *tblname) {
	int cnt = ARRAY_SIZE(tblops);
	int i = 0;
	for (i = 0; i < cnt; i++) {
		stTableOperaion_t *to = &tblops[i];
		if (strcmp(to->tblname, tblname) == 0) {
			return to;
		}
	}
	return NULL;
}

static int ds_sqlite3_exec_callback(void *data, int argc, char **argv, char **azColName) {
	stExecEnv_t *ee = (stExecEnv_t*)data;

	int i;
	for	(i=0; i	<	argc; i++)	{
		//log_info("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	//log_info("%s", "\n");

	switch (ee->cbmode) {
	case CBMODE_EXIST:
		if (argc == 0) {
			ee->ret = 0;
		} else {
			ee->ret = 1;
		}
		break;
	case CBMODE_CREATE:
		break;
	case CBMODE_INSERT:
		break;
	case CBMODE_DELETE:
		break;
	case CBMODE_SEARCH:
		{
			stTableRecord_t tr;
			char *ptr = (char *)&tr;
			stTableOperaion_t *to = (stTableOperaion_t*)ee->data;
			int i= 0;
			for (i = 0; i < argc; i++) {
				stTableItem_t *ti = &to->items[i];
				//log_trace("parse %s, %s\n", ti->name, argv[i]);
				if (strcmp(ti->type, "char") == 0) {
					if (ti->len == 1) {
						int x = 0;
						sscanf(argv[i], "%d", &x);	

						*ptr = x&0xff;

						ptr += ti->len;
					} else {
						strcpy(ptr, argv[i]);

						ptr += ti->len;
					}
				} else if (strcmp(ti->type, "int") == 0) {
					if (ti->len == 1) {
						int x = 0;
						sscanf(argv[i], "%d", &x);	
						*(int*)ptr = x;

						ptr += ti->len * 4;
					} else {
						;;
						ptr += ti->len * 4;
					}
				} 
			}

			if (ee->cb != NULL) {
				((SEARCH_CB)(ee->cb))(&tr, ee->arg);
			}
		}
		break;
	case CBMODE_COUNT:
		ee->ret = atoi(argv[0]);
		break;
	default:
		break;
	}
	return 0;
}

static int ds_sqlite3_exec(const char *buf, void *data) {
	char *zErrMsg = NULL;

	log_debug("[%s] %s\n", __func__, buf);

	int rc	= sqlite3_exec((sqlite3 *)ds.db, buf, ds_sqlite3_exec_callback, data, &zErrMsg);

	if (rc != SQLITE_OK) {
		log_err("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	return rc;
}

static int ds_table_exsit(const char *tblname) {
	char buf[512];
	sprintf(buf, "select name from sqlite_master where type='table' AND name='%s'", tblname);

	stExecEnv_t ee = { CBMODE_EXIST, NULL, -1, NULL, NULL};

	int rc = 	ds_sqlite3_exec(buf, &ee);

	//log_debug("[%s] [%d] rc : %d\n", __func__, __LINE__, rc);

	if (rc == 0 && ee.ret == 1) {
		//log_debug("[%s] [%d] table(%s) has exsit.\n", __func__, __LINE__, tblname);
		return 1;
	}

	return 0;
}



static int ds_fill_table_operation_create(stTableOperaion_t *to) {
	int itemcnt = to->itemcnt;
	int i = 0;
	char buf[512];
	int len = 0;
	
	len += sprintf(buf + len, "create table %s(", to->tblname);
	for (i = 0; i < itemcnt; i++) {
		stTableItem_t *ti = &to->items[i];
		len += sprintf(buf + len, "%s %s", ti->name, ti->type);
		if (ti->len > 1) {
			len += sprintf(buf + len, "(%d)", ti->len);
		}
		if (ti->primary) {
			len += sprintf(buf + len, " primary key");
		}
		if (i < itemcnt - 1) {
			len += sprintf(buf + len, ",");
		}
	}
	len += sprintf(buf + len, ");");

	to->create = (char*)malloc(len + 1);
	strcpy(to->create, buf);

	return 0;
}
static int ds_fill_table_operation_insert(stTableOperaion_t *to) {
	int itemcnt = to->itemcnt;
	int i = 0;
	char buf[512];
	int len = 0;
	
	len += sprintf(buf + len, "insert into %s(", to->tblname);
	for (i = 0; i < itemcnt; i++) {
		stTableItem_t *ti = &to->items[i];
		len += sprintf(buf + len, "%s", ti->name);
		if (i < itemcnt - 1) {
			len += sprintf(buf + len, ",");
		}
	}
	len += sprintf(buf + len, ") values(");
	for (i = 0; i < itemcnt; i++) {
		stTableItem_t *ti = &to->items[i];
		if (strcmp(ti->type, "char") == 0) {
			if (ti->len == 1) {
				len += sprintf(buf + len, "%%d");
			} else {
				len += sprintf(buf + len, "'%%s'");
			}
		} else if (strcmp(ti->type, "int") == 0) {
			if (ti->len == 1) {
				len += sprintf(buf + len, "%%d");
			} else {
				log_warn("not support integer type len : %d\n", ti->len);
			}
		}
		if (i < itemcnt - 1) {
			len += sprintf(buf + len, ",");
		}
	}
	len += sprintf(buf + len, ");");

	to->insert = (char*)malloc(len + 1);
	strcpy(to->insert, buf);

	return 0;
}
static int ds_fill_table_operation_update(stTableOperaion_t *to) {
	int itemcnt = to->itemcnt;
	int i = 0;
	char buf[512];
	int len = 0;
	
	len += sprintf(buf + len, "update %s set ", to->tblname);
	for (i = 0; i < itemcnt; i++) {
		stTableItem_t *ti = &to->items[i];
		len += sprintf(buf + len, "%s = ", ti->name);

		if (strcmp(ti->type, "char") == 0) {
			if (ti->len == 1) {
				len += sprintf(buf + len, "%%d");
			} else {
				len += sprintf(buf + len, "'%%s'");
			}
		} else if (strcmp(ti->type, "int") == 0) {
			if (ti->len == 1) {
				len += sprintf(buf + len, "%%d");
			} else {
				log_warn("not support integer type len : %d\n", ti->len);
			}
		}

		if (i < itemcnt - 1) {
			len += sprintf(buf + len, ",");
		}
	}
	len += sprintf(buf + len, " ");
	//len += sprintf(buf + len, " where %%s;");

	to->update = (char *)malloc(len + 1);
	strcpy(to->update, buf);

	return 0;
}
static int ds_fill_table_operation_search(stTableOperaion_t *to) {
	char buf[512];
	int len = 0;
	
	//len += sprintf(buf + len, "select * from %s where %%s;", to->tblname);
	len += sprintf(buf + len, "select * from %s ", to->tblname);
	
	to->search = (char*)malloc(len + 1);
	strcpy(to->search, buf);

	return 0;

}
static int ds_fill_table_operation_remove(stTableOperaion_t *to) {
	char buf[512];
	int len = 0;
	
	//len += sprintf(buf + len, "delete from %s where %%s;", to->tblname);
	len += sprintf(buf + len, "delete from %s ", to->tblname);
	
	to->remove = (char*)malloc(len + 1);
	strcpy(to->remove, buf);

	return 0;
}

static int ds_fill_table_operation() {
	int cnt = ARRAY_SIZE(tblops);
	int i = 0;
	for (i = 0; i < cnt; i++) {
		stTableOperaion_t *to = &tblops[i];

		ds_fill_table_operation_create(to);
		ds_fill_table_operation_insert(to);
		ds_fill_table_operation_update(to);
		ds_fill_table_operation_search(to);
		ds_fill_table_operation_remove(to);

	}

	return 0;
}

static void ds_view_table_operation() {
	int cnt = ARRAY_SIZE(tblops);
	int i = 0;
	for (i = 0; i < cnt; i++) {
		stTableOperaion_t *to = &tblops[i];

		printf("Table [%s]:\n", to->tblname);
		printf("  [create]: %s\n", to->create);
		printf("  [insert]: %s\n", to->insert);
		printf("  [update]: %s\n", to->update);
		printf("  [search]: %s\n", to->search);
		printf("  [remove]: %s\n", to->remove);
	}
}



//////////////////////////////////////////////////////////////////////////
int ds_free() {
	ds_db_close();
	return 0;
}

int ds_init(const char *pathname, int option, stTableRecord_t *tr) {

	if (!ds_valid_db_pathname(pathname)) {
		log_err("Invalid database name: %s\n", pathname);
		return -1;
	}

	strcpy(ds.pathname, pathname);
	memset(ds.records, 0, sizeof(ds.records));
	
	/*
	if (!ds_db_exsit()) {
		log_info("Create database : %s\n", ds.pathname);
		if (ds_db_create() != 0) {
			log_err("Can't create database: %s\n", ds.pathname);
			return -2;
		}
		log_info("Create database : %s success!\n", ds.pathname);
	}
	*/

	if (ds_db_open() != 0) {
		log_err("Open database failed: %s\n", ds.pathname);
		return -3;
	}

	ds_fill_table_operation();

	//ds_view_table_operation();

	ds_db_prepare(tr);

	return 0;
}

int ds_create_table(const char *tblname) {
	//log_debug("[%s] [%d] <%s>\n", __func__, __LINE__, tblname);

	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	sprintf(buf, to->create, tblname);

	stExecEnv_t ee = { CBMODE_CREATE, to, -1, NULL, NULL};
	int ret = ds_sqlite3_exec(buf, &ee);

	return ret;
}



int ds_insert_record(const char *tblname, stTableRecord_t *record) {
	//log_debug("[%s] ]%d] <%s>\n", __func__, __LINE__, tblname);

	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	SPRINTF(buf, to->insert, tblname, record);

	stExecEnv_t ee = { CBMODE_INSERT, to, -1, NULL, NULL};
	int ret = ds_sqlite3_exec(buf, &ee);

	return ret;
}

int ds_update_record(const char *tblname, stTableRecord_t *record, const char *where, ...) {
	//log_debug("[%s] ]%d] <%s>\n", __func__, __LINE__, tblname);

	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	SPRINTF(buf, to->update, tblname, record);

	char wbuf[256];
	va_list ap;
	va_start(ap, where);
	vsprintf(wbuf, where, ap);
	//log_trace("wbuf is %s\n", wbuf);
	va_end(ap);

	//sprintf(buf + strlen(buf), "where %s;", where);
	sprintf(buf + strlen(buf), "%s;", wbuf);

	stExecEnv_t ee = { CBMODE_UPDATE, to, -1, NULL, NULL};
	int ret = ds_sqlite3_exec(buf, &ee);

	return ret;
}

int ds_delete_record(const char *tblname, const char *where, ...) {
	//log_debug("[%s] ]%d] <%s>\n", __func__, __LINE__, tblname);

	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	sprintf(buf, to->remove, tblname);

	char wbuf[256];
	va_list ap;
	va_start(ap, where);
	vsprintf(wbuf, where, ap);
	//log_trace("wbuf is %s\n", wbuf);
	va_end(ap);

	//sprintf(buf + strlen(buf), "where %s;", where);
	sprintf(buf + strlen(buf), "%s;", wbuf);

	stExecEnv_t ee = { CBMODE_DELETE, to, -1, NULL, NULL};
	int ret = ds_sqlite3_exec(buf, &ee);

	return ret;
}

int ds_search_record(const char *tblname, 
										 SEARCH_CB callback,
										 void *arg,
										 const char *where,
										 ...) {
	//log_debug("[%s] ]%d] <%s>\n", __func__, __LINE__, tblname);

	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}

	char buf[512];
	sprintf(buf, to->search, tblname);

	char wbuf[256];
	va_list ap;
	va_start(ap, where);
	vsprintf(wbuf, where, ap);
	//log_trace("wbuf is %s\n", wbuf);
	va_end(ap);

	//sprintf(buf + strlen(buf), "where %s;", where);
	sprintf(buf + strlen(buf), "%s;", wbuf);

	stExecEnv_t ee = { CBMODE_SEARCH, to, -1, (void*)callback, arg};
	int ret = ds_sqlite3_exec(buf, &ee);

	/* compare and fill result */

	return ret;
}


int ds_table_info(const char *tblname, stTableInfo_t *ti) {
	int i = 0;
	int cnt = sizeof(tblops)/sizeof(tblops[0]);
	for (i = 0; i < cnt; i++) {
		stTableOperaion_t *to = &tblops[i];
		if (strcmp(tblname, to->tblname) == 0) {
			ti->tblname = to->tblname;
			ti->itemcnt = to->itemcnt;
			ti->items = (stItemInfo_t*)to->items;
			return 0;
		}
	}

	return -1;
}


int ds_table_total_record_num(const char *tblname) {
	stTableOperaion_t *to = ds_search_table_operation(tblname);
	if (to == NULL) {
		log_warn("Invalid table name : %s\n", tblname);
		return -1;
	}


	char buf[512];
	sprintf(buf, "select count(*) from %s", tblname);

	stExecEnv_t ee = { CBMODE_COUNT, to, -1, NULL, 0};
	int ret = ds_sqlite3_exec(buf, &ee);
	if (ret != 0) {
		return 0;
	}

	return ee.ret;
}

