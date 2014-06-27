//gcc -O3 -o websocket_ircredirect -std=c99 -llolie -lwebsockets -lircinterface websocket_ircredirect.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <libwebsockets/libwebsockets.h>
#include <lolie/LinkedList.h>
#include <lolie/Math.h>
#include <lolie/Memory.h>
#include <lolie/Stringp.h>
#include <lolien/types.h>
#include <lolie/utf8.h>
#include <ircinterface/irc.h>
#include <tinycthread.h>

#define IRC_CHANNEL "#toa"
#define NICKNAME_MAXLENGTH 32
struct Session{
	struct libwebsocket* wsi;
	char* nickname;
	irc_connection irc;
	thrd_t thread;
	bool alive;
};

void print_timestamp(FILE* file){
	static time_t timeData;
	static struct tm* timeInfo;
	static char buffer[20];//YYYY-MM-DD HH:MM:SS

	time(&timeData);
	timeInfo=localtime(&timeData);
	strftime(buffer,20,"%Y-%m-%d %H:%M:%S",timeInfo);
	fprintf(file,"[%s] ",buffer);
}

#define WRITEBUFFER_LENGTH 512
static unsigned char writeBuffer_raw[LWS_SEND_BUFFER_PRE_PADDING+WRITEBUFFER_LENGTH+LWS_SEND_BUFFER_POST_PADDING];
static unsigned char* writeBuffer=writeBuffer_raw+LWS_SEND_BUFFER_PRE_PADDING;

static void session_send(struct Session* session,Stringcp str){
	//Copy to buffer
	size_t len=utf8_correct(str,STRINGP((char*)writeBuffer,WRITEBUFFER_LENGTH));

	//Send buffer to client
	libwebsocket_write(session->wsi,writeBuffer,len,LWS_WRITE_TEXT);

	//Output logging
	print_timestamp(stdout);
	printf(">> %.*s\n",(int)str.length,writeBuffer);
}

static int protocol_http_func(struct libwebsocket_context* context,struct libwebsocket* wsi,enum libwebsocket_callback_reasons reason,void* user,void* in,size_t len){
	return 0;
}

