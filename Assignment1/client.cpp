//Azim Bazarov
//ab4908
// Data Commiunication Networks
/*
Sources and concepts used in this coding assignment:

https://cplusplus.com/reference/clibrary/
https://man7.org/linux/man-pages/man7/ip.7.html
https://pubs.opengroup.org/
Canvas power point slides discussed in class

*/

#include <iostream>
#include <fstream> //Handling files: open/close; write on files
#include <sys/types.h>   // defines size_t, which is used for sizes of object
#include <sys/socket.h> //header defines sockaddr
#include <netinet/in.h> //defines numbers for sockets
#include <arpa/inet.h> // makes available the in_addr
#include <netdb.h> 
#include <time.h>
#include <string.h> //convert int to string
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;
  
//Start of main function
int main(int argc, char const *argv[])
{   
	//Handshake phase
	struct hostent *s;

	//Getting hostname by c.line argument
	s = gethostbyname(argv[1]); 

	//Handshake phase socket. Setting destination information
	struct sockaddr_in server;
	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2])); //Command line argument to int conversion
	bcopy((char*)s->h_addr, 
			(char*)&server.sin_addr.s_addr, 
			s->h_length);
	socklen_t slen = sizeof(server);//Send data to server
	
	//Declaring Socket. And creating condition
	int sock = 0; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cout<<"Error! Couldn't create socket.";
		return -1;
	}

 bind(sock, (struct sockaddr *)&server, sizeof(server));

	//Connecting to socket
	if ((connect(sock, (struct sockaddr*)&server, slen)) == -1)
	{
		return -1;
	}


//Start transfer
	char payload[256]="1357";
	memset(payload, 0, 256);
	sprintf(payload, "%d", 259);

	if ((send(sock, payload, 256, 0))<0)
	{
		cout<<"Error! Couldn't send data.";
		return -1;
	}

//Generate random port number
	char randPort[256];
	memset(randPort, 0, 256);
	int sock1 = 0;
	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	int bytesRead = read(sock, randPort, 5);
	close(sock);
	
	//Transfer phase
	//Declaring required data items
	FILE *data;
	char *buf;
	long dataLength;
	size_t result;

	const char *myFile = argv[3];
	int sock2 = 0;
	if ((sock2 = socket(AF_INET, SOCK_DGRAM, 0)) <0)
	{
		cout<<"Error! Couldn't create data socket";
		return -1;
	}
	//Change port
	server.sin_port = htons(atoi(randPort));
	data = fopen(myFile, "r"); //Open file
	//End of file
	fseek(data, 0, SEEK_END);
	long lSize = ftell(data);
	rewind(data);

	for (int i = 0; i<14; i++)
	{
		buf = (char*)malloc(4);
		memset(buf, 0, 5);
		
		//Reading file
		dataLength = fread(buf, 1, 4, data);

		if ((sendto(sock2, buf, 4, 0, (struct sockaddr*)&server, slen))<0)
		{
			cout<<"Could not send data";
			return -1;
		}

		char acknowledge[4];
		memset(acknowledge, 0, 8);
		if ((recvfrom(sock2, acknowledge, 4, 0, (struct sockaddr*)&server, &slen))<0)
		{
			cout<<"Error! Couldn't aknowledge.";
			return -1;
		}
		cout << acknowledge << endl;
	}	
	fclose(data); //Closing file
	close(sock2); //Closing socket
}