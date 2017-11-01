#include "web.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "ds.h"
#include "json_parser.h"

#include "common.h"
#include "ayla/log.h"
#include "ayla/timer.h"
#include "ayla/file_event.h"




static stWebEnv_t we = {
	.ip		= "192.168.0.6",
	.port = 10888,
	.base	= "./www",
	.svr	= NULL,
	.user = "admin",
	.pass = "admin",
	//.currpage = 3,
	.th = NULL,
	.fet = NULL,
};

static const char *tablenames[] = {
//	"basicinfo",
	"person",
	//"device",
	"vcard",
	"vcard_person",
	//"vcard_device",
	"device_status",
	"lock_record",
	"device_alarm",
	"log",
};
/* 1   1   1   */
/* add mod del*/
static int tableops[] = {
	0x7, 0x7, 0x7, 
	//0x7, 0x7,
	//0x7, 0x7, 0x7
	0x1, 0x1, 0x1, 0x1
	//0x0, 0x0, 0x0, 0x0
};


static void center_func(httpd *server, httpReq *req);
static void prev_func(httpd *server, httpReq *req);
static void next_func(httpd *server, httpReq *req);
static void curr_func(httpd *server, httpReq *req);
static void add_func(httpd *server, httpReq *req);
static void del_func(httpd *server, httpReq *req);
static void mod_func(httpd *server, httpReq *req);
static void menu_func(httpd *server, httpReq *req);
static void web_render_header(httpd *server, httpReq *req);
static void web_render_center(httpd *server, httpReq *req);
static void web_render_footer(httpd *server, httpReq *req);
static void web_render_menu(httpd *server, httpReq *req);
static void web_render_middle(httpd *server, httpReq *req);
static void web_render_content(httpd *server, httpReq *req);
static void web_render_other(httpd *server, httpReq *req);


static void web_render_status(httpd *server, httpReq *req);
static void web_render_import_export(httpd *server, httpReq *req);
static void web_render_import(httpd *server, httpReq *req);
static void web_render_export(httpd *server, httpReq *req);
static void web_render_modify(httpd *server, httpReq *req);
static void web_render_admin(httpd *server, httpReq *req);
static void web_render_about(httpd *server, httpReq *req);


static void api_db_insert_func(httpd *server, httpReq *req);
static void api_db_update_func(httpd *server, httpReq *req);
static void api_db_select_func(httpd *server, httpReq *req);
static void api_db_delete_func(httpd *server, httpReq *req);
static void api_db_import_func(httpd *server, httpReq *req);
static void api_db_export_func(httpd *server, httpReq *req);
static void api_db_setpass_func(httpd *server, httpReq *req);


static void api_devdbm_add_person(httpd *server, httpReq *req);
static void api_devdbm_rm_person(httpd *server, httpReq *req);
static void api_devdbm_add_card(httpd *server, httpReq *req);
static void api_devdbm_rm_card(httpd *server, httpReq *req);

static void web_render_table(httpd *server, httpReq *req, const char *tblname, int start, int count);

static int db_test_search_status(stTableRecord_t *tr, void *arg);

static stContentHandler_t chs[] = {
	{PAGE_STATUS, web_render_status},
	//{PAGE_IMPORT, web_render_import},
	//{PAGE_EXPORT, web_render_export},
	{PAGE_IMPORT_EXPORT, web_render_import_export},
	{PAGE_MODIFY, web_render_modify},
	{PAGE_ADMIN,	web_render_admin},
	//{PAGE_ABOUT,  web_render_about},
};

