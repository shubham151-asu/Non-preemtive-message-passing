#include "sem.h"

#define N 12 	    // each packet size is 4 INT hence multiple of 4 is used
#define MAXCLIENT 3 // MAX number of clients, the server can handle
#define MAXSIZE 1048576 //MAX size of file, server can receive which 1 MB

#define PORT_REQ 20  // used by clients to request a port from server 
#define SEND 21      // used by clients to alert server with incoming message 
#define RECIEVE 22   // not used but can be used if in a two way exchange environment


#define ACK 200      //reponse from the server that operation has completed 
#define SYNC_ACK 202 //operation completed but should continue by sending next packet
#define ERROR 400    //error has happened during the operation

#define FILE_NAME 50 //used by clients to alert the server that incoming message has filename
#define FILE_DATA 51 //used by clients to alert the server that incoming message has filedata

#define FILE_NAME_SIZE_ERROR 100  // errorcode for filename size exceeding 15 characters
#define FILE_TOTAL_SIZE_ERROR 101 // errorcode for file size exceeding 1 MB
#define STOP 102                  // abort

char serverfilename[3][23]; // used by server to store modified file name of the files from clients
int serverclienthistory[MAXCLIENT] = {0,0,0}; // server stores the record of clients which it is 						      // servicing

int prod_delay = 1, cons_delay = 1; //optional can be used

typedef struct Request{ 	// Packet used by clients to send message to server 
    int opcode;
    int req_type;
    int req_data;
    int address;
}Request;

typedef struct Response{	// Packet used by server to send reply to clients 
   int status;
   int res_data;
   int errorcode;
   int address;
}Response;


typedef struct port       // stucture to define the ports
	{
	int message[N];
	int out,in;
	semaphore_t empty;
	semaphore_t full;
	semaphore_t mutex;
}port;

/* 
	EVERY REQUEST IS FOLLOWED BY RECIEVE THAT MEANS TWO WAY MESSAGE PASSING IS HAPPENING  .
	 ALL MESSAGE COME IN SINGLE SERVER PORT AND NO EXTRA PORT USED 
*/


void sendRequest(port *port_number, Request request)  //used by clients to send request
{
	//printf(" Client sending request to server\n");
        P(&port_number->empty);
	P(&port_number->mutex);
	port_number->message[port_number->in] = request.opcode;	
	port_number->in = (port_number->in+1) % N;
	port_number->message[port_number->in] = request.req_type;
	port_number->in = (port_number->in+1) % N;
        port_number->message[port_number->in] = request.req_data;
        port_number->in = (port_number->in+1) % N;
        port_number->message[port_number->in] = request.address;
        port_number->in = (port_number->in+1) % N;
	V(&port_number->mutex);
	V(&port_number->full);
	//printf(" Client succeesfully sent request to server\n");
	//sleep(prod_delay);
}

void sendResponse(port *port_number, Response response)  //used by server to send response
{
        //printf(" Server sending request to the client\n");
        P(&port_number->empty);
        P(&port_number->mutex);
        port_number->message[port_number->in] = response.status;
        port_number->in = (port_number->in+1) % N;
        port_number->message[port_number->in] = response.res_data;
        port_number->in = (port_number->in+1) % N;
        port_number->message[port_number->in] = response.errorcode;
        port_number->in = (port_number->in+1) % N;
        port_number->message[port_number->in] = response.address;
        port_number->in = (port_number->in+1) % N;
        V(&port_number->mutex);
        V(&port_number->full);
        //sleep(prod_delay);

}

Request recieveRequest(port *port_number)   // used by server to recieve request
{
	//printf(" Server waiting to recive client request\n");
	Request request;
	P(&port_number->full);
	P(&port_number->mutex);
	request.opcode = port_number->message[port_number->out];   
	port_number->out = (port_number->out+1)%N;
	request.req_type = port_number->message[port_number->out];
	port_number->out = (port_number->out+1)%N;
        request.req_data = port_number->message[port_number->out];
        port_number->out = (port_number->out+1)%N;
        request.address = port_number->message[port_number->out];
        port_number->out = (port_number->out+1)%N;
	V(&port_number->mutex);	
	V(&port_number->empty);
	//sleep(cons_delay);
	//printf(" Server recived to recive client request\n");
	return request;
}

Response recieveResponse(port *port_number) //used by client to recieve response from server 
{
	Response response;
	//printf(" Client Waiting to recieve server response\n");
        P(&port_number->full);
        P(&port_number->mutex);
        response.status = port_number->message[port_number->out];  
        port_number->out = (port_number->out+1)%N;
        response.res_data = port_number->message[port_number->out];
        port_number->out = (port_number->out+1)%N;
        response.errorcode = port_number->message[port_number->out];
        port_number->out = (port_number->out+1)%N;
        response.address = port_number->message[port_number->out];
        port_number->out = (port_number->out+1)%N;
        V(&port_number->mutex);
        V(&port_number->empty);
        //sleep(cons_delay);
        return response;

}


