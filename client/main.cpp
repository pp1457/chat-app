#include <iostream>
#include <string>
#include <cstring>

#include "chat_screen.hpp"
#include <enet/enet.h>

static ChatScreen chatScreen;

using std::cin;
using std::cout;
using std::endl;

void sendPacket(ENetPeer *peer, const char * data) {
    ENetPacket *packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
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

    ENetHost* client;
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



    // GAME LOOP START

    chatScreen.init();

    bool running = true;

    while (running) {
        std::string msg = chatScreen.checkBoxInput();

        if (msg == "!EXIT") {
            running = false;
        }

        chatScreen.postMessage(username.c_str(), msg.c_str()); 
        msg = username + ": " + msg;

        sendPacket(peer, msg.c_str());
        enet_host_service(client, &event, 10);
        switch(event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                printf ("A packet of length %u containing %s was received from %x:%u on channel %u.\n",
                (unsigned int)event.packet -> dataLength,
                event.packet -> data,
                event.peer -> address.host,
                event.peer -> address.port,
                event.channelID);
                break;
            case ENET_EVENT_TYPE_CONNECT:
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                break;
        }
    }
    chatScreen.end();

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
            case ENET_EVENT_TYPE_CONNECT:
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }

    return EXIT_SUCCESS;

}

