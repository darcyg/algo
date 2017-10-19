#include "web.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "ds.h"


static stWebEnv_t we = {
	.ip		= "192.168.0.6",
	.port = 10888,
	.base	= "./www",
	.svr	= NULL,
	.user = "admin",
	.pass = "admin",
	.currpage = 3,
};

static void center_func(httpd *server, httpReq *req);
static void web_render_header(httpd *server, httpReq *req);
static void web_render_center(httpd *server, httpReq *req);
static void web_render_footer(httpd *server, httpReq *req);
static void web_render_menu(httpd *server, httpReq *req);
static void web_render_content(httpd *server, httpReq *req);


static void web_render_status(httpd *server, httpReq *req);
static void web_render_import(httpd *server, httpReq *req);
static void web_render_export(httpd *server, httpReq *req);
static void web_render_modify(httpd *server, httpReq *req);
static void web_render_admin(httpd *server, httpReq *req);
static void web_render_about(httpd *server, httpReq *req);


static stContentHandler_t chs[] = {
	{PAGE_STATUS, web_render_status},
	{PAGE_IMPORT, web_render_import},
	{PAGE_EXPORT, web_render_export},
	{PAGE_MODIFY, web_render_modify},
	{PAGE_ADMIN,	web_render_admin},
	{PAGE_ABOUT,  web_render_about},
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

	{PT_FILE,	"/",				"error",			0,						NULL, INVALID_ITEM,		"error.html",	INVALID_ITEM},
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
	return 0;
}

int web_loop() {
	while (1) {
		struct timeval timeout;
		timeout.tv_sec = 5;
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
		"	</head>\n"\
		"	<body>\n"\
		"		<div id=\"header\">\n"\
		"		</div>\n"
	);
}
static void web_render_footer(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char *)
		"		<div id=\"footer\">\n" \
		"		</div>\n"\
		"	</body>\n"\
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
	web_render_content(server, req);

	httpdPrintf(server, req, (char*)
		"		</div>\n"
	);
}
static void web_render_menu(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char*)
		"			<div id=\"menu\" style=\"width:20%; float: left\">\n"
	);


	httpdPrintf(server, req, (char *)
		"				<ul>\n"
	);

	stMenuItem_t ms[] = {
		{"4.jpg", "Status",			"Current Status",		0,			NULL},
		{"4.jpg", "Import",			"Import DataBase",	0,			NULL},
		{"4.jpg", "Export",			"Export DataBase",	0,			NULL},
		{"4.jpg", "Modify",			"Modify DataBase",	0,			NULL},
		{"4.jpg", "Admin",			"Setting Admin",		0,			NULL},
		{"4.jpg", "About",			"About Info",				0,			NULL},
	};

	int cnt = sizeof(ms)/sizeof(ms[0]);
	int i = 0;
	for (i = 0; i < cnt; i++) {
		stMenuItem_t *mi = &ms[i];
	httpdPrintf(server, req, (char *)
		"					<li>\n"
		"						<div style=\"cursor:pointer\" align=\"center\">\n"
		"							<div style=\"position:relative;\"><img src=\"image/%s\" style=\"position:absolute;top:0; left:0;clip:rect(0px 200px 400px 0px)\"></img></div>\n"
		"							<p><strong>%s</strong></p>\n"
		"							<p><strong>%s</strong></p>\n"
		"						</div>\n"
		"					</li>\n",
		mi->img,
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
static int db_test_search_callback_1(stTableRecord_t *tr, void *arg) {
	stReq_t *r = (stReq_t *)arg;
	httpd		*server = r->server;
	httpReq *req = r->request;
	char *tblname = r->tblname;

	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;


	
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
					"								<th>%d</th>\n", *ptr);
					ptr += ii->len * 1;
			} else {
				httpdPrintf(server, req, (char *)
					"								<th>%s</th>\n", ptr);
					ptr += ii->len * 1;
			}
		} else if (strcmp(ii->type, "int") == 0) {
			if (ii->len == 1) {
				httpdPrintf(server, req, (char *)
					"								<th>%d</th>\n", *(int*)ptr);
					ptr += ii->len * 4;
			} else {
				httpdPrintf(server, req, (char *)
					"								<th>%s</th>\n", "-");
					ptr += ii->len * 4;
			}
		}
	}
	httpdPrintf(server, req, (char *)
		"								<td><button>-</button>  <button>m</button> <button>+</button></td>\n"
	);

	httpdPrintf(server, req, (char *)
		"							</tr>\n"
	);

	return 0;
}


static void web_render_content(httpd *server, httpReq *req) {
	httpdPrintf(server, req, (char*)
		"			<div id=\"content\"  style=\"width:70%; float:right\">\n"
	);


	int i = 0;
	for (i = 0; i < sizeof(chs)/sizeof(chs[0]); i++) {
		stContentHandler_t *ch = &chs[i];
		if (ch->page == we.currpage) {
			ch->function(server, req);
			break;
		}
	}

	httpdPrintf(server, req, (char *)
		"			</div>\n"
	);
}

static void web_render_status(httpd *server, httpReq *req) {
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
static void web_render_modify(httpd *server, httpReq *req) {

	const char *tablenames[] = {
		"basicinfo",
		"person",
		"device",
		"vcard",
		"vcard_person",
		"vcard_device",
		"device_status",
		"lock_record",
		"device_alarm",
		"log",
	};

	httpdPrintf(server, req, (char *)
		"				<div class=\"disarea\">\n"
	);
	httpdPrintf(server, req, (char *)
		"					<div class=\"title\">\n"
		"						<p display=\"inline\">ModifyDataBase</p>\n"
		"						<select type=\"select\" display=\"inline\">\n");

		int i = 0;
		for (i = 0; i < sizeof(tablenames)/sizeof(tablenames[0]); i++) {
			httpdPrintf(server, req, "							<option>%s</option>\n", tablenames[i]);
		}
	httpdPrintf(server, req, (char *)
		"						</select>\n"
		"						<strong>Total 100, </strong>\n"
		"						<strong>Current 10</strong>\n"
		"						<button>Prev</button>\n"
		"						<button>Next</button>\n"
		"					</div>\n"
	);

	httpdPrintf(server, req, (char *)
		"					<div class=\"hline\">\n"
		"						<hr>\n"
		"					</div>\n"
	);


	//httpdPrintf(server, req, (char *)
	//	"				<img src=\"image/2014020715592025845.jpg\"  alt=\"ShangHai\" />\n"
	//);


	httpdPrintf(server, req, (char *)
		"					<div class=\"table\">\n"
		"						<table border=\"1\">\n"
		"							<tr>\n");
	
	//const char *tblname = "basicinfo";
	const char *tblname = "log";
	stTableInfo_t ti;
	int ret = ds_table_info(tblname, &ti);
	ret = ret;
	for (i = 0; i < ti.itemcnt; i++) {
		stItemInfo_t *ii = &ti.items[i];
	httpdPrintf(server, req, (char *)
		"								<th>%s(%s[%d])</th>\n", ii->name, ii->type, ii->len);
	}
	httpdPrintf(server, req, (char *)
		"								<th>Opeation</th>\n"
		"							</tr>\n");

		stReq_t r = {server, req, tblname};
		ds_search_record(tblname, db_test_search_callback_1, &r, "1 = 1 limit 10");

		/*
		"							<tr>\n");
		"								<td>1</td>\n"
		"								<td>Kevne</td>\n"
		"								<td>-----</td>\n"
		"								<td>HangZhou</td>\n"
		"								<td>WAngtong</td>\n"
		"								<td><button>-</button>  <button>m</button> <button>+</button></td>\n"
		"							</tr>\n"
		*/
	httpdPrintf(server, req, (char *)
		"						</table>\n"
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
		"						<p>Modify Admin</p>\n"
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
		"					<div class=\"Submit\">\n"
		"						<input type=\"button\" value=\"Submit\"></input>\n"
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



