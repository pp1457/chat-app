#include "enet_server.hpp"

void ENetServer::setupConnection() {
    if (enet_initialize()) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        exit(1);
    }
    atexit(enet_deinitialize);

    printf("Please enter the port you want to set up the server on: ");
    int tmp_port;
    scanf("%d", &tmp_port);
    address.host = ENET_HOST_ANY;
    address.port = tmp_port;

    server = enet_host_create(&address, 32, 1, 0, 0);

    if (!server) {
        fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
        exit(1);
    }
}


void ENetServer::sendPacket(ENetPeer *peer, std::string data) {
    ENetPacket *packet = enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void ENetServer::broadcastPacket(std::string data) {
    ENetPacket *packet = enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(server, 0, packet);
}

void ENetServer::run() {
    while (true) {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    printf("A new client connected from %x:%u.\n", 
                            event.peer->address.host,
                            event.peer->address.port);

                    handleConnection();
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    printf ("A packet of length %u was received from %x:%u on channel %u.\n",
                        (unsigned int)event.packet -> dataLength,
                        event.peer -> address.host,
                        event.peer -> address.port,
                        event.channelID);
                    printf("Content: %s\n", event.packet -> data);
                    handleReceipt();
                    enet_packet_destroy(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    printf("%x:%u disconnected.\n",
                            event.peer->address.host,
                            event.peer->address.port);
                    handleDisconnection();
                    event.peer->data = NULL;
                    break;
                }
                default:
                    break;
            }

        }

    }
}
