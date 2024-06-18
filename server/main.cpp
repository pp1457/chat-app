#include <iostream>
#include <map>
#include <enet/enet.h>

using std::cin;
using std::cout;
using std::endl;

void sendPacket(ENetPeer *peer, std::string data) {
    ENetPacket *packet = enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void broadcastPacket(ENetHost* server, std::string data) {
    ENetPacket *packet = enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(server, 0, packet);
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

void parseData(ENetHost *server, int id, char *data) {
	int data_type;

	sscanf(data,"%d|", &data_type);

	switch(data_type) {
		case 1: {
            char msg[100];
            sscanf(data, "1|%[^\n]", &msg);

            std::string data = "1|" + std::to_string(id) + "|" + msg;
            broadcastPacket(server, data);
			break;
        }
		case 2: {
            char username[80];
            sscanf(data, "2|%[^\n]", &username);

            std::string data = "2|" + std::to_string(id) + "|" + username;

            broadcastPacket(server, data);
            client_map[id]->setUsername(username);
			break;
        }
	}
}

static int player_cnt = 0;

int main(int argc, char ** argv) {
    if (enet_initialize()) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    ENetEvent event; 
    ENetPeer* peer;
    address.host = ENET_HOST_ANY;
    address.port = 7777;

    ENetHost* server;
    server = enet_host_create(&address, 32, 1, 0, 0);

    if (!server) {
        fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
        return EXIT_FAILURE;
    }
    
    while (true) {

        while (enet_host_service(server, &event, 1000) > 0) {

            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    printf("A new client connected from %x:%u.\n", 
                            event.peer->address.host,
                            event.peer->address.port);
                    for (auto const &x: client_map) {
                        char tmp_data[200] = {'\0'};
                        std::string data = "2|" + std::to_string(x.first) + "|" + x.second->getUsername();
                        broadcastPacket(server, data);
                    }

                    player_cnt++;
                    client_map[player_cnt] = new ClientData(player_cnt);
                    event.peer->data = client_map[player_cnt];

                    std::string data = "3|" + std::to_string(player_cnt);
                    sendPacket(event.peer, data);
                    
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    printf ("A packet of length %u was received from %x:%u on channel %u.\n",
                        (unsigned int)event.packet -> dataLength,
                        event.peer -> address.host,
                        event.peer -> address.port,
                        event.channelID);
                    printf("Content: %s\n", event.packet -> data);

                    parseData(server, static_cast<ClientData*>(event.peer->data)->getID(), (char *)event.packet->data);
                    enet_packet_destroy(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    printf("%x:%u disconnected.\n",
                            event.peer->address.host,
                            event.peer->address.port);
                    std::string data = "4|" + std::to_string(static_cast<ClientData*>(event.peer->data)->getID());
                    broadcastPacket(server, data);
                    event.peer->data = NULL;
                    break;
                }
                default:
                    break;
            }

        }

    }

    enet_host_destroy(server);

    return EXIT_SUCCESS;

}
