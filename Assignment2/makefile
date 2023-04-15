all: client server

client: client.o
	g++ client.cpp packet.cpp -o client
	
server: server.o
	g++ server.cpp packet.cpp -o server	
	
clean:
	\rm client server output.txt arrival.log clientack.log clientseqnum.log