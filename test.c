//gcc -o chat.out -std=c99 -llolie -lwebsockets -lircinterface -I../lib chat.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <libwebsockets.h>

static int protocol_http_func(struct libwebsocket_context* context,struct libwebsocket* wsi,enum libwebsocket_callback_reasons reason,void* user,void* in,size_t len){
	return 0;
}

static int protocol_test(struct libwebsocket_context* context,struct libwebsocket* wsi,enum libwebsocket_callback_reasons reason,void* data,void* in,size_t len){
    switch(reason){
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("Connection established\n");
            break;
        case LWS_CALLBACK_RECEIVE: { // the funny part
            printf("%.*s\n",len,(char*)in);
            break;
        }
        default:
            break;
    }
   
   
    return 0;
}

static struct libwebsocket_context* context;
static struct lws_context_creation_info info;
static struct libwebsocket_protocols protocols[]={
	//(protocol name,callback,per session data size)
	{"http-only"     ,protocol_http_func          ,0},//HTTP handler first is required
	{"test",(int(*)(struct libwebsocket_context*,struct libwebsocket*,enum libwebsocket_callback_reasons,void*,void*,size_t))protocol_test,sizeof(struct Session*)},
	{NULL,NULL,0}//End of list
};

int main(){
   info=(struct lws_context_creation_info){
		.port                     = 2001,
		.iface                    = NULL,
		.protocols                = protocols,
		.extensions               = libwebsocket_get_internal_extensions(),
		.ssl_cert_filepath        = NULL,
		.ssl_private_key_filepath = NULL,
		.ssl_ca_filepath          = NULL,
		.gid                      = -1,
		.uid                      = -1,
		.options                  = 0,
		.ka_time                  = 15,
		.ka_probes                = 3,
		.ka_interval              = 5
	};

	//Create libwebsocket context
	if(!(context=libwebsocket_create_context(&info))){
		fprintf(stderr,"Error: libwebsocket failed when creating context\n");
		return -1;
	}

	fputs("Server started\n",stdout);

	while(1){
		//Process pending events and then sleep specified amount of ms.
		libwebsocket_service(context,1500);
	}

	libwebsocket_context_destroy(context);
   
    return 0;
}