static stWebNode_t wns[] = {
	{PT_WILD, "/image",		INVALID_ITEM,	INVALID_ITEM,	NULL, INVALID_ITEM,		"image",			INVALID_ITEM},

	{PT_WILD, "/css",			INVALID_ITEM,	INVALID_ITEM,	NULL, INVALID_ITEM,		"css",				INVALID_ITEM},
	{PT_WILD, "/js",			INVALID_ITEM,	INVALID_ITEM,	NULL, INVALID_ITEM,		"js",					INVALID_ITEM},
	{PT_WILD, "/site",		INVALID_ITEM,	INVALID_ITEM,	NULL, INVALID_ITEM,		"site",				INVALID_ITEM},
	{PT_WILD, "/font",		INVALID_ITEM,	INVALID_ITEM,	NULL, INVALID_ITEM,		"font",				INVALID_ITEM},


	{PT_CFUNC, "/",				"",						HTTP_TRUE,		NULL, center_func,		INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"index.html",	HTTP_TRUE,		NULL, center_func,		INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"index.htm",	HTTP_TRUE,		NULL, center_func,		INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"prev",				0,						NULL, prev_func,			INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"next",				0,						NULL, next_func,			INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"curr",				0,						NULL, curr_func,			INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"add",				0,						NULL, add_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"del",				0,						NULL, del_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"mod",				0,						NULL, mod_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/",				"menu",				0,						NULL, menu_func,				INVALID_ITEM, INVALID_ITEM},

	{PT_FILE,	"/",				"error",				0,					NULL, INVALID_ITEM,		"page/error.html",	INVALID_ITEM},
	{PT_FILE, "/",				"favicon.ico",	0,					NULL, INVALID_ITEM,		"image/favicon.ico",				INVALID_ITEM},


	{PT_CFUNC, "/api/db/",	"insert",			0,						NULL, api_db_insert_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/db/",	"update",			0,						NULL, api_db_update_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/db/",	"select",			0,						NULL, api_db_select_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/db/",	"delete",			0,						NULL, api_db_delete_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/db/",	"import",			0,						NULL, api_db_import_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/db/",	"export",			0,						NULL, api_db_export_func,				INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/ad/",	"setpass",		0,						NULL, api_db_setpass_func,			INVALID_ITEM, INVALID_ITEM},

	{PT_CFUNC, "/api/dev_dbm/",	"add_person",		0,			NULL, api_devdbm_add_person,		INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/dev_dbm/",	"rm_person",		0,			NULL, api_devdbm_rm_person,			INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/dev_dbm/",	"add_card",			0,			NULL, api_devdbm_add_card,			INVALID_ITEM, INVALID_ITEM},
	{PT_CFUNC, "/api/dev_dbm/",	"rm_card",			0,			NULL, api_devdbm_rm_card,				INVALID_ITEM, INVALID_ITEM},
};


static void web_add_nodes(httpd *svr) {
	int cnt = sizeof(wns)/sizeof(wns[0]);
	int i = 0;

	//httpdAddFileContent(svr, "/", "index", HTTP_TRUE, NULL, "/opt/au/tmp/smartdd/algo/www/index.html");
	//httpdAddCContent(svr, "/", "", HTTP_TRUE, NULL, index_function);
	//httpdAddCContent(svr, "/", "index", HTTP_TRUE, NULL, index_function);
	//httpdAddCContent(svr, "/", "index.html", HTTP_TRUE, NULL, index_function);
	//httpdAddCContent(svr, "/", "index.htm", HTTP_TRUE, NULL, index_function);
	//httpdAddWildcardContent(svr, "/images",NULL, "images");


	for (i = 0; i <  cnt; i++) {
		stWebNode_t *wn = &wns[i];
		switch (wn->type) {
			case PT_CFUNC:
				httpdAddCContent(svr, wn->dir, wn->name, wn->isindex, (WEB_PRELOAD)wn->preload, (WEB_FUNCTION)wn->function);
			break;
			case PT_FILE:
				httpdAddFileContent(svr, wn->dir, wn->name, wn->isindex, wn->preload, wn->path);
			break;
			case PT_WILD:
				httpdAddWildcardContent(svr, wn->dir, wn->preload, wn->path);
			break;
			case PT_CWILD:
				httpdAddCWildcardContent(svr, wn->dir, wn->preload, wn->function);
			break;
			case PT_STATIC:
				httpdAddStaticContent(svr, wn->dir, wn->name, wn->isindex, wn->preload, wn->html);
			break;
			default:
			break;
		}
	}
}

int web_start(const char *ip, int port, const char *base) {
	strcpy(we.ip, ip);
	we.port = port;
	strcpy(we.base, base);

	we.svr = httpdCreate(we.ip, we.port);
	if (we.svr == NULL) {
		printf("Couldn't create http server : %s %d\n", we.ip, we.port);
		return -1;
	}

	httpdSetAccessLog	(we.svr,	stdout);
	httpdSetErrorLog	(we.svr,	stdout);
	httpdSetFileBase	(we.svr,	we.base);

	web_add_nodes(we.svr);

	printf("start at : %s:%d\n", we.ip, we.port);

	return 0;
}

int web_socket_get() {
	if (we.svr != NULL) {
		return we.svr->serverSock;
	}
	return -1;
}

int web_loop() {
	while (1) {
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		int result;
		httpReq *req =  httpdReadRequest(we.svr, &timeout, &result);
		if (req == NULL && result == 0) {
			return 0;
			continue;
		} 
		if (result < 0) {
			/* error */
			continue;
		}
		httpdProcessRequest(we.svr, req);
		httpdEndRequest(we.svr, req);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
static int AuthSuccess(httpd *server, httpReq *req) {
	if (httpdAuthenticate(server, req, (char *)"LibHTTPD Test") == 0) {
		return 0;
	}
	if (strcmp(req->authUser, we.user) != 0 ||
			strcmp(req->authPassword, we.pass) != 0) {
		return 0;
	}

	return 1;
}


static void RenderAuth(httpd *server, httpReq *req) {
	httpdForceAuthenticate(server, req, (char *)"LibHTTPD Test");
}




static void center_func(httpd *server, httpReq *req) {
	if (!AuthSuccess(server, req)) {
		RenderAuth(server, req);
		return;
	} 

	web_render_header(server, req);
	web_render_center(server, req);
	web_render_footer(server, req);
}



static void web_render_header(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"<html>\n"\
		"	<head>\n"\
		"		<meta charset=\"UTF-8\">\n"
		//"		<title>智能门禁系统</title>\n"
		"		<title>智能门禁</title>\n"
		"		<link href=\"css/drr.css\" rel=\"stylesheet\" type=\"text/css\"/>\n"
		"		<script language=\"JavaScript\" src=\"js/jquery-3.2.1.min.js\">\n"\
		"		</script>\n"\
		"		<script language=\"JavaScript\" src=\"js/dr.js\">\n"\
		"		</script>\n"\
		"	</head>\n"\
		"	<body><div id=\"body\">\n"\
		"		<div id=\"header\">\n"\
		"		</div>\n"
	);
}
static void web_render_footer(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"		<div id=\"footer\">\n" \
		"		</div>\n"\
		"	</div></body>\n"\
		"</html>\n"
	);
}
static void web_render_center(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char*)
		"		<div id=\"center\">\n"
	);
	web_render_menu(server, req);
	/*
	httpdPrintf(server, req, (char*)
		"		<div style=\"clear:both\">\n"
	);
	*/
	web_render_middle(server, req);
	web_render_content(server, req);
	web_render_other(server, req);

	httpdPrintf(server, req, (char*)
		"		</div>\n"
	);
}
static void web_render_menu(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char*)
		"			<div id=\"menu\">\n"
	);


	httpdPrintf(server, req, (char *)
		"				<ul>\n"
	);

	stMenuItem_t ms[] = {
		{"4.jpg", "Status",			"当前状态",		0,			NULL},
		{"4.jpg", "Import/Export",			"导入/导出",	0,			NULL},
		//{"4.jpg", "Export",			"Export DataBase",	0,			NULL},
		{"4.jpg", "Modify",			"修改",	0,			NULL},
		{"4.jpg", "Admin",			"管理设置",		0,			NULL},
		//{"4.jpg", "About",			"About Info",				0,			NULL},
	};

	int cnt = sizeof(ms)/sizeof(ms[0]);
	int i = 0;
	for (i = 0; i < cnt; i++) {
		stMenuItem_t *mi = &ms[i];
	httpdPrintf(server, req, (char *)
		"					<li>\n"
		"						<div>\n"
		"							<div><img src=\"image/f-%d.jpg\"></img></div>\n"
		"							<p><strong>%s</strong></p>\n"
		"							<p>%s</p>\n"
		"						</div>\n"
		"					</li>\n",
		i+1,
		mi->menuname,
		mi->description
	);
	}
	httpdPrintf(server, req, (char *)
		"				</ul>\n"
	);
	

	httpdPrintf(server, req, (char *)
		"			</div>\n"
	);
}

