#include "chat_client.hpp"

int main(int argc, char ** argv) {

    ChatClient chatClient;
    chatClient.init();
    chatClient.run();
    chatClient.end();

    return EXIT_SUCCESS;

}

