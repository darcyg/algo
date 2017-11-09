#ifndef __COMM_H_
#define __COMM_H_


typedef enum emTaskType {
	TT_REGISTER	= 0,
	TT_RPT_DEV_STATUS = 1,
	TT_RPT_ALARM = 2,
	TT_RPT_RECORD = 3, 
}emTaskType_t;

typedef struct stTask {
	emTaskType_t	type;
	int  len;
	void *data;
}stTask_t;

typedef void (*THANDLER)(stTask_t*);

typedef struct stTaskHandler {
	int				type;
	THANDLER	handler;
}stTaskHandler_t;

typedef struct stRegister {
	char	mac[32];
	char	uuid[36];
}stRegister_t;

typedef struct stAccessRecord {
	char	cardno[36];
	char	mac[32];
	int		opentype;
	char	slide_date[32];
	char	dev_uuid[36];
	char	dev_number[32];
}stAccessRecord_t;
typedef struct stAlarm {
	char	occur_date[32];
	int		type_;
	char	mac[32];
	char	device_uuid[36];
	char	cardno[36];
}stAlarm_t;
typedef struct stStatus {
	char	dev_uuid[36];
	char	mac[32];
	char	hw_ver[32];
	char	sf_ver[32];
	char	imsi[32];
	char	msisdn[32];
	int		battery;
	int		temperature;
	int		signal_;
	int		card_capacity;
	int		whitelist_count;
	int		finger_capacity;
	int		finger_count;
	int		opened;
	int		work_mode;
	int		power_mode;
}stStatus_t;

enum {
	OSA_STATUS_OK = 0,
	OSA_STATUS_EINVAL = 22,
	OSA_STATUS_ENOMEM = 12,
	OSA_STATUS_EEXIST = 17,
	OSA_STATUS_ENOENT = 2,
};


void task_handler_task(stTask_t *t, stTaskHandler_t *th, int hcnt);

#endif