static void web_render_middle(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char*)
		"			<div id=\"middle\">a</div>\n"
	);
}

static void web_render_content(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char*)
		"			<div id=\"content\">\n"
	);

	menu_func(server, req);


	httpdPrintf(server, req, (char *)
		"			</div>\n"
	);
}

static void web_render_status(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"				<div class=\"disarea\">\n"
	);

	httpdPrintf(server, req, (char *)
		"				<div >开发中...</div>\n"
	);

	char *tblname = "basicinfo";
	stReq_t r = {server, req, tblname, NULL};
	ds_search_record(tblname, db_test_search_status, &r, "limit 0, 1");

	httpdPrintf(server, req, (char *)
		"				</div>\n"
	);


}
static void web_render_import(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"				<div class=\"disarea\">\n"
	);
	httpdPrintf(server, req, (char *)
		"					<div class=\"title\">\n"
		"						<p>ImportDataBase</p>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"inputfile\">\n"
		"						<form method=\"post\" action=\"importdb\">\n"
		"							<input type=\"text\" size=\"20\"></input>\n"
		"							<input type=\"button\" value=\"Import\"></input>\n"
		"						</form>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"progress\">\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"Submit\">\n"
		"						<input type=\"button\" value=\"Submit\"></input>\n"
		"					</div>\n"
	);



	httpdPrintf(server, req, (char *)
		"				</div>\n"
	);


}

static void web_render_import_export(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"				<div class=\"disarea\">\n"
	);
	httpdPrintf(server, req, (char *)
		"					<div class=\"title\">\n"
		"						<p>导入数据库</p>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"inputfile\">\n"
		"						<form method=\"post\" action=\"importdb\">\n"
		"							<input type=\"text\" size=\"20\"></input>\n"
		"							<button>导入</input>\n"
		"						</form>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"title\">\n"
		"						<p>导出数据库</p>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"inputfile\">\n"
		"						<form method=\"post\" action=\"importdb\">\n"
		"							<button>导出</button>\n"
		"						</form>\n"
		"					</div>\n"
	);


	httpdPrintf(server, req, (char *)
		"					<div class=\"progress\">\n"
		"					</div>\n"
	);

	/*
	httpdPrintf(server, req, (char *)
		"					<div class=\"Submit\">\n"
		"						<input type=\"button\" value=\"Submit\"></input>\n"
		"					</div>\n"
	);
	*/



	httpdPrintf(server, req, (char *)
		"				</div>\n"
	);

}
static void web_render_modify(httpd *server, httpReq *req) {


	httpdPrintf(server, req, (char *)
		"				<div class=\"disarea\">\n"
	);
	httpdPrintf(server, req, (char *)
		"					<div class=\"title\">\n"
		"						<strong display=\"inline\">修改数据表:</strong>\n"
		"						<select type=\"select\" display=\"inline\" id=\"tblname\">\n");

		int i = 0;
		for (i = 0; i < sizeof(tablenames)/sizeof(tablenames[0]); i++) {
			httpdPrintf(server, req, "							<option>%s</option>\n", tablenames[i]);
		}
	httpdPrintf(server, req, (char *)
		"						</select>\n"
		"						<button id=\"prev\">前一页</button>\n"
		"						<button><strong id=\"curr\">10/100</strong></button>\n"
		"						<button id=\"next\">后一页</button>\n"
		"						<button id=\"add\">增加</button>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"hline\">\n"
		"						<hr>\n"
		"					</div>\n"
	);



	httpdPrintf(server, req, (char *)
		"					<div class=\"table\" id=\"moditbl\">\n"
	);

	const char *tblname = "person";
	web_render_table(server, req, tblname, 0, 10);

	httpdPrintf(server, req, (char *)
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"progress\">\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"				</div>\n"
	);
}

static void web_render_export(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"				<div class=\"disarea\">\n"
	);
	httpdPrintf(server, req, (char *)
		"					<div class=\"title\">\n"
		"						<p>ExportDataBase</p>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"inputfile\">\n"
		"						<form method=\"post\" action=\"importdb\">\n"
		"							<button type=\"button\">DownLoad</button>\n"
		"						</form>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"progress\">\n"
		"					</div>\n"
	);


	httpdPrintf(server, req, (char *)
		"				</div>\n"
	);


}
static void web_render_admin(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"				<div class=\"disarea\">\n"
	);
	httpdPrintf(server, req, (char *)
		"					<div class=\"title\">\n"
		"						<p>修改密码</p>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"inputfile\">\n"
		"						<form method=\"post\" action=\"importdb\">\n"
		"							<input type=\"password\" size=\"20\"></input>\n"
		"							<input type=\"password\" size=\"20\"></input>\n"
		"						</form>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"progress\">\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"submit\">\n"
		"						<button>修改</button>\n"
		"					</div>\n"
	);



	httpdPrintf(server, req, (char *)
		"				</div>\n"
	);

}
static void web_render_about(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"				<div class=\"disarea\">\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"inputfile\">\n"
		"						<div><p>Dusun Ltd.</p></div>\n"
		"						<div><a>tvlink.com</a></p>\n"
		"						<div><a>2017/10/19</a></p>\n"
		"					</div>\n"
	);


	httpdPrintf(server, req, (char *)
		"				</div>\n"
	);
}


static void web_render_other(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"				<div id=\"other\">\n"
		"					<div>\n"
		"						<strong>Software Version: %d.%d.%d Build %s %s</strong>\n"
		"					</div>\n"
		"				</div>\n",
		MAJOR,MINOR,PATCH,DATE,TIME
	);


}

//////////////////////////////////////////////////////////////////////
static int web_get_ops(const char *tblname) {
	int i = 0;
	int cnt = sizeof(tablenames)/sizeof(tablenames[0]);
	for (i = 0; i < cnt; i++) {
		if (strcmp(tblname, tablenames[i]) == 0) {
			break;
		}
	}
	if (i == cnt) {
		return 0;
	}
	//printf("ops %d[%s]\n", tableops[i], tblname);
	return tableops[i];
}
static int db_test_search_status(stTableRecord_t *tr, void *arg) {
	stReq_t *r = (stReq_t *)arg;
	httpd		*server = r->server;
	httpReq *req = r->request;
	char *tblname = r->tblname;

	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;


	//int ops = web_get_ops(tblname);
	httpdPrintf(server, req, (char *)
			"								<div id=\"status\">\n"
	);


	char *ptr = (char *)tr;
	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				httpdPrintf(server, req, (char *)
					"								<div class=\"shl\"><div>%s</div><div></div><div>%d</div></div>\n", ii->name, *ptr);
					ptr += ii->len * 1;
			} else {
				httpdPrintf(server, req, (char *)
					"								<div class=\"shl\"><div>%s</div><div></div><div>%s</div></div>\n", ii->name, ptr);
					ptr += ii->len * 1;
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				httpdPrintf(server, req, (char *)
					"								<div class=\"shl\"><div>%s</div><div></div><div>%d</div></div>\n", ii->name, *(int*)ptr);
					ptr += ii->len * 4;
			} else {
				httpdPrintf(server, req, (char *)
					"								<div class=\"shl\"><div>%s</div><div></div><div>%s</div></div>\n", ii->name, "-");
					ptr += ii->len * 4;
			}
		}
	}

	httpdPrintf(server, req, (char *)
			"								</div>\n"
	);



	return 0;
}
static int db_test_search_callback_1(stTableRecord_t *tr, void *arg) {
	stReq_t *r = (stReq_t *)arg;
	httpd		*server = r->server;
	httpReq *req = r->request;
	char *tblname = r->tblname;

	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;

	int ops = web_get_ops(tblname);

	httpdPrintf(server, req, (char *)
		"							<tr>\n"
	);
	char *ptr = (char *)tr;
	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				httpdPrintf(server, req, (char *)
					"								<td>%d</td>\n", *ptr);
					ptr += ii->len * 1;
			} else {
				httpdPrintf(server, req, (char *)
					"								<td>%s</td>\n", ptr);
					ptr += ii->len * 1;
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				httpdPrintf(server, req, (char *)
					"								<td>%d</td>\n", *(int*)ptr);
					ptr += ii->len * 4;
			} else {
				httpdPrintf(server, req, (char *)
					"								<td>%s</td>\n", "-");
					ptr += ii->len * 4;
			}
		}
	}

	if (ops == 0x7) {
	httpdPrintf(server, req, (char *)
		"								<td class=\"op\"><button class=\"del\">删除</button>  <button class=\"mod\">修改</button>\n"
	);
	} else if (ops == 0x0) {
	httpdPrintf(server, req, (char *)
		"								<td class=\"op\"><button disabled=\"disabled\">删除</button>  "
		"<button disabled=\"disabled\">修改</button>\n"
	);
	} else if (ops == 0x1) {
	httpdPrintf(server, req, (char *)
		"								<td class=\"op\"><button class=\"del\">删除</button>\n"
		"<button disabled=\"disabled\">修改</button>\n"
	);
	} else {
	httpdPrintf(server, req, (char *)
		"								<td class=\"op\"><button disabled=\"disabled\">删除</button>  "
		"<button disabled=\"disabled\">修改</button>\n"
	);
	}

	httpdPrintf(server, req, (char *)
		"							</tr>\n"
	);

	return 0;
}

static int search_nline_callback(stTableRecord_t *tr, void *arg) {
	stReq_t *r = (stReq_t *)arg;
	//httpd		*server = r->server;
	//httpReq *req = r->request;
	char *tblname = r->tblname;
	char *where = (char *)r->arg;
	int len = 0;

	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;

	len += sprintf(where, "where ");
	
	char *ptr = (char *)tr;
	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];

		char *and = "";
		if (i > 0 ) {
			and = "and ";
		}

		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				int x = (*ptr)&0xff;
				len += sprintf(where + len, "%s%s = %d ", and, ii->name, x);
		
				ptr += ii->len * 1;
			} else {
				len += sprintf(where + len, "%s%s = '%s' ", and, ii->name, ptr);

				ptr += ii->len * 1;
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				int x = *(int*)ptr;
				len += sprintf(where + len, "%s%s = %d ", and, ii->name, x);

				ptr += ii->len * 4;
			} else {
				ptr += ii->len * 4;
			}
		}
	}

	return 0;
}

