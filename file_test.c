#include <stdbool.h>
#include "message.h"


port ports[100]; // Kernel globals can be used to establish the message passing Interface. Upto 100 clients and server ports can work together
char clientfilenames[10][20];  // fileNames are copied in the global variables.


void initialize_port(int port_number)
{
	memset(ports[port_number].message, 0, N*sizeof(int));
	init_sem(&ports[port_number].empty,N);
	init_sem(&ports[port_number].full,0);
	init_sem(&ports[port_number].mutex,4);  // DATA PACKET SIZE IS 4
	ports[port_number].in = 0;
	ports[port_number].out = 0;
}



void server(int server_address)
{
	
	while(1) // server loop
	{
		Request request = recieveRequest(&ports[server_address]); // SEE COMPLETE CODE IN MESSAGE.H
		Response response = generateresponse(request);
		sendResponse(&ports[response.address], response);
	}
}


void clients(int client_id, int server_address) 
{
	
	char filename[20];
	Request request;
	Response response;
	request.opcode = PORT_REQ; //FIRST CLIENT REQUEST FOR A SERVER PORT 
	request.address = client_id; // clients local address
	strcpy(filename,clientfilenames[request.address-1]); 
	sendRequest(&ports[server_address],request);
	response.status = 6;
	bool check = false;

	while(response.status!=ACK){
		response = recieveResponse(&ports[request.address]);
		if (response.status!=ACK){ // if server is servicing other clients this message will get displayed until server gets free
			if (response.status==ERROR && !check){
				printf(" client %d DO NOT DO THAT \n",request.address); 
				check = true;
			}
  
		sendRequest(&ports[server_address],request);
		}
	}

	request.opcode = SEND; 
	request.req_type = FILE_NAME;
	printf("filename %s has filename length %d\n",filename, (int)strlen(filename));
	for (int i=0;i<strlen(filename);i++)
	{
		request.req_data = (int)filename[i];
		sendRequest(&ports[server_address],request);
		response = recieveResponse(&ports[request.address]);
		//printf("Response from the server %d\n",response.status);
	}
	
	request.req_data = -1;
	sendRequest(&ports[server_address],request);
	response = recieveResponse(&ports[request.address]);	
	//printf("Response from the server %d %d\n",response.status,response.errorcode);

	if (response.status==ACK)  // if there are no error file transfer starts
	{
		request.opcode = SEND;
		request.req_type = FILE_DATA; // NOW clients send file data
		response.status = 5;
		FILE *fileptr = fopen(filename,"r");
		if (fileptr==NULL)
			printf(" Unable to open file");
		else
		{

			while(response.status!=ACK)
    			{
				char ch = getc(fileptr);
				if (ch==EOF) // when EOF file reached alert server that no more message is coming and free the port being used
					request.req_data = -1;
				else
					request.req_data = (int)ch;
				sendRequest(&ports[server_address],request);
				response = recieveResponse(&ports[request.address]);
				if(response.status==ERROR){
					break;
				}
    			} 
			if(response.status==ACK)
				printf("file %s upload completed successfully\n",filename);
			if(response.errorcode==FILE_TOTAL_SIZE_ERROR) 
				printf("file %s upload failed as file size is greater that 1 MB\n",filename);

			while(1){
			yield();
			}
			
		}


	}
	else
	{
		if (response.errorcode==FILE_NAME_SIZE_ERROR)
		printf("file %s cannot be uploaded as it has name of size greater that 15 characters\n",filename);
	}
	
	
}





int main(int argc , char *argv[])
{
	
	init_readyQ(); // Initialize the readyQ to store thread control block(tcb) for each threads
		
	for(int i=0 ; i<argc-1 ;i++)
		strcpy(clientfilenames[i],argv[i+1]);
	
		
	for(int i=0 ; i<argc-1 ;i++)
		printf("Client filenames to be uploaded : %s\n",clientfilenames[i]);
	

	for(int i=0 ;i<argc ;i++)
		initialize_port(i); 
	
	
	start_thread(server,0,0); // 0 is the address assigned to server port
	
	for(int i=1;i<argc ;i++)
		start_thread(clients,i,0); // i is the address assigned to client port and uses "0" as server port address 
	
	run();
	
	return 0;
}
	
	
	


