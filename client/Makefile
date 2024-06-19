all: chat_screen enet_client
	g++ -std=c++17 build/chat_screen.o build/enet_client.o main.cpp -o build/run -lenet -lncurses

chat_screen:
	g++ -std=c++17 chat_screen.cpp -c -o build/chat_screen.o
enet_client:
	g++ -std=c++17 enet_client.cpp -c -o build/enet_client.o
clean:
	rm build/*