static void web_render_table(httpd *server, httpReq *req, const char *tblname, int start, int count) {

	httpdPrintf(server, req, (char *)
		"						<table >\n"
		"							<tr>\n");
	
	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;
	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
	httpdPrintf(server, req, (char *)
		//"								<th>%s(%s[%d])</th>\n", ii->name, ii->type, ii->len);
		"								<th>%s</th>\n", ii->name);
	}
	httpdPrintf(server, req, (char *)
		"								<th>Opeation</th>\n"
		"							</tr>\n");

	stReq_t r = {server, req, tblname, NULL};
	//ds_search_record(tblname, db_test_search_callback_1, &r, "between(recno(), %d, %d)", start, start + count);
	ds_search_record(tblname, db_test_search_callback_1, &r, "limit %d,%d", start, count);

	httpdPrintf(server, req, (char *)
		"						</table>\n"
	);


	int num = ds_table_total_record_num(tblname);
	int total = (num + count -1 ) / count;
	int curr = start / count;
	httpdPrintf(server, req, (char *) 
		"						<div style=\"display:none\" id=\"aurr\">%d/%d</div>\n",
		curr+1,
		total);

}


//////////////////////////////////////////////////////////////////////
static void prev_func(httpd *server, httpReq *req) {
	httpdDumpVariables(server, req);

	httpVar *val = httpdGetVariableByName(server, req, "curr");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	int curr = atoi(val->value);
	curr--;

	val = httpdGetVariableByName(server, req, "tblname");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	char tblname[64];
	strcpy(tblname, val->value);


	int num = ds_table_total_record_num(tblname);
	int total = (num + 10-1)/10;

	curr = curr-1;

	if (curr >= total) {
		curr = total - 1;
	}

	if (curr < 0) { 
		curr =  0;
	}


	web_render_table(server, req, tblname, curr * 10, 10) ;

	
}
static void next_func(httpd *server, httpReq *req) {
	httpdDumpVariables(server, req);

	httpVar *val = httpdGetVariableByName(server, req, "curr");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	int curr = atoi(val->value);
	curr--;

	val = httpdGetVariableByName(server, req, "tblname");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	char tblname[64];
	strcpy(tblname, val->value);


	int num = ds_table_total_record_num(tblname);
	int total = (num + 10-1)/10;

	curr = curr+1;

	if (curr >= total) {
		curr = total - 1;
	}

	if (curr < 0) { 
		curr =  0;
	}


	web_render_table(server, req, tblname, curr * 10, 10) ;
}

static void curr_func(httpd *server, httpReq *req) {
	httpdDumpVariables(server, req);

	httpVar *val = httpdGetVariableByName(server, req, "curr");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	int curr = atoi(val->value);
	curr--;

	val = httpdGetVariableByName(server, req, "tblname");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	char tblname[64];
	strcpy(tblname, val->value);


	int num = ds_table_total_record_num(tblname);
	int total = (num + 10-1)/10;

	if (curr >= total) {
		curr = total - 1;
	}

	if (curr < 0) { 
		curr =  0;
	}


	web_render_table(server, req, tblname, curr * 10, 10) ;
}

static void del_func(httpd *server, httpReq *req) {
	httpdDumpVariables(server, req);


	httpVar *val = httpdGetVariableByName(server, req, "curr");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	int curr = atoi(val->value);
	curr--;


	val = httpdGetVariableByName(server, req, "tblname");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	char tblname[64];
	strcpy(tblname, val->value);

	int ops = web_get_ops(tblname);
	if ((ops &0x1) == 0) {
		httpdPrintf(server, req, (char *)
			"<div>错误: 没有执行的权限!</div>\n"
		);
		return;
	}

	int num = ds_table_total_record_num(tblname);
	int total = (num + 10-1)/10;

	if (curr >= total) {
		curr = total - 1;
	}

	if (curr < 0) { 
		curr =  0;
	}


	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;

	stTableRecord_t tr;
	char *ptr = (char *)&tr;
	int i = 0;
	char where[256];
	int len = 0;
	len += sprintf(where, "where ");


	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		char argx[32];
		sprintf(argx, "arg%d", i);
		httpVar *val = httpdGetVariableByName(server, req, argx);

		if (val == NULL) {
			httpdPrintf(server, req, (char *)
				"<div>Error: missing argments!</div>\n"
			);
			return;
		}

		char *and = "";
		if (i > 0 ) {
			and = "and ";
		}

		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				int x = 0;
				sscanf(val->value, "%d", &x);

		
				*ptr = x&0xff;

				//printf("%d ", (*ptr)&0xff);

				ptr += ii->len * 1;

				len += sprintf(where + len, "%s%s = %d ", and, ii->name, x);
			} else {
				strcpy(ptr, val->value);

				//printf("%s ", ptr);

				ptr += ii->len * 1;

				len += sprintf(where + len, "%s%s = '%s' ", and, ii->name, val->value);
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				int x = 0;
				sscanf(val->value, "%d", &x);

				*(int*)ptr = x;

				//printf("%d ", x);

				ptr += ii->len * 4;

				len += sprintf(where + len, "%s%s = %d ", and, ii->name, x);
			} else {
				ptr += ii->len * 4;
			}
		}
	}

	//printf("\n del %s\n", where);
	ds_delete_record(tblname, where);
	web_render_table(server, req, tblname, curr * 10, 10) ;
}



static void menu_func(httpd *server, httpReq *req) {
	int page;

	httpVar *val = httpdGetVariableByName(server, req, "page");
	if (val == NULL) {
		page = 2;
	} else {
		page = atoi(val->value);
	}
	printf("page is %d\n", page);




	int i = 0;
	for (i = 0; i < sizeof(chs)/sizeof(chs[0]); i++) {
		stContentHandler_t *ch = &chs[i];
		if (ch->page == page) {
			ch->function(server, req);
			break;
		}
	}

}

