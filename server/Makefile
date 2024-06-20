all: chat_server
	g++ -std=c++17 build/enet_server.o build/chat_server.o main.cpp -o build/run -lenet -lncurses 

chat_server: enet_server
	g++ -std=c++17 chat_server.cpp -c -o build/chat_server.o

enet_server:
	g++ -std=c++17 enet_server.cpp -c -o build/enet_server.o


clean:
	rm build/*
