#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <thread>
#include <atomic>

#include "chat_screen.hpp"
#include "enet_client.hpp"
#include <enet/enet.h>

static ChatScreen chatScreen;
static int CLIENT_ID;

using std::cin;
using std::cout;
using std::endl;


class ClientData {

private:
    int m_id;
    std::string m_username;

public:
    ClientData(int id): m_id(id) {}
    void setUsername(std::string username) { m_username = username; }
    int getID() { return m_id; }
    std::string getUsername() { return m_username; }

};

std::map<int, ClientData*> client_map;
ENetClient enetClient;

void parseData(char *data) {
	int data_type, id;

	sscanf(data,"%d|%d", &data_type, &id);

	switch(data_type) {
		case 1: 
            if(id != CLIENT_ID) {
				char msg[100];
				sscanf(data, "1|%*d|%[^\n]", &msg);
				chatScreen.postMessage(client_map[id]->getUsername().c_str(), msg);
			}
			break;
		case 2: 
            if(id != CLIENT_ID) {
				char username[80];
				sscanf(data, "2|%*d|%[^\n]", &username);

				client_map[id] = new ClientData(id);
				client_map[id]->setUsername(username);
		    }
			break;
		case 3:
			CLIENT_ID = id;
			break;
	}
}


int main(int argc, char ** argv) {

    enetClient.setupConnection();

    cout << "Please Enter Your Username." << endl;
    std::string username;
    cin >> username;

    std::string data = "2|" + username;
    enetClient.sendPacket(data);

    std::thread t1(enetClient.handleEnet);

    // GAME LOOP START

    chatScreen.init();

    bool running = true;

    while (running) {
        std::string msg = chatScreen.checkBoxInput();

        if (msg == "!EXIT") {
            running = false;
            break;
        }

        chatScreen.postMessage(username.c_str(), msg.c_str()); 

        std::string tmp_data = "1|" + msg;
        enetClient.sendPacket(tmp_data);
    }

    chatScreen.end();
    enetClient.disconnect();
    t1.join();

    // GAME LOOP END
    

    return EXIT_SUCCESS;

}