static void mod_func(httpd *server, httpReq *req) {
	httpdDumpVariables(server, req);

	httpVar *val = httpdGetVariableByName(server, req, "curr");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	int curr = atoi(val->value);
	curr--;

	val = httpdGetVariableByName(server, req, "tblname");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	char tblname[64];
	strcpy(tblname, val->value);

	int ops = web_get_ops(tblname);
	if ((ops &0x02) == 0) {
		httpdPrintf(server, req, (char *)
			"<div>错误: 没有执行的权限!</div>\n"
		);
		return;
	}



	int num = ds_table_total_record_num(tblname);
	int total = (num + 10-1)/10;

	if (curr >= total) {
		curr = total - 1;
	}

	if (curr < 0) { 
		curr =  0;
	}


	val = httpdGetVariableByName(server, req, "start");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	int start = atoi(val->value);
	start--;
	if (start < 0) {
		start = 0;
	}
	if (start >= num) {
		start = num -1;
	}


	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;

	char where[256] = {0};
	stReq_t r = {server, req, tblname, where};
	ds_search_record(tblname, search_nline_callback, &r, "limit %d,%d", curr*10 + start, 1);
	//len += sprintf(where, "where (select * from %s limit %d,%d)", tblname, curr * 10 + start, 1);
	if (strcmp(where, "") == 0) {
			httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}

	stTableRecord_t tr;
	char *ptr = (char *)&tr;
	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		char argx[32];
		sprintf(argx, "arg%d", i);
		httpVar *val = httpdGetVariableByName(server, req, argx);

		if (val == NULL) {
			httpdPrintf(server, req, (char *)
				"<div>Error: missing argments!</div>\n"
			);
			return;
		}

		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				int x = 0;
				sscanf(val->value, "%d", &x);

		
				*ptr = x&0xff;

				//printf("%d ", (*ptr)&0xff);

				ptr += ii->len * 1;
			} else {
				strcpy(ptr, val->value);

				//printf("%s ", ptr);

				ptr += ii->len * 1;
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				int x = 0;
				sscanf(val->value, "%d", &x);

				*(int*)ptr = x;

				//printf("%d ", x);

				ptr += ii->len * 4;
			} else {
				ptr += ii->len * 4;
			}
		}
	}

	//printf("\n del %s\n", where);
	ds_update_record(tblname, &tr, where);
	web_render_table(server, req, tblname, curr * 10, 10) ;

}

static void add_func(httpd *server, httpReq *req) {
	httpdDumpVariables(server, req);

	httpVar *val = httpdGetVariableByName(server, req, "curr");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	int curr = atoi(val->value);
	curr--;

	val = httpdGetVariableByName(server, req, "tblname");
	if (val == NULL) {
		httpdPrintf(server, req, (char *)
			"<div>Error: missing argments!</div>\n"
		);
		return;
	}
	char tblname[64];
	strcpy(tblname, val->value);

	int ops = web_get_ops(tblname);
	if ((ops &0x4) == 0) {
		httpdPrintf(server, req, (char *)
			"<div>错误: 没有执行的权限!</div>\n"
		);
		return;
	}



	int num = ds_table_total_record_num(tblname);
	int total = (num + 10-1)/10;

	if (curr >= total) {
		curr = total - 1;
	}

	if (curr < 0) { 
		curr =  0;
	}


	stTableRecord_t tr;
	memset(&tr, 0, sizeof(tr));
	int ret =  ds_insert_record(tblname,&tr);
	ret = ret;

	num = ds_table_total_record_num(tblname);
	total = (num + 10-1)/10;

	web_render_table(server, req, tblname, (total-1)*10, 10) ;
}

/* 
 * db api for server 
 */
/* tblname, {name:value,...} */
static void api_db_insert_func(httpd *server, httpReq *req) {
	printf("[%s] [%d]\n", __func__, __LINE__);
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	printf("[%s] [%d]\n", __func__, __LINE__);
	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	printf("[%s] [%d]\n", __func__, __LINE__);
	char *tblname = (char *)json_get_string(jarg, "tblname");
	if (tblname == NULL) {
		json_decref(jarg);
		return;
	}

	printf("[%s] [%d] : tblname : %s\n", __func__, __LINE__, tblname);
	int ops = web_get_ops(tblname);
	if ((ops &0x4) == 0) {
		json_decref(jarg);
		return;
	}




	printf("[%s] [%d]\n", __func__, __LINE__);
	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;

	json_t *jnewval = json_object_get(jarg, "newval");

	stTableRecord_t tr;
	char *ptr = (char *)&tr;
	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		json_t *jval = json_object_get(jnewval, ii->name);
		if (jval == NULL) {
			continue;
		}
		//char *sval = json_dumps(jval, 0);


		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				int x = 0;
				json_get_int(jnewval,ii->name, &x);
		
				*ptr = x&0xff;

				ptr += ii->len * 1;
			} else {
				char *sval = (char *)json_get_string(jnewval, ii->name);
				sval = sval ? sval : "";
				strcpy(ptr, sval);

				ptr += ii->len * 1;
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				int x = 0;
				json_get_int(jnewval,ii->name, &x);

				*(int*)ptr = x;

				ptr += ii->len * 4;
			} else {
				ptr += ii->len * 4;
			}
		}

		printf("[%s] [%d]\n", __func__, __LINE__);
	}


	ds_insert_record(tblname, &tr);

	json_t *jret = json_object();
	json_object_set_new(jret, "retcode",json_integer(0));
	json_object_set_new(jret, "tblname",json_string(tblname));
	json_object_set_new(jret, "newval",	jnewval);
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);
}
/* tblname, {name:value,...}, {name:value, ...} */
static void api_db_update_func(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *tblname = (char *)json_get_string(jarg, "tblname");
	if (tblname == NULL) {
		json_decref(jarg);
		return;
	}
	int ops = web_get_ops(tblname);
	if ((ops &0x2) == 0) {
		json_decref(jarg);
		return;
	}



	json_t *jmatch = json_object_get(jarg, "match");

	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;

	char where[512];
	int len = 0;
	len += sprintf(where + len, "where ");

	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		json_t *jval = json_object_get(jmatch, ii->name);
		if (jval == NULL) {
			continue;
		}

		char *and  = "";
		if (i > 0) and = " and ";

		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				int x = 0;
				json_get_int(jmatch,ii->name, &x);
		
				len += sprintf(where + len, "%s%s = %d", and, ii->name, x);

			} else {
				char *sval = (char *)json_get_string(jmatch, ii->name);
				sval = sval ? sval : "";

				len += sprintf(where + len, "%s%s = '%s'", and, ii->name, sval);

			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				int x = 0;
				json_get_int(jmatch,ii->name, &x);

				len += sprintf(where + len, "%s%s = %d", and, ii->name, x);

			} else {
			}
		}

	}

	json_t *jnewval = json_object_get(jarg, "newval");

	stTableRecord_t tr;
	char *ptr = (char *)&tr;
	i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		json_t *jval = json_object_get(jnewval, ii->name);
		if (jval == NULL) {
			continue;
		}

		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				int x = 0;
				json_get_int(jnewval,ii->name, &x);
		
				*ptr = x&0xff;

				ptr += ii->len * 1;
			} else {
				char *sval = (char *)json_get_string(jnewval, ii->name);
				sval = sval ? sval : "";
				strcpy(ptr, sval);

				ptr += ii->len * 1;
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				int x = 0;
				json_get_int(jnewval,ii->name, &x);

				*(int*)ptr = x;

				ptr += ii->len * 4;
			} else {
				ptr += ii->len * 4;
			}
		}
	}

	ds_update_record(tblname, &tr, where);

	json_t *jret = json_object();
	json_object_set_new(jret, "retcode",json_integer(0));
	json_object_set_new(jret, "tblname",json_string(tblname));
	json_object_set_new(jret, "match",	jmatch);
	json_object_set_new(jret, "newval",	jnewval);
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);

}
/* tblname, start, count */
static int api_db_select_callback(stTableRecord_t *tr, void *arg) {
	stReq_t *r = (stReq_t *)arg;
	//httpd		*server = r->server;
	//httpReq *req = r->request;
	char *tblname = r->tblname;
	json_t *jrs = (json_t *)r->arg;

	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;

	char *ptr = (char *)tr;
	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		json_t *jval = json_object();
		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
					json_object_set_new(jval, ii->name, json_integer((*ptr)&0xff));
					ptr += ii->len * 1;
			} else {
					json_object_set_new(jval, ii->name, json_string(ptr));
					ptr += ii->len * 1;
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
					json_object_set_new(jval, ii->name, json_integer(*(int*)ptr));
					ptr += ii->len * 4;
			} else {
					ptr += ii->len * 4;
			}
		}
		json_array_append(jrs, jval);
	}

	return 0;
}

static void api_db_select_func(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *tblname = (char *)json_get_string(jarg, "tblname");
	int start = -1; 
	int count = -1; 
	if (tblname == NULL || 
			json_get_int(jarg, "start", &start) != 0 || 
			json_get_int(jarg, "count", &count) != 0) {
		json_decref(jarg);
		return;
	}

	if (start < 0 || count <= 0 || count > 10) {
		json_decref(jarg);
		return;
	}

	/*
	int ops = web_get_ops(tblname);
	if ((ops &0x4) == 0) {
		json_decref(jarg);
		return;
	}
	*/


	//int num = ds_table_total_record_num(tblname);
	//int total = (num + 10-1)/10;


	json_t *rs = json_array();
	stReq_t r = {server, req, tblname, rs};
	ds_search_record(tblname, api_db_select_callback, &r, "limit %d,%d", start, count);
	json_decref(jarg);

	json_t *jret = json_object();
	json_object_set_new(jret, "retcode",json_integer(0));
	json_object_set_new(jret, "tblname",json_string(tblname));
	json_object_set_new(jret, "start",  json_integer(start));
	json_object_set_new(jret, "count",	json_integer(count));
	json_object_set_new(jret, "records",rs);
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);
}

/* tblname, {name:value,...} */
static void api_db_delete_func(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *tblname = (char *)json_get_string(jarg, "tblname");
	if (tblname == NULL) {
		json_decref(jarg);
		return;
	}
	int ops = web_get_ops(tblname);
	if ((ops &0x1) == 0) {
		json_decref(jarg);
		return;
	}


	json_t *jmatch = json_object_get(jarg, "match");

	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;

	char where[512];
	int len = 0;
	len += sprintf(where + len, "where ");

	int i = 0;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
		json_t *jval = json_object_get(jmatch, ii->name);
		if (jval == NULL) {
			continue;
		}

		char *and  = "";
		if (i > 0) and = " and ";

		if (strcmp(ii->type, "char") == 0) {
			if (ii->len == 1) {
				int x = 0;
				json_get_int(jmatch,ii->name, &x);
		
				len += sprintf(where + len, "%s%s = %d", and, ii->name, x);

			} else {
				char *sval = (char *)json_get_string(jmatch, ii->name);
				sval = sval ? sval : "";

				len += sprintf(where + len, "%s%s = '%s'", and, ii->name, sval);

			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				int x = 0;
				json_get_int(jmatch,ii->name, &x);

				len += sprintf(where + len, "%s%s = %d", and, ii->name, x);

			} else {
			}
		}

	}

	ds_delete_record(tblname, where);

	json_t *jret = json_object();
	json_object_set_new(jret, "retcode",json_integer(0));
	json_object_set_new(jret, "tblname",json_string(tblname));
	json_object_set_new(jret, "match",	jmatch);
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);
}

