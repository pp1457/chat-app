#include "chat_client.hpp"

using std::cin;
using std::cout;
using std::endl;

void ChatClient::parseData(char *data) {
    int data_type, id;

    sscanf(data,"%d|%d", &data_type, &id);

    switch(data_type) {
        case 1: 
            if(id != client_id) {
                char msg[100];
                sscanf(data, "1|%*d|%[^\n]", &msg);
                chatScreen.postMessage(client_map[id]->getUsername().c_str(), msg);
            }
            break;
        case 2: 
            if(id != client_id) {
                char username[80];
                sscanf(data, "2|%*d|%[^\n]", &username);

                client_map[id] = new ClientData(id);
                client_map[id]->setUsername(username);
            }
            break;
        case 3:
            client_id = id;
            break;
    }
}

void ChatClient::init() {
    setupConnection();

    cout << "Please Enter Your Username: ";
    std::string username; cin >> username;
    client_username = username;

    std::string inform_server = "2|" + client_username;
    sendPacket(inform_server);

    chatScreen.init();
}

void ChatClient::run() {
    stopENetConnection = false;
    ts = std::thread(&ENetClient::keepENetConnection, this);

    stopMainLoop = false;
    while (!stopMainLoop) {
        std::string msg = chatScreen.checkBoxInput();

        if (msg == "!EXIT") {
            stopMainLoop = true;
            continue;
        }

        chatScreen.postMessage(client_username.c_str(), msg.c_str()); 

        std::string msg_data = "1|" + msg;
        sendPacket(msg_data);
    }
}

void ChatClient::end() {
    stopENetConnection = true;
    ts.join();
    chatScreen.end();
    disconnect();
}