/* used by server to indentify the filename "XXXX.server" to put the clients file data */
int lookclientaddress(int clientaddress){           
	int i ;
	for(i = 0;i<MAXCLIENT;i++){
		if (serverclienthistory[i]==clientaddress)
			break;
	}
	//printf("found address %d of client\n",i);
	return i;
}

/* used by server to respond if port request made by clients can be done, if feasible response is ACK else error is thrown which means "DO NOT DO THAT"
For every address having "0", the server can handle the clients and the address is updated with clients address */

int assignclienttoserver(){
	int i ;
	for(i=0;i<MAXCLIENT;i++){
		if (serverclienthistory[i]==0)
			break;	
	}
	//printf("Assiging address %d to client\n",i);
	return i;
}

/* used by server to identify if the filesize has crossed 1MB */

int checkfilesize(FILE *fp){
	long int current = ftell(fp);
	int res;
	fseek(fp,0L,SEEK_SET);
	//printf("start length %ld",ftell(fp));
	//printf("Difference %ld\n",current - ftell(fp));
	if (current-ftell(fp)>MAXSIZE)
		res = 1;
	else
		res = 0;
	fseek(fp,0L,SEEK_END);
	//printf("response res %d",res);
	return res;
}	

/* 
   Server parse the packet, identfies the type of request and sends the response to clients 
   Request are of three types 
   1) PORT_REQ : Clients request the server that it wants to upload a file
   2) SEND : Request used by client two send message to server
	     There are two catogories of request identified by req_type
		1) FILE_NAME : send by clients to identify message is FILE NAME
		2) FILE_DATA : send by clients to identify message is FILE DATA
   3) RECIEVE : not used but if the server wants to upload data to clients this can be used 
*/
	

Response generateresponse(Request req)
{
	Response response;	
	response.address = req.address;
	//printf("client address %d\n",req.address);
	if(req.opcode==PORT_REQ)
	{ 
		if(assignclienttoserver()<MAXCLIENT)
		{
			response.status = ACK;
			serverclienthistory[assignclienttoserver()] = req.address;
		}
		else
		{
			response.status = ERROR;
			response.errorcode = STOP;
		}
	}
	
	if(req.opcode==SEND)
	{
		if (req.req_type==FILE_NAME)
			{
			char filename[15] ;
			strcpy(filename,serverfilename[lookclientaddress(req.address)]);
			//printf("Filename from the server %s\n",serverfilename[req.address]);
			char data = (char)req.req_data;
			strncat(filename,&data,1);
			if (req.req_data!=-1){
				strcpy(serverfilename[lookclientaddress(req.address)],filename);
				response.status = SYNC_ACK;
			
			}
			else
			{
				if(strlen(filename)>16)
				{
					response.status = ERROR;
					response.errorcode = FILE_NAME_SIZE_ERROR;
					int clientaddress = lookclientaddress(req.address);
					strcpy(serverfilename[clientaddress],"");
					serverclienthistory[clientaddress] = 0;
				}
				else
				{
			 strncat(serverfilename[lookclientaddress(req.address)],".server",7);
					response.status = ACK;
	printf("Filename at the server %s\n",serverfilename[lookclientaddress(req.address)]);
				}
			}
			}
		
		else if (req.req_type==FILE_DATA)
			{	
				//printf("req recived at the server %c\n",(char)req.req_data);
				if (req.req_data!=-1)
				{
					char filename[25];
				  strcpy(filename,serverfilename[lookclientaddress(req.address)]);
					FILE *file = fopen(filename,"a");
					if (file == NULL)
						printf(" failed to open file filename %s",filename);
					else
					{
						if (!checkfilesize(file))
						{
							char data = (char)req.req_data;
							fputc(data,file);
						}
						else
						{
							response.status = ERROR;
							response.errorcode = FILE_TOTAL_SIZE_ERROR;
							fclose(file);
						int clientaddress = lookclientaddress(req.address);
							strcpy(serverfilename[clientaddress],"");
							serverclienthistory[clientaddress] = 0;
							//assignclienttoserver();
							return response;
						}
						
					}
					fclose(file);
					file = NULL;
					response.status = SYNC_ACK;

				}
				else
				{
					response.status = ACK;
					int clientaddress = lookclientaddress(req.address);
					strcpy(serverfilename[clientaddress],"");
					serverclienthistory[clientaddress] = 0;
						
				}
			}
					
		
	}
	
	return response;
}

