#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "algo.h"
#include "ds.h"
#include "util.h"
#include "web.h"

void base64_test();
void md5_test();
void triple_des_ecb_test();
void db_test();
void web_test(const char *ip, int port);

static char apppath[256];
static char basedir[256];
static char dbpath[256];
static char webbase[256];

int main(int argc, char *argv[]) {

	get_exe_path(apppath, sizeof(apppath));
	strcpy(basedir, apppath);
	*rindex(basedir, '/') = 0;

	sprintf(dbpath, "%s/../devroot/test.db", basedir);
	sprintf(webbase, "%s/../devroot/www", basedir);

	printf("[apppath]: %s\n", apppath);
	printf("[basedir]: %s\n", basedir);
	printf("[dbpath ]: %s\n",	dbpath);
	printf("[webbase]: %s\n", webbase);


	/* base64 test */
	base64_test();


	/* md5 test */
	md5_test();

	/* 3des ecb */
	triple_des_ecb_test();

	/* db test */
	db_test();

	/* web_test() */
	web_test(argv[1], atoi(argv[2]));
	return 0;
}

void base64_test() {
	//char buf[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	char buf[] = {'1', '2', '3', '4', '5', '6', '7'};
	int len = algo_Base64EncodeLen(sizeof(buf));
	char *out = (char *)malloc(len+1);
	algo_Base64Encode(out, buf, sizeof(buf));

	int dlen = algo_Base64DecodeLen(out);
	char *dout = (char *)malloc(dlen+1);
	algo_Base64Decode(dout, out);
	
	printf("\n[Base64 Encode/Decode Test]:\n");
	printf("origin  ---        < ");
	view_buf(buf, sizeof(buf));
	printf(" >\n");

	printf("coded   ---        < ");
	printf("%s", out);
	printf(" >\n");

	printf("decoded ---        < ");
	view_buf(dout, dlen);
	printf(" >\n");


	free(out);
	free(dout);
}

void md5_test() {
	const char *str = "hello";
	char out[16];
	
	algo_Md5Encode(str, strlen(str), out);
	
	
	printf("\n[MD5 Encode Test]:\n");
	printf("string  ---         < ");
	printf("%s", str);
	printf(" >\n");
	printf("coded   ---         < ");
	view_buf(out, sizeof(out));
	printf(" >\n");
	
}


int db_test_search_callback(stTableRecord_t *tr, void *arg) {
	printf("[%s] %d(%d,%s,%d,%s)\n", __func__, __LINE__, tr->log.timestamp,tr->log.module, tr->log.level, tr->log.content);
	return 0;
}

void db_test() {
	stTableRecord_t trbasic = {
		.basic = {
			.uuid = "1212",
			.reluuid = "2323",
			.state = 0,
			.fwxxdz = "hzdusun",
			.area_code = "310000",
			.areatype = 0,
			.ip = "192.168.10.1",
			.mac = "0102030405060708",
			.model = "dusun",
			.sysversion = "1.0.0",
			.appversion = "1.0.0",
			.adminname  = "admin",
			.adminpass	= "123456",
			.capability = 0,
			.lurumode		= 0,
		},
	};


	ds_init(dbpath, 0, &trbasic);

	int i = 0; 
	static char* modules[] = {
		"TEST", "FUNC", "STORAGE",
	};
	for (i = 0; i < 10; i++) {
		stTableRecord_t tr = {
			.log = {
				.timestamp = time(NULL),
				.module = "",
				.level	= i%3,
				.content = "helloworld",
			},
		};
		strcpy(tr.log.module, modules[rand()%3]);
		ds_insert_record("log", &tr);
	}

	ds_delete_record("log", "where module = '%s'", "TEST");
	ds_delete_record("log", "where module = '%s'", "FUNC");

	stTableRecord_t tr = {
			.log = {
				.timestamp = time(NULL),
				.module = "MODIFIED",
				.level	= 99,
				.content = "Yes",
			},
	};
	ds_update_record("log", &tr, "where module = '%s'", "STORAGE");

	ds_search_record("log", db_test_search_callback, NULL, "where module = '%s'", "MODIFIED");

	ds_free();
}

void triple_des_ecb_test() {
	//char *origin = "01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQ";
	//char *origin = "1234567";
	//char *origin = "5140000000080401AEBFFFFFFFF7FBFE";
	char *origin = "dongsheng";
	char enstr[256];
	char destr[256];

	char g_keyBytes[]={0x11, 0x22, 0x4F, 0x58,0x88, 0x10, 0x40, 0x38, 0x28, 0x25, 0x79, 0x51,   
                     0xCB,0xDD, 0x55, 0x66, 0x77, 0x29, 0x74, 0x98, 0x30, 0x40, 0x36, 0xE2 };
	//char g_keyBytes[]={0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x31, 0x32, 0x33,
	//										0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x31, 0x32, 0x33};


	char keystr[512];
	int len = 0;
	

	long t1, t2, t3, t4;

	printf("[%s]", g_keyBytes);
	//algo_3des_ecb_init("0123456789abcdef0123456789abcdef0123456789abcdef");

	int i = 0;
	for (i = 0; i < sizeof(g_keyBytes); i++) {
		len += sprintf(keystr + len,"%02X", g_keyBytes[i]&0xff);
	}
	printf("key is %s, len is %d\n", keystr, strlen(keystr));

	//algo_3des_ecb_init("313131313131313131313131313131313131313131313131");
	//algo_3des_ecb_init("313131313131313131313131313131313131313131313131");
	algo_3des_ecb_init(keystr);
	//algo_3des_ecb_init(g_keyBytes);
	
	
	t1 = current_system_time_us();
	algo_3des_ecb_encrypt(origin, enstr);
	t2 = current_system_time_us();

	t3 = current_system_time_us();
	algo_3des_ecb_decrypt(enstr, destr);
	t4 = current_system_time_us();

	
	printf("\n[3Des-Ecb Encode/DeCode Test]:\n");
	printf("origin  ---         < ");
	printf("%s", origin);
	printf(" >\n");
	printf("coded   ---         < ");
	printf("%s", enstr);
	printf(" >\n");
	printf("used time: %ld us\n", t2-t1);
	printf("decoded ---         < ");
	printf("%s", destr);
	printf(" >\n");
	printf("used time: %ld us\n", t4-t3);

	algo_3des_ecb_free();
	
}

void web_test(const char *ip, int port) {
	ds_init(dbpath, 0, NULL);
	int ret = web_start(ip, port, webbase);
	if (ret != 0) {
		return;
	}
	
	while (1) {	
		web_loop();
	}

	ds_free();
}


