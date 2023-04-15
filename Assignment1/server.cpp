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
#include <time.h>        
#include <string.h> //convert int to string
#include <arpa/inet.h> //makes available the in_addr
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

int main(int argc, char const *argv[])
{
	//Declaring socket; handshake phase
	int server = 0;
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)	
	{
		cout<<"Error! Couldn't create socket.\n";
		return -1;
	}

	//Server receiving data
	struct sockaddr_in serveraddress;
	memset((char*)&serveraddress, 0, sizeof(serveraddress));	
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(atoi(argv[1]));
	serveraddress.sin_addr.s_addr = htons(INADDR_ANY);
	//Bind to socket
	if ((bind(server, (struct sockaddr*)&serveraddress, sizeof(serveraddress))) <0)
	{
		cout<<"Error! Could not bind to address\n";
		return -1;
	}

	if((listen(server, 6)) < 0)
	{
		cout<<"Error! Can't listen to port: ";
		cout<<argv[1];
		cout<<"\n";
		return -1;
	}

	struct sockaddr_in clientaddress;
	socklen_t clen = sizeof(clientaddress);
	int sock1 = ( accept(server, (struct sockaddr*)&clientaddress, &clen));

	char package[256];	
	memset(package,0,256);
	if((read(sock1,package,5)) < 0)
	{
		cout<<"unable to get data";
		return -1;
	}

	//Random number generator for socket
	srand((unsigned)time(0)); 
    int randNo = (rand()%65536)+1024;

	char r_port[256];
	memset(r_port,0,256);

	//Converting to int
	sprintf(r_port,"%d",randNo);
    cout<< "Random port number: "<< r_port <<endl;

	 //Sharing with client
    if ((send(sock1, r_port, 256, 0)) < 0)
	{
		cout<<"Error! Couldn't transfer to client";
		return -1;
	}
	
	close(server);//Closing server
	close(sock1); //Closing socket

	//Transfer phase
	//Declaring socket
	int sock = 0;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		cout << "Error! Couldn't create socket" << endl;
		return -1;
	}
	//Transfer port
	serveraddress.sin_port = htons(atoi(r_port)); 

	if ((bind(sock, (struct sockaddr*)&serveraddress, sizeof(serveraddress))) <0)
	{
		cout << "Error! Couldn't bind to socket" << endl;
		return -1;
	}
	//Receiving files four by four
	char payload[4];
	for(int i=0;i<14;i++)
	{
		if((recvfrom(sock,payload, 4, 0, (struct sockaddr*)&clientaddress, &clen))<0)
		{
			return -1;
		}		
			char data [4];
		//Output file declaration
		FILE *output;
		strcpy(data, payload);
		output = fopen("upload.txt", "f");
			fwrite(data, 4, sizeof(data), output);
		for (int i=0; i< 10; i++)
		{
			payload[i] = toupper(data[i]);
		}
		if((sendto(sock, payload, 4,0, (struct sockaddr*)&clientaddress, clen))<0)
		{
			cout<<"Aknowledge not sent";
			return -1;
		}
		fclose(output); //Closing file
	}
close(sock); //Closing socket
}