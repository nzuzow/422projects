all: Tic_Tac_Toe.o lab1_server.o lab1_client.o
	g++ -g -o lab1_server lab1_server.o Tic_Tac_Toe.o 
	g++ -g -o lab1_client lab1_client.o Tic_Tac_Toe.o

Tic_Tac_Toe.o: Tic_Tac_Toe.cc Tic_Tac_Toe.h
	g++ -Wall -c Tic_Tac_Toe.cc

lab1_server.o: lab1_server.cc lab1_server.h packet.h Tic_Tac_Toe.h
	g++ -Wall -c lab1_server.cc 

lab1_client.o: lab1_client.cc lab1_client.h packet.h Tic_Tac_Toe.h 
	g++ -Wall -c lab1_client.cc 

clean:
	rm -rf lab1_server lab1_client *.o

