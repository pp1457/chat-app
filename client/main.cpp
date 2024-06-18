#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <thread>
#include <atomic>

#include "chat_screen.hpp"
#include <enet/enet.h>

static ChatScreen chatScreen;
static int CLIENT_ID;

using std::cin;
using std::cout;
using std::endl;

void sendPacket(ENetPeer *peer, std::string data) {
    ENetPacket *packet = enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

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

ENetHost* client;
std::atomic<bool> stopHandleEnet(false);

void handleEnet() {
    while (!stopHandleEnet) {
        ENetEvent event;
        while (enet_host_service(client, &event, 0) > 0) {
            switch(event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    parseData((char *)event.packet->data);
                    enet_packet_destroy(event.packet);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}


int main(int argc, char ** argv) {

    if (enet_initialize()) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetAddress address; // IP and port of the server
    ENetEvent event; 
    ENetPeer* peer;

    client = enet_host_create(NULL, 1, 1, 0, 0);

    if (!client) {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        return EXIT_FAILURE;
    }

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 7777;

    peer = enet_host_connect(client, &address, 1, 0);
    if (!peer) {
        fprintf(stderr, "No available peers for initiating an ENet connection!\n");
        return EXIT_FAILURE;
    }

    if (enet_host_service(client, &event, 5000) > 0 && 
            event.type == ENET_EVENT_TYPE_CONNECT) {
        puts("Connection to 127.0.0.1:7777 succeeded.");

    } else {
        enet_peer_reset(peer);
        puts("Connection to 127.0.0.1:7777 failed.");
        return EXIT_SUCCESS;
    }

    cout << "Please Enter Your Username." << endl;
    std::string username;
    cin >> username;

    std::string data = "2|" + username;
    sendPacket(peer, data);

    std::thread t1(handleEnet);

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
        sendPacket(peer, tmp_data);
    }

    chatScreen.end();

    stopHandleEnet = true;

    t1.join();

    // GAME LOOP END

    enet_peer_disconnect(peer, 0);

    while (enet_host_service(client, &event, 10) > 0) {
        switch(event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                puts("Disconnection succeeded.");
                break;
            default:
                break;
        }
    }

    return EXIT_SUCCESS;

}