/* tblname, url of db */
static void api_db_import_func(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *urldb = (char *)json_get_string(jarg, "dburl");
	if (urldb == NULL) {
		json_decref(jarg);
		return;
	}

	//system(urldb);

	json_t *jret = json_object();
	json_object_set_new(jret, "retcode",json_integer(0));
	json_object_set_new(jret, "dburl",json_string(urldb));
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);
}
/* tblname */
static void api_db_export_func(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	/*
	char *urldb = (char *)json_get_string(jarg, "dburl");
	if (urldb == NULL) {
		return;
	}
	*/

	//system(urldb);
	char *urldb = "http://www.baidu.com/x/test.db";

	json_t *jret = json_object();
	json_object_set_new(jret, "retcode",json_integer(0));
	json_object_set_new(jret, "dburl",json_string(urldb));
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);

}
/* oldpass, new pass */
static void api_db_setpass_func(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *oldpass = (char *)json_get_string(jarg, "oldpass");
	char *newpass = (char *)json_get_string(jarg, "newpass");
	
	if (oldpass == 0 || newpass == 0) {
		json_decref(jarg);
		return;
	}

	/* set pass */

	json_t *jret = json_object();
	json_object_set_new(jret, "retcode",json_integer(0));
	json_object_set_new(jret, "oldpass",json_string(oldpass));
	json_object_set_new(jret, "newpass",json_string(newpass));
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);
}

/* for clound server */
static void api_devdbm_add_person(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *uuid = (char *)json_get_string(jarg, "uuid");
	char *name = (char *)json_get_string(jarg, "name");
	char *sex  = (char *)json_get_string(jarg, "sex");
	
	if (uuid == 0 || name == 0 || sex == 0) {
		json_decref(jarg);
		return;
	}

	stTableRecord_t tr;
	strncpy(tr.person.name, name, sizeof(tr.person.name));
	strncpy(tr.person.uuid, uuid, sizeof(tr.person.uuid));
	tr.person.sex = sex[0];
	ds_insert_record("person", &tr);

	/* return */
	json_t *jret = json_object();
	json_t *jpayload = json_object();
	json_object_set_new(jret, "status",json_integer(0));
	json_object_set_new(jret, "payload", jpayload);
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);
}
static void api_devdbm_rm_person(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *uuid = (char *)json_get_string(jarg, "uuid");
	
	if (uuid == 0) {
		json_decref(jarg);
		return;
	}

	char where[256];
	sprintf(where, "where uuid = '%s'", uuid);
	ds_delete_record("person", where);

	/* return */
	json_t *jret = json_object();
	json_t *jpayload = json_object();
	json_object_set_new(jret, "status",json_integer(0));
	json_object_set_new(jret, "payload", jpayload);
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);
}

static void api_devdbm_add_card(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *uuid				= (char *)json_get_string(jarg, "uuid");
	char *card_number = (char *)json_get_string(jarg, "card_number");
	char *card_type	  = (char *)json_get_string(jarg, "card_type");
	char *state_			= (char *)json_get_string(jarg, "state_");
	char *es					= (char *)json_get_string(jarg, "effective_start_time");
	char *ee					= (char *)json_get_string(jarg, "effective_end_time");
	
	if (uuid == 0 || card_number == 0 || card_type == 0 || state_ == 0 || es == 0 || ee == NULL) {
		json_decref(jarg);
		return;
	}

	stTableRecord_t tr;
	strncpy(tr.vcard.uuid, uuid, sizeof(tr.vcard.uuid));
	tr.vcard.vtype = atoi(card_type);
	strncpy(tr.vcard.vcardid, card_number, sizeof(tr.vcard.vcardid));
	tr.vcard.state = atoi(state_);
	strncpy(tr.vcard.stime, es, sizeof(tr.vcard.stime));
	strncpy(tr.vcard.etime, ee, sizeof(tr.vcard.etime));
	ds_insert_record("vcard", &tr);

	/* return */
	json_t *jret = json_object();
	json_t *jpayload = json_object();
	json_object_set_new(jret, "status",json_integer(0));
	json_object_set_new(jret, "payload", jpayload);
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);
}

static void api_devdbm_rm_card(httpd *server, httpReq *req) {
	httpVar *val = httpdGetVariableByName(server, req, "argements");
	if (val == NULL || val->value == NULL) {
		return;
	}

	json_error_t err;
	json_t *jarg = json_loads(val->value, 0, &err);
	if (jarg == NULL) {
		return;
	}

	char *uuid = (char *)json_get_string(jarg, "uuid");
	
	if (uuid == 0) {
		json_decref(jarg);
		return;
	}

	char where[256];
	sprintf(where, "where uuid = '%s'", uuid);
	ds_delete_record("vcard", where);

	/* return */
	json_t *jret = json_object();
	json_t *jpayload = json_object();
	json_object_set_new(jret, "status",json_integer(0));
	json_object_set_new(jret, "payload", jpayload);
	char *retstr = json_dumps(jret, 0);
	httpdPrintf(server, req, retstr);
	free(retstr);

	json_decref(jarg);

}


////////////////////////////////////////////////////////////////////////
void web_init(void *_th, void *_fet, const char *ip, int port, const char *base) {
	we.th = _th;
	we.fet = _fet;

	web_start(ip, port, base);

	file_event_reg(we.fet, web_getfd(), web_in, NULL, NULL);
}
void web_step() {
}
void web_push(void *task) {
}
void web_run(void *timer) {
}
void web_in(void *arg, int fd) {
	web_loop();
}
int  web_getfd() {
	return web_socket_get();
}


