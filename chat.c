//gcc -o chat.out -std=c99 -llolie -lwebsockets -lircinterface -I../lib chat.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <libwebsockets.h>
#include <lolie/LinkedList.h>
#include <lolie/Math.h>
#include <lolie/Memory.h>

#define WRITEBUFFER_LENGTH 512
unsigned char* writeBuffer=NULL;

#define NICKNAME_MAXLENGTH 32
struct Session{
	struct libwebsocket* wsi;
	void* user;
	char* nickname;
};

LinkedList/*<struct Session*>*/* sessionList;

void print_timestamp(FILE* file){
	static time_t timeData;
	static struct tm* timeInfo;
	static char buffer[20];//YYYY-MM-DD HH:MM:SS

	time(&timeData);
	timeInfo=localtime(&timeData);
	strftime(buffer,20,"%Y-%m-%d %H:%M:%S",timeInfo);
	fprintf(file,"[%s] ",buffer);
}

static void sendWriteBuffer(size_t len){
	LinkedList_forEach(sessionList,node)
		libwebsocket_write(((struct Session*)node->ptr)->wsi,writeBuffer+LWS_SEND_BUFFER_PRE_PADDING,len,LWS_WRITE_TEXT);
	
	//Output logging
	print_timestamp(stdout);
	printf(">> [%u] %.*s\n",writeBuffer[LWS_SEND_BUFFER_PRE_PADDING],len-1,writeBuffer+LWS_SEND_BUFFER_PRE_PADDING+1);//http://stackoverflow.com/questions/5189071/print-part-of-char-array
}

static int protocol_http_func(struct libwebsocket_context* context,struct libwebsocket* wsi,enum libwebsocket_callback_reasons reason,void* user,void* in,size_t len){
	return 0;
}


#define LOLIROFLEECHO_MESSAGETYPE_OUT_UNKNOWN     0
#define LOLIROFLEECHO_MESSAGETYPE_OUT_CONNECT     1
#define LOLIROFLEECHO_MESSAGETYPE_OUT_DISCONNECT  2
#define LOLIROFLEECHO_MESSAGETYPE_OUT_MESSAGE     3
#define LOLIROFLEECHO_MESSAGETYPE_OUT_SETNICKNAME 4

#define LOLIROFLEECHO_MESSAGETYPE_IN_UNKNOWN     0
#define LOLIROFLEECHO_MESSAGETYPE_IN_MESSAGE     3
#define LOLIROFLEECHO_MESSAGETYPE_IN_SETNICKNAME 4

