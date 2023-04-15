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
int main(int argc, char* argv[])
{
	int type = 0;	  			//declare packet type start
	int length = 30;			//declare packet length
	int seqnum = -1;  			//declare packet seqnum
	int nextSeqNum = 1; 		//declare next seqnum
	int base = 0;				//declare base 
	int N = 7;					//declare window size
	char data[8][31]; 			//declare char array to store data

	char dataPacket[42];
	memset(dataPacket, '\0', sizeof(dataPacket));

	bool EOFile = false;

	int outstandingPacket = 0;	   
	int receivedData[42];

	//timer
	struct timeval timer;
	timer.tv_sec = 2;
	timer.tv_usec = 0;



	//getting right number of inputs
	if (argc != 5)
	{
		cout << "Error! Incorrect number of args!\n";
		return 1;
	}

	//getting host; then converting to ip
	// declare and setup server
	struct hostent* em_host;            
	em_host = gethostbyname(argv[1]);   // declaring host name for emulator
	if (em_host == NULL) {                // displaying error message 
		cout << "Error! Failed to obtain server.\n";
		exit(EXIT_FAILURE);
	}


	//converting port number to int
	int portNo = atoi(argv[2]);

	//check port number range
	if (portNo < 1024 || portNo > 65355) {
		cout << "Error! Port number is between 1024 and 65355!" << endl;
		exit(1);
	}

	// client sets up datagram socket for sending
	int CESocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (CESocket < 0) {
		cout << "Error: failed to open datagram socket.\n";
	}

	// set up the sockaddr_in structure for sending
	struct sockaddr_in CE;
	socklen_t CE_length = sizeof(CE);
	bzero(&CE, sizeof(CE));
	CE.sin_family = AF_INET;
	bcopy((char*)em_host->h_addr, (char*)&CE.sin_addr.s_addr, em_host->h_length);  
	char* end;
	int em_rec_port = strtol(argv[2], &end, 10);  // get emulator's receiving port and convert to int
	CE.sin_port = htons(em_rec_port);             // set to emulator's receiving port


	// client sets up datagram socket for receiving
	int ECSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ECSocket < 0) {
		cout << "Error: failed to open datagram socket.\n";
	}

	// set up the sockaddr_in structure for receiving
	struct sockaddr_in EC;
	socklen_t EC_length = sizeof(EC);
	bzero(&EC, sizeof(EC));
	EC.sin_family = AF_INET;
	EC.sin_addr.s_addr = htonl(INADDR_ANY);
	char* end2;
	int cl_rec_port = strtol(argv[3], &end2, 10);  // client's receiving port and convert to int
	EC.sin_port = htons(cl_rec_port);             // set to emulator's receiving port

	// do the binding
	if (bind(ECSocket, (struct sockaddr*)&EC, EC_length) == -1) {
		cout << "Error in binding.\n";
	}

	//file descriptors for select
	fd_set fileDescriptors;
	FD_ZERO(&fileDescriptors);
	FD_SET(ECSocket, &fileDescriptors);
	//reading from file
	FILE* file;
	file = fopen(argv[4], "r");
	if (file == NULL)
	{
		cout << "Error!Can't open the file\n";
		return 1;
	}
	//writing logs to file
	ofstream seqfile;
	ofstream ackfile;
	//opening files
	seqfile.open("clientseqnum.log");
	ackfile.open("clientack.log");

	//checking to see files opened 
	if (!seqfile.is_open())
	{
		cout << "Error! Can't open file\n";
		return 1;
	}
	if (!ackfile.is_open())
	{
		cout << "Error! Can't open file\n";
		return 1;
	}

	//set all elements to zero in the array
	for (int j = 0; j < 8; j++)
	{
		//memeset call for each element setting 0
		memset(data[j], '\0', sizeof(data[j]));
	}
	//creating packet
	packet pack = packet(0, 0, 0, NULL);

   //encapsulation
	while (1)
	{
		//if EOF and oustanding pack
		if (EOFile && outstandingPacket == 0)
		{
			seqnum++;
			seqnum = seqnum % 8;
			//type equals 3 
			pack = packet(3, seqnum, 0, NULL);
			pack.serialize(dataPacket);
			sendto(CESocket, dataPacket, sizeof(dataPacket), 0, (struct sockaddr*)&CE, sizeof(CE));
			//push seqnumber into file
			seqfile << pack.getSeqNum() << "\n";
		}

		while (outstandingPacket < 7 && !EOFile)
		{
			//setting data packet
			seqnum++;
			seqnum = seqnum % 8;
			type = 1;
			//setting datapacket == 0
			memset(&dataPacket, 0, sizeof(dataPacket));

			//read from file
			int readCount = fread(data[seqnum], 1, length, file);
			if (readCount == length)
			{

				//make the pack to send to emulator
				pack = packet(type, seqnum, length, data[seqnum]);
				pack.serialize(dataPacket);	//serializing a packet
				sendto(CESocket, dataPacket, sizeof(dataPacket), 0, (struct sockaddr*)&CE, sizeof(CE));

				//add the outstanding packet and set the next seq number; ie if 0 it will be 1 from the math below
				outstandingPacket = (outstandingPacket + 1) % 8;
				nextSeqNum = (seqnum + 1) % 8;
				//write sequence number to file with a new line added
				seqfile << pack.getSeqNum() << "\n";
			}
			else
			{
				//set data array == 0 for all elements
				data[seqnum][readCount] = '\0';

				//seqnum will increment before and allow us to send last packet
				pack = packet(type, seqnum, readCount, data[seqnum]);
				pack.serialize(dataPacket);
				sendto(CESocket, dataPacket, sizeof(dataPacket), 0, (struct sockaddr*)&CE, sizeof(CE));


				//writing to seqfile with newline
				seqfile << pack.getSeqNum() << "\n";

				//same as last if
				EOFile = true;
				nextSeqNum = (seqnum + 1) % 8;
				outstandingPacket = (outstandingPacket + 1) % 8;
				break;
			}
		}
		//de encapsulation and timer set up 
		int m = ECSocket;
		int setTimer = select(m + 1, &fileDescriptors, NULL, NULL, &timer);
		if (setTimer > 0)
		{
			int c = recvfrom(ECSocket, receivedData, sizeof(receivedData), 0, (struct sockaddr*)&EC, &EC_length);
			if (c < 0)
			{

				perror("recvfrom\n");
			}

			//packing data
			packet pack(0, 0, 0, NULL);
			pack.deserialize((char*)receivedData);

			//if EOT then ...
			if (pack.getType() == 2)
			{
				ackfile << pack.getSeqNum() << endl;
				break;
			}
			else
			{
				//getting seqnum of the packet
				int ackseq = pack.getSeqNum();
				//if else seqnum
				if (ackseq >= base || (ackseq < (base + N) % 8 && ackseq >= 0))
				{
					base = (pack.getSeqNum() + 1) % 8;

					if (nextSeqNum > pack.getSeqNum())
					{
						outstandingPacket = (nextSeqNum - base) % 8;
					}
					else
					{
						outstandingPacket = (N - (abs(nextSeqNum - ackseq))) % 8;
					}
				}
				//seqnum returning to ackfile
				ackfile << pack.getSeqNum() << endl;
			}
		}
		//error
		else if (setTimer < 0)
		{
			cout << "udp socket error has occured .. \n";
			return 1;
		}
		//timeout occured
		else
		{

			cout << "Resending packets due to timeout\n";


			//resend

			for (int i = (seqnum - 6) % 8; i < seqnum + 1; ++i)
			{
				//if there is data in the element create packet to send to emulator
				if (data[i] != NULL)
				{
					pack = packet(type, i, length, data[i]);
					pack.serialize(dataPacket);

					//send to emulator

					sendto(CESocket, dataPacket, sizeof(dataPacket), 0, (struct sockaddr*)&EC, sizeof(EC));
					seqfile << pack.getSeqNum() << endl;
				}
			}
		}
	}

	//closing the files and sockets
	seqfile.close();
	ackfile.close();
	close(ECSocket);
	close(CESocket);
}