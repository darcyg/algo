#ifndef __DS_H_
#define __DS_H_

#include "sqlite3.h"

#ifdef __cplusplus 
extern "C" {
#endif

#pragma pack(push)
#pragma pack(1)
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
	int		timestamp;
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
#pragma pack(pop)


typedef struct stItemInfo {
	const char *name;
	const char *type;
	int					len;
	int					primary;
}stItemInfo_t;

typedef struct stTableInfo {
	const	char		*tblname;
	int						itemcnt;
	stItemInfo_t	*items;
}stTableInfo_t;


typedef int (*SEARCH_CB)(stTableRecord_t *, void *);

/* ds_init
 * 
 * 数据库表init 
 *
 * @param[in] pathname 数据文件路径
 * @param[in] not used
 * @param[in] tr 记录,basic info表只有一条记录，初始化的时候复制进去
 * 
 * @return 0 -> ok, 其他失败
 */
int ds_init(const char *pathname, int option, stTableRecord_t *tr);

/* ds_free
 * 
 * 关闭数据库
 * 
 * @param none
 * 
 * @return 0
 */
int ds_free();

/* ds_insert_record
 * 
 * 插入数据记录
 * 
 * @param[in] tblname 表名称
 * @param[in] record  记录数据
 * 
 * @return 返回0表示OK, 其他表示失败
 */
int ds_insert_record(const char *tblname, stTableRecord_t *record);

/* ds_update_record
 * 
 * 更新数据记录
 * 
 * @param[in] tblname 表名称
 * @param[in] record  记录数据
 * @param[in] where  更新记录匹配条件
 * @param[in] ...　可变参数, where 和 ...共同组成 sql where语句条件, 例如 name=%s, "keven", 表示 where name='keven'
 * 
 * @return 返回0表示OK, 其他表示失败
 */
int ds_update_record(const char *tblname, stTableRecord_t *record, const char *where, ...);

/* ds_delete_record
 * 
 * 删除记录
 * 
 * @param[in] tblname 表名称
 * @param[in] where  更新记录匹配条件
 * @param[in] ...　可变参数, where 和 ...共同组成 sql where语句条件, 例如 name=%s, "keven", 表示 where name='keven'
 * 
 * @return 返回0表示OK, 其他表示失败
 */
int ds_delete_record(const char *tblname, const char *where, ...);

/* ds_search_record
 * 
 * 查找记录
 * 
 * @param[in] tblname 表名称
 * @param[in] callback 回调函数,对于每条匹配的记录会回调这个函数,
 * @param[in] args, 传给回调函数的参数
 * @param[in] where  更新记录匹配条件
 * @param[in] ...　可变参数, where 和 ...共同组成 sql where语句条件, 例如 name=%s, "keven", 表示 where name='keven'
 * 
 * @return 返回0表示OK, 其他表示失败
 */
int ds_search_record(const char *tblname, 
										 int callback(stTableRecord_t *, void *), 
										 void *arg,
										 const char *where,
										 ...);

/* ds_table_info
 * 
 * 查询表信息
 *
 * @param[in]  tblname 表名称　
 * 
 * @return 0 -> ok, < 0 -> 不存在
 */
int ds_table_info(const char *tblname, stTableInfo_t *ti);

/* ds_table_total_record_num 
 *
 * 查询表总的record 数目
 *
 * @param[in] tblname 表名
 *
 * @return 返回表的总记录数
 */
int ds_table_total_record_num(const char *tblname);

#ifdef __cplusplus
}
#endif

#endif
