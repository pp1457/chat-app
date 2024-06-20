#include <iostream>

#include "chat_server.hpp"

using std::cin;
using std::cout;
using std::endl;

int main(int argc, char ** argv) {

    ChatServer chatServer;
    chatServer.setupConnection();
    chatServer.run();
    return EXIT_SUCCESS;
}
