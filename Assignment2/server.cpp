/*
Azim Bazarov [ab4908]
Thach Nguyen [tdn127]
We used several sources to write this program. We cited the the sources used down below and throughout the code
Dr.Young emulation-hookup example
Dr.Young provided packet.cpp and .h files
Some of the other code was used from presentation slides provided in Canvas
Additiional:
read from file reference https://www.cplusplus.com/reference/fstream/ofstream/
timeout reference https://stackoverflow.com/questions/13547721/udp-socket-set-timeout
memset source https://www.cplusplus.com/reference/cstring/memset/
*/

#include <iostream>
#include "packet.h"
#include <sys/types.h>
#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for sockets
#include <arpa/inet.h>   
#include <string.h>
#include <unistd.h>
#include <math.h>

using namespace std;

int main(int argc, char* argv[]) {
	int type = 1;         //declare packet type start
	int seqnum = 0;       //declare packet seqnum
	int acktype = 0;      //declare acknowledgement
	int expectseq = 0;    //declare expected sequence
	char receiveData[37]; // receive from packet
	char charData[30];    
	char ackData[42];     // sending ack data to client


	  //get the right number of inputs from the user or end program execution
	if (argc != 5)
	{
		cout << "Error! Incorrect number of args!\n";
		return 1;
	}

	// sets up datagram socket for receiving from emulator
	int ESSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ESSocket < 0) {
		cout << "Error: failed to open datagram socket.\n";
	}

	// set up the sockaddr_in structure for receiving
	struct sockaddr_in ES;
	socklen_t ES_length = sizeof(ES);
	bzero(&ES, sizeof(ES));
	ES.sin_family = AF_INET;
	ES.sin_addr.s_addr = htonl(INADDR_ANY);
	char* end;
	int sr_rec_port = strtol(argv[2], &end, 10);  // server receiving port converted to int
	ES.sin_port = htons(sr_rec_port);            

	// do the binding
	if (bind(ESSocket, (struct sockaddr*)&ES, ES_length) == -1)
		cout << "Error in binding.\n";

	// declare and setup server
	struct hostent* em_host;            
	em_host = gethostbyname(argv[1]);   

	if (em_host == NULL) {                
		cout << "Error! Failed to obtain server.\n";
		exit(EXIT_FAILURE);
	}
	int SESocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (SESocket < 0) {
		cout << "Error in trying to open datagram socket.\n";
		exit(EXIT_FAILURE);
	}
	// setup sockaddr_in struct  
	struct sockaddr_in SE;
	memset((char*)&SE, 0, sizeof(SE));
	SE.sin_family = AF_INET;
	bcopy((char*)em_host->h_addr, (char*)&SE.sin_addr.s_addr, em_host->h_length);
	int em_rec_port = strtol(argv[3], &end, 10);
	SE.sin_port = htons(em_rec_port);

//writing to files
	ofstream outfile;
	outfile.open(argv[4]); // open file
	if (!outfile.is_open())
	{
		cout << "Error! Can't open file\n";
		return 1;
	}

	// setting up ack file 
	ofstream ackFile;
	ackFile.open("arrival.log");

	if (!ackFile.is_open())
	{
		cout << "Could not open file to write to\n";
		return 1;
	}

	packet filepacket(type, seqnum, sizeof(charData), charData);

	//deencapsulation
	while (1)
	{
		memset(receiveData, 0, sizeof(receiveData));
		memset(charData, 0, sizeof(charData));
		memset(ackData, 0, sizeof(ackData));

		if (recvfrom(ESSocket, receiveData, sizeof(receiveData), 0, (struct sockaddr*)&ES, &ES_length) == -1)
		{
			perror("Receiving\n");
			return -1;
		}

		filepacket.deserialize((char*)receiveData); // desterialize to see data sent
		type = filepacket.getType();                 //get type
		seqnum = filepacket.getSeqNum();
		//write to ack file
		ackFile << seqnum << "\n";
		cout << endl;

		//  if the seq num matches
		if (expectseq == seqnum)
		{
			if (filepacket.getType() == 3)
			{
				// setting acktype to 2
				acktype = 2;
				packet ackpacket(acktype, seqnum, 0, 0);
				ackpacket.serialize(ackData);

				//send to client
				if (sendto(SESocket, ackData, sizeof(ackData), 0, (struct sockaddr*)&SE, sizeof(SE)) == -1)
				{
					perror("error sending");
					return 1;
				}
				cout << endl;

				break;
			}
			//write data into file
			outfile << filepacket.getData();


			packet ackpacket(acktype, seqnum, 0, 0);
			ackpacket.serialize((char*)ackData);

			//send ack to client
			if (sendto(SESocket, ackData, sizeof(ackData), 0, (struct sockaddr*)&SE, sizeof(SE)) == -1)
			{
				perror("error sending");
				return 1;
			}
			expectseq = (expectseq + 1) % 8;
		}
		else
		{
			packet ackpacket(acktype, expectseq, 0, 0);
			ackpacket.serialize((char*)ackData);
			sendto(SESocket, ackData, sizeof(ackData), 0, (struct sockaddr*)&SE, sizeof(SE));

		}
	}
	//close file and sockets
	outfile.close();
	close(SESocket);
}