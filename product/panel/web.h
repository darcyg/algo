#ifndef __WEB_H_
#define __WEB_H_

#include "lweb.h"

#ifdef __cplusplus 
extern "C" {
#endif



typedef void (*WEB_FUNCTION)(httpd *server, httpReq *req);
typedef int  (*WEB_PRELOAD)(httpd *server,	httpReq *req);


typedef enum emPageType {
	PT_CFUNC	= 0x00,
	PT_FILE		= 0x01,
	PT_WILD		= 0x02,
	PT_CWILD	= 0x03,
	PT_STATIC = 0x04,
}emPageType_t;

#define INVALID_ITEM	0

typedef struct stWebNode {
	emPageType_t		type;
	char						*dir;
	char						*name;
	int							isindex;
	WEB_PRELOAD			preload;
	WEB_FUNCTION		function;
	char						*path;
	char						*html;
}stWebNode_t;


typedef struct stWebEnv {
	char						ip[32];
	int							port;
	char						base[256];
	httpd						*svr;

	char						user[64];
	char						pass[64];

	//int							currpage;
}stWebEnv_t;

typedef struct stReq {
	httpd *server;
	httpReq *request;
	char		*tblname;
	void		*arg;
}stReq_t;

typedef struct stMenuItem {
	char					*img;
	char					*menuname;
	char					*description;
	int						subcnt;
	struct stMenuItem_t *sub;
}stMenuItem_t;

enum {
	PAGE_STATUS = 0,
	PAGE_IMPORT = 1,
	PAGE_EXPORT = 2,
	PAGE_MODIFY = 3,
	PAGE_ADMIN  = 4,
	PAGE_ABOUT  = 5,
};

typedef struct stContentHandler {
	int						page;
	WEB_FUNCTION	function;
}stContentHandler_t;




int web_start(const char *ip, int port, const char *base);
int web_socket_get();
int web_loop();


#ifdef __cplusplus
}
#endif


#endif
