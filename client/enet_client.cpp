#include "enet_client.hpp"

void ENetClient::connectToServer(char * ip, int port) {
    enet_address_set_host(&address, ip);
    address.port = port;

    server = enet_host_connect(client, &address, 1, 0);
    if (!server) {
        fprintf(stderr, "No available server for initiating an ENet connection!\n");
        exit(1);
    }

    if (enet_host_service(client, &event, 1000) > 0 && 
            event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to server@%s:%d succeeded!\n", ip, port);

    } else {
        enet_peer_reset(server);
        printf("Connection to server@%s:%d failed...\n", ip, port);
        exit(1);
    }

}

void ENetClient::keepENetConnection() {
    while (!stopENetConnection) {
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

void ENetClient::setupConnection() {
    if (enet_initialize()) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return;
    }
    atexit(enet_deinitialize);

    printf("Welcome to chat app!\n");
    printf("Please enter the IP of the server you want to connect to: ");
    scanf("%s", server_ip);
    printf("Please also enter the port of the server: ");
    scanf("%d", &server_port);

    client = enet_host_create(NULL, 1, 1, 0, 0);

    connectToServer(server_ip, server_port);
    stopENetConnection = false;
}


void ENetClient::sendPacket(std::string data) {
    ENetPacket *packet = enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(server, 0, packet);
}


void ENetClient::disconnect() {
    stopENetConnection = true;

    enet_peer_disconnect(server, 0);
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
}