static int protocol_lolirofle_echo_func(struct libwebsocket_context* context,struct libwebsocket* wsi,enum libwebsocket_callback_reasons reason,void* user,void* in,size_t len){
	switch(reason){
		case LWS_CALLBACK_ESTABLISHED:{
			//Output logging
			print_timestamp(stdout);
			printf("Connection established: %lu\n",user);

			//Add connection to list
			struct Session* session=smalloc(sizeof(struct Session));
			session->wsi=wsi;
			session->user=user;
			session->nickname=smalloc(3+1);
			strcpy(session->nickname,"???");
			LinkedList_push(&sessionList,session);

			//Send response
			size_t writeBufferLength=snprintf(writeBuffer+LWS_SEND_BUFFER_PRE_PADDING,WRITEBUFFER_LENGTH,"%c%s",LOLIROFLEECHO_MESSAGETYPE_OUT_CONNECT,session->nickname);
			sendWriteBuffer(writeBufferLength);

		}	break;

		case LWS_CALLBACK_CLOSED:{
			//Output logging
			print_timestamp(stdout);
			printf("Connection closed: %lu \n",user);

			//Remove connection to list
			struct Session* session;
			if((session=LinkedList_removeFirst(&sessionList,function(bool,(void* elem){return ((struct Session*)elem)->user==user;})))){
				//Buffer response
				size_t writeBufferLength=snprintf(writeBuffer+LWS_SEND_BUFFER_PRE_PADDING,WRITEBUFFER_LENGTH,"%c%s",LOLIROFLEECHO_MESSAGETYPE_OUT_DISCONNECT,session->nickname);
				sendWriteBuffer(writeBufferLength);

				free(session->nickname);
				free(session);
			}else{
				print_timestamp(stderr);
				fputs("Error: Unable to remove connection from list",stderr);
			}

		}	break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			//Output logging
			print_timestamp(stderr);
			printf("Connection handshake failed: %lu\n",user);

			break;

		case LWS_CALLBACK_RECEIVE:{
			//Input logging
			print_timestamp(stdout);
			printf("<< %lu: [%u] %s\n",user,((char*)in)[0],(char*)in+1);

			struct Session* session;
			if((session=LinkedList_find(sessionList,function(bool,(void* elem){return ((struct Session*)elem)->user==user;})))){
				switch(((char*)in)[0]){
					case LOLIROFLEECHO_MESSAGETYPE_IN_MESSAGE:{
						//Buffer response
						size_t writeBufferLength=snprintf(writeBuffer+LWS_SEND_BUFFER_PRE_PADDING,WRITEBUFFER_LENGTH,"%c%s:",LOLIROFLEECHO_MESSAGETYPE_OUT_MESSAGE,session->nickname);
						size_t inLength=MIN(len-1,WRITEBUFFER_LENGTH-writeBufferLength);
						memcpy(writeBuffer+LWS_SEND_BUFFER_PRE_PADDING+writeBufferLength,in+1,inLength);
						writeBufferLength+=inLength;
						sendWriteBuffer(writeBufferLength);
					}	break;

					case LOLIROFLEECHO_MESSAGETYPE_IN_SETNICKNAME:{
						//Buffer response
						size_t writeBufferLength=snprintf(writeBuffer+LWS_SEND_BUFFER_PRE_PADDING,WRITEBUFFER_LENGTH,"%c%s:",LOLIROFLEECHO_MESSAGETYPE_OUT_SETNICKNAME,session->nickname);
						size_t inLength=MIN(len-1,WRITEBUFFER_LENGTH-writeBufferLength);

						//If nickname isn't occupied (TODO: Bad way of checking with memcmp and a null-terminated string)
						if(!LinkedList_find(sessionList,function(bool,(void* elem){return memcmp(((struct Session*)elem)->nickname,in+1,inLength)==0  && ((struct Session*)elem)->nickname[inLength]=='\0';}))){
							//Check for invalid characters
							size_t i;
							for(i=1;i<inLength;++i)
								if(!isgraph(((char*)in)[i]) || ((char*)in)[i]==':' || ((char*)in)[i]==' ')
									break;
							if(i<inLength)
								break;

							//Finish buffering and send
							memcpy(writeBuffer+LWS_SEND_BUFFER_PRE_PADDING+writeBufferLength,in+1,inLength);
							writeBufferLength+=inLength;
							sendWriteBuffer(writeBufferLength);

							//Allocate new nickname 
							free(session->nickname);
							session->nickname=smalloc(inLength+1);
							memcpy(session->nickname,in+1,inLength);
							session->nickname[inLength]='\0';
						}
					}	break;
				}
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
	{"lolirofle-echo",protocol_lolirofle_echo_func,sizeof(unsigned long)},
	{NULL,NULL,0}//End of list
};

int main(void){
	//Initialize info structure for context creation
	info=(struct lws_context_creation_info){
		.port                     = 2001,
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
	
	//Allocate write buffe with pre and post padding.
	writeBuffer = malloc(LWS_SEND_BUFFER_PRE_PADDING+WRITEBUFFER_LENGTH+LWS_SEND_BUFFER_POST_PADDING);

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
		libwebsocket_service(context,2000);
	}

	libwebsocket_context_destroy(context);

	//Free write buffer
	free(writeBuffer);
	writeBuffer=NULL;

	return 0;
}
