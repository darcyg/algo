#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#include "algo.h"
#include "ds.h"
#include "util.h"
#include "web.h"
#include "curl.h"
#include "json_parser.h"

#include "common.h"
#include "ayla/log.h"
#include "ayla/timer.h"
#include "ayla/file_event.h"

#include "task.h"
#include "trig.h"

#if 0
static void base64_test();
static void md5_test();
static void triple_des_ecb_test();
static void db_test();
static void curl_test();
static void jansson_test();
static void web_test(const char *ip, int port);
#endif

static void init_path();
static int	parse_args(int argc, char *argv[]);
static int usage();
static int write_pid();
static void sig_set();
static void run_main();
static void timerout_cb(struct timer *t);
static void database_init();




///////////////////////////////////////////////////////////////////////////////////////////////
static char apppath[256];
static char basedir[256];
static char dbpath[256];
static char webbase[256];
static char *ip = "0.0.0.0";
static int  port = 80;
static int ds_child_died = 0;
static struct timer_head th = {.first = NULL};
static int loglevel = 0;

int main(int argc, char *argv[]) {

	init_path();

	/* base64 test */
	//base64_test();


	/* md5 test */
	//md5_test();

	/* 3des ecb */
	//triple_des_ecb_test();

	/* db test */
	//db_test();

	/* curl test */
	//curl_test();

	/* json_test */
	//jansson_test();

	/* web_test() */
	//web_test(argv[1], atoi(argv[2]));


	sig_set();

	log_init(argv[0], LOG_OPT_DEBUG | LOG_OPT_CONSOLE_OUT | LOG_OPT_TIMESTAMPS | LOG_OPT_FUNC_NAMES);

	if (parse_args(argc, argv) != 0) {
		usage();
		return -1;
	}

	if (write_pid() != 0) {
		log_err("smd has startted!");
		return -2;
	}

	run_main();

	return 0;
}

static void init_path() {
	get_exe_path(apppath, sizeof(apppath));
	strcpy(basedir, apppath);
	*rindex(basedir, '/') = 0;

	sprintf(dbpath, "%s/../devroot/test.db", basedir);
	sprintf(webbase, "%s/../devroot/www", basedir);

	printf("[apppath]: %s\n", apppath);
	printf("[basedir]: %s\n", basedir);
	printf("[dbpath ]: %s\n",	dbpath);
	printf("[webbase]: %s\n", webbase);
}

static void ds_child_exit_handler(int s) {
	log_info("[%s] %d", __func__, __LINE__);
	ds_child_died = 1;
}
static void ds_sig_exit_handler(int s) {
	log_info("[%s] %d : %d", __func__, __LINE__, s);
	exit(1);
}
static void ds_sigpipe_handler(int s) {
	log_info("[%s] %d : %d", __func__, __LINE__, s);
}
static void ds_exit_handler(void) {
	log_info("[%s] %d", __func__, __LINE__);
}

static void sig_set() {
	log_info("[%s] %d", __func__, __LINE__);

	struct sigaction sigHandler;

	memset(&sigHandler, 0, sizeof(sigHandler));

	sigHandler.sa_handler = ds_sig_exit_handler;
	sigemptyset(&sigHandler.sa_mask);
	sigaction(SIGINT, &sigHandler, NULL);
	sigaction(SIGTERM, &sigHandler, NULL);

	sigHandler.sa_handler = ds_child_exit_handler;
	sigaction(SIGCHLD, &sigHandler, NULL);

	sigHandler.sa_handler = ds_sigpipe_handler;
	sigaction(SIGPIPE, &sigHandler, NULL);

	atexit(ds_exit_handler);
}


static int parse_args(int argc, char *argv[]) {
	int ch = 0;
	while((ch = getopt(argc,argv,"i:p:l:"))!= -1){
		switch(ch){
			case 'i':
				ip = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'l':
				loglevel = atoi(optarg);
				break;
			default:
				return -1;
				break;
		}
	}
	return 0;
}

static int usage() {
	printf(	"Usage: smd [options] ...\n"
					"Options:\n"
					"  -i 192.168.10.1          httpd server ip\n"
					"  -p 80                    httpd server port\n"
					"  -l loglevel              Set ALink LogLevel:\n"
					"     0 : none\n"
					"     1 : fatal\n"
					"     2 : error\n"
					"     3 : warn\n"
					"     4 : info\n"
					"     5 : debug\n"
					"     6 : trace\n"
					"  -s                       Use Sandbox mode\n"
					"For more infomation, please mail to dlauciende@gmail.com\n");
	return 0;
}

static int write_pid() {
	int fd = -1;
	if (access("/var/run/smd:.pid", F_OK) != 0) {
		fd = open("/var/run/smd.pid", O_WRONLY | O_CREAT, 0644);
	} else {
		fd = open("/var/run/smd.pid", O_WRONLY);
	}
	
	if (fd < 0) {
		return -1;
	}
	
	if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
		return -2;
	}
	
	char buf[64];
	
	sprintf(buf, "%d\n", (int)getpid());
	if (write(fd, buf, strlen(buf)) != strlen(buf)) {
		return -3;
	}
	
	return 0;
}
void timerout_cb(struct timer *t) {
	log_info("[%s] %d : %p", __func__, __LINE__, t);
}