static int protocol_lolirofle_ircredirect(struct libwebsocket_context* context,struct libwebsocket* wsi,enum libwebsocket_callback_reasons reason,struct Session** userSession,void* in,size_t len){
	switch(reason){
		//When client connects
		case LWS_CALLBACK_ESTABLISHED:{
			//Output logging
			print_timestamp(stdout);
			printf("Connection established: %p\n",userSession);

			//Allocate session for user storage
			*userSession=smalloc(sizeof(struct Session));

			//Initialize structure members with data
			(*userSession)->alive=true;
			(*userSession)->wsi=wsi;
			(*userSession)->nickname=smalloc(NICKNAME_MAXLENGTH);
			libwebsockets_get_peer_addresses(context,wsi,libwebsocket_get_socket_fd(wsi),(*userSession)->nickname,NICKNAME_MAXLENGTH,(char*)writeBuffer,0);
			for(char* i=(*userSession)->nickname;*i!='\0';++i)
				if(!isalnum(*i))
					*i='_';

			//Connect to IRC server
			(*userSession)->irc=irc_connect("flygande-toalett.tk",1568);
			irc_set_username(&(*userSession)->irc,"Webchat",(*userSession)->nickname);
			irc_set_nickname(&(*userSession)->irc,(*userSession)->nickname);

			//Create thread that reads IRC messages
			thrd_create(&(*userSession)->thread,function(int,(void* session){
				Stringp writeBuffer;
				writeBuffer.ptr=malloc(WRITEBUFFER_LENGTH);
				writeBuffer.length=WRITEBUFFER_LENGTH;

				while(((struct Session*)session)->alive &&
					irc_read_message(&((struct Session*)session)->irc,function(void,(const irc_connection* connection,const irc_message* message){
						switch(message->command_type){
							case IRC_MESSAGE_TYPE_NUMBER:
								if(message->command_type_number == 1){
									irc_join_channel(&((struct Session*)session)->irc,IRC_CHANNEL);
								}
								break;							
							default:
								session_send((struct Session*)session,message->raw_message);
								putchar('\n');
								break;
						}
					}))
				);

				free(writeBuffer.ptr);
				return 0;
			}),*userSession);

		}	break;

		//When client disconnects
		case LWS_CALLBACK_CLOSED:{
			//Output logging
			print_timestamp(stdout);
			printf("Connection closed: %p \n",userSession);

			//Disconnect
			thrd_t thread;
			thrd_create(&thread,(int(*)(void*))function(int,(struct Session* session){
				session->alive=false;
				thrd_join(session->thread,NULL);//TODO: Bug: If nothing happens (no events from server) and someone tries to join from the client, they will have to wait because of thrd_join waiting for the thread to be killed (read() blocks)
				irc_disconnect(&session->irc);

				free(session->nickname);
				free(session);

				return 0;
			}),*userSession);
		}	break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			//Output logging
			print_timestamp(stderr);
			printf("Connection handshake failed: %p\n",userSession);

			break;

		//When receiving a message from client
		case LWS_CALLBACK_RECEIVE:{
			//Input logging
			print_timestamp(stdout);
			printf("<< %p: %s\n",userSession,(char*)in);

			if(((char*)in)[0]=='/'){
				if(len>1+4+1 && (memcmp(in+1,"nick ",4+1)==0 || memcmp(in+1,"name ",4+1)==0)){
					free((*userSession)->nickname);
					(*userSession)->nickname=smalloc(len-(1+4+1)+1);
					memcpy((*userSession)->nickname,in+1+4+1,len-(1+4+1));
					(*userSession)->nickname[len-(1+4+1)]='\0';
					irc_set_nickname(&(*userSession)->irc,(*userSession)->nickname);
				}
				else if((len>1+4+1 && (memcmp(in+1,"quit ",4+1)==0) || memcmp(in+1,"exit ",4+1)==0 || memcmp(in+1,"part ",4+1)==0) ||
				        (len>1+10+1 && memcmp(in+1,"disconnect ",10+1)==0) ||
				        (len>1+6+1 && memcmp(in+1,"logout ",6+1)==0)
				){
					protocol_lolirofle_ircredirect(context,wsi,LWS_CALLBACK_CLOSED,userSession,in,len);
				}
				else if((len>1+5+1 && (memcmp(in+1,"users ",5+1)==0) || memcmp(in+1,"names ",5+1)==0) ||
				        (len>1+6+1 && memcmp(in+1,"online ",6+1)==0)
				){
					protocol_lolirofle_ircredirect(context,wsi,LWS_CALLBACK_CLOSED,userSession,in,len);
				}
			}else{
				size_t utf8_len=utf8_correct(STRINGCP(in,len),STRINGP((char*)writeBuffer,WRITEBUFFER_LENGTH));
				irc_send_message(&(*userSession)->irc,STRINGCP(IRC_CHANNEL,sizeof(IRC_CHANNEL)),STRINGCP((char*)writeBuffer,utf8_len));
			}
		}	break;

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
	{"lolirofle-ircredirect",(int(*)(struct libwebsocket_context*,struct libwebsocket*,enum libwebsocket_callback_reasons,void*,void*,size_t))protocol_lolirofle_ircredirect,sizeof(struct Session*)},
	{NULL,NULL,0}//End of list
};

int main(void){
	//Initialize info structure for context creation
	info=(struct lws_context_creation_info){
		.port                     = 1569,
		.iface                    = NULL,
		.protocols                = protocols,
		.extensions               = libwebsocket_get_internal_extensions(),
		.ssl_cert_filepath        = NULL,
		.ssl_private_key_filepath = NULL,
		.ssl_ca_filepath          = NULL,
		.ssl_cipher_list          = NULL,
		.gid                      = -1,
		.uid                      = -1,
		.options                  = 0,
		.ka_time                  = 15,
		.ka_probes                = 3,
		.ka_interval              = 5
	};

	//Create libwebsocket context
	if(!(context=libwebsocket_create_context(&info))){
		print_timestamp(stderr);
		fprintf(stderr,"Error: libwebsocket failed when creating context\n");
		return -1;
	}

	print_timestamp(stdout);
	fputs("Server started\n",stdout);

	while(1){
		//Process pending events and then sleep specified amount of ms.
		libwebsocket_service(context,1500);
	}

	libwebsocket_context_destroy(context);

	return 0;
}
