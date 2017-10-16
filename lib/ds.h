#ifndef __DS_H_
#define __DS_H_

#include "sqlite3.h"

#ifdef __cplusplus 
extern "C" {
#endif


typedef struct stBasicInfo {
	char	uuid[32];
	char	reluuid[32];
	int		state;
	char	fwxxdz[256];
	char	area_code[60];
	int		areatype;
	char	ip[16];
	char	mac[32];
	char	model[64];
	char	sysversion[64];
	char	appversion[64];
	char	adminname[32];
	char	adminpass[32];
	int		capability;
	int		lurumode;
}stBasicInfo_t;

typedef struct stPerson {
	char	uuid[32];
	int		state;
	char		name[50];
	char	sex;
	char	idcard[18];
	char	photopath[256];
	char	flow;
	char	family;
	char	owner;
	char	patrol;
	char	finger1[32];
	char	finger2[32];
	int		finger1type;
	int		finger2type;
}stPerson_t;

typedef struct stVcard {
	char	uuid[32];
	int		vtype;
	char	vcardid[32];
	int		state;
	int		stime;
	int		etime;
} stVcard_t;

typedef struct stDevice {
	char	uuid[32];
	char	name[64];
	char	mac[32];
	int		type;
	char	gw_uuid[32];
	int		capability;
}stDevice_t;

typedef struct stVcardDevice {
	char	vcard_uuid[32];
	char	dev_uuid[32];
	int		state;
	int		stime;
	int		etime;
	int		capability;
}stVcardDevice_t;

typedef struct stVcardPerson {
	char	vcard_uuid[32];
	char	person_uuid[32];
}stVcardPerson_t;

typedef struct stDeviceStatus {
	char	dev_uuid[32];
	int		status;
	char	hwversion[32];
	char	sfversion[32];
	int		battery;
	int		temperature;
	int		onoff;
	int		workmode;
	int		powermode;
}stDeviceStatus_t;

typedef struct stLockRecord {
	char	vcard_uuid[32];
	char	person_uuid[32];
	int		timestamp;
	char	mac[32];
	int		opentype;
	char	area_uuid[32];
	char	vcardid[32];
	char	area_code[60];
}stLockRecord_t;

typedef struct stDeviceAlarm {
	char dev_uuid[32];
	int	 timestamp;
	char mac[32];
	int	 type;
	char area_uuid[32];
	char vcardid[32];
}stDeviceAlarm_t;

typedef struct stLog_t {
	char	module[32];
	int		level;				// 0->DEBUG, 1->ERROR, 2->WARNNING, 3->INFO
	char	content[256];
}stLog_t;


typedef union stTableRecord {
	stBasicInfo_t			basic;
	stPerson_t				persion;
	stVcard_t					vcard;
	stDevice_t				device;
	stVcardPerson_t		vcard_person;
	stVcardDevice_t		vcard_device;
	stDeviceStatus_t	device_status;
	stDeviceAlarm_t		device_alarm;
	stLockRecord_t		lock_record;
	stLog_t						log;
}stTableRecord_t;


enum {
	CBMODE_CREATE = 0,
	CBMODE_INSERT = 1,
	CBMODE_UPDATE = 2,
	CBMODE_SEARCH = 3,
	CBMODE_DELETE = 4,
	CBMODE_EXIST	= 5,
};
typedef struct stDataStorage {
	char						pathname[256];

	void						*db;

	int							cbmode;
	int							ret;
	stTableRecord_t records[256];
}stDataStorage_t;

int ds_init(const char *pathname, int option);
int ds_free();
int ds_insert_record(const char *tblname, stTableRecord_t *record);
int ds_update_record(const char *tblname, stTableRecord_t *record, const char *where);
int ds_delete_record(const char *tblname, const char *where);
int ds_search_record(const char *tblname, 
										 int compare(stTableRecord_t *, void *), 
										 void *arg,
										 const char *where,
										 stTableRecord_t *retult);





#ifdef __cplusplus
}
#endif

#endif
