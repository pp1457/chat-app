all: chat_screen
	g++ -std=c++17 chat_screen.o main.cpp -o build/run -lenet -lncurses

chat_screen:
	g++ -std=c++17 chat_screen.cpp -c -o chat_screen.o
clean:
	rm build/*