void database_init() {
	stTableRecord_t trbasic = {
		.basic = {
			.uuid = "",
			//.reluuid = "2323",
			//.state = 0,
			//.fwxxdz = "hzdusun",
			//.area_code = "310000",
			//.areatype = 0,
			.key = "",
			.dev_number = "",
			.ip = "192.168.0.6",
			.mac = "40f2e92b6622",
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
}

static void run_main() {
	log_info("[%s] %d", __func__, __LINE__);

	struct timer tr;
	timer_init(&tr, timerout_cb);

	struct file_event_table fet;
	file_event_init(&fet);

	database_init();
	task_init(&th, &fet);
	trig_init(&th, &fet);
	web_init(&th, &fet, ip, port, webbase);

	timer_set(&th, &tr, 10);
	log_info("[%s] %d : goto main loop", __func__, __LINE__);
	while (1) {
		s64 next_timeout_ms;
		next_timeout_ms = timer_advance(&th);
		if (file_event_poll(&fet, next_timeout_ms) < 0) {
			log_warn("poll error: %m");
		}
	}


	while (1) sleep(10);
}

///////////////////////////////////////////////////////////////////////////////////////////////
#if 0
static void base64_test() {
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

static void md5_test() {
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



static void triple_des_ecb_test() {
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
	printf("key is %s, len is %d\n", keystr, (int)strlen(keystr));

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

// reply of the requery  
static size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream) {  
	//cout << "----->reply" << endl;  
	//string *str = (string*)stream;  
	//cout << *str << endl;  
	//(*str).append((char*)ptr, size*nmemb);  

	printf("[%s] : [%s]\n", __func__, (char *)ptr);

	char *ptrString = (char *)ptr;
	char *outString = (char *)stream;
	strncat(outString, ptrString, size*nmemb);

	return size * nmemb;  
}  

// http GET  
static static CURLcode curl_get_req(const char *url, char *response) {  
	// init curl  
	CURL *curl = curl_easy_init();  
	// res code  
	CURLcode res;  
	if (curl)  {  
		// set params  
		curl_easy_setopt(curl, CURLOPT_URL, url); // url  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // if want to use https  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0); // set peer and host verify false  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);  
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);  
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);  
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);  
		// start req  
		res = curl_easy_perform(curl);  
	}  
	// release curl  
	curl_easy_cleanup(curl);  
	return res;  
}  

// http POST  
static CURLcode curl_post_req(const char *url, const char *postParams, char *response)  {  
	// init curl  
	CURL *curl = curl_easy_init();  
	// res code  
	CURLcode res = 0;  
	if (curl)   {  
		// set params  
		curl_easy_setopt(curl, CURLOPT_POST, 1); // post req  
		curl_easy_setopt(curl, CURLOPT_URL, url); // url  
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams); // params  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // if want to use https  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0); // set peer and host verify false  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);  
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);  
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);  
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);  
		// start req  
		res = curl_easy_perform(curl);  
	}  
	// release curl  
	curl_easy_cleanup(curl);  
	return res;  
}  

static void curl_test() {
	const char *url = "http://baidu.com";
	char resp[2048];
	int ret = 0;
	
	ret = curl_get_req(url, resp);
	

	if (ret != CURLE_OK) {
		printf("error : %s\n", curl_easy_strerror(ret));
		return;
	}

	printf("[resp] : %s\n", resp);

	curl_global_cleanup();


	//string postUrlStr = "https://www.baidu.com/s";  
	//string postParams = "f=8&rsv_bp=1&rsv_idx=1&word=picture&tn=98633779_hao_pg";  
	//string postResponseStr;  
	//auto res = curl_post_req(postUrlStr, postParams, postResponseStr);  

}

static void jansson_test() {
	json_t *x = json_object();
	if (x != NULL) {
		json_object_set_new(x, "value", json_integer(10));
		json_object_set_new(x, "name", json_string("name"));
		char *xstr =  json_dumps(x, 0);
		if (xstr != NULL) {
			printf("x is %s\n", xstr);
			free(xstr);
		}
		json_decref(x);
	} else {
		printf("new json object failed\n");
	}
}

static void web_test(const char *ip, int port) {
	ds_init(dbpath, 0, NULL);
	int ret = web_start(ip, port, webbase);
	if (ret != 0) {
		return;
	}
	
	while (1) {	

		int sock = web_socket_get();
		if (sock < 0) {
			sleep(1);
			continue;
		}

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(sock, &fds);

		struct timeval tv = {4, 0};
		int ret = select(sock+1, &fds, NULL, NULL, &tv);

		if (ret == 0) {
			continue;
		}
		if (ret < 0) {
			sleep (1);
			continue;
		}

		web_loop();
	}

	ds_free();
}


static int db_test_search_callback(stTableRecord_t *tr, void *arg) {
	printf("[%s] %d(%d,%s,%d,%s)\n", __func__, __LINE__, tr->log.timestamp,tr->log.module, tr->log.level, tr->log.content);
	return 0;
}

static void db_test() {
	stTableRecord_t trbasic = {
		.basic = {
			.uuid = "",
			//.reluuid = "2323",
			//.state = 0,
			//.fwxxdz = "hzdusun",
			//.area_code = "310000",
			//.areatype = 0,
			.key = "",
			.dev_number = "",
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
#endif

