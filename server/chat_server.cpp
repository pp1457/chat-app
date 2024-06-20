#include "chat_server.hpp"

void ChatServer::handleConnection() {
    for (auto const &x: client_map) {
        std::string data = "2|" + std::to_string(x.first) + "|" + x.second->getUsername();
        broadcastPacket(data);
    }

    client_cnt++;
    client_map[client_cnt] = new ClientData(client_cnt);
    event.peer->data = client_map[client_cnt];

    std::string data = "3|" + std::to_string(client_cnt);
    sendPacket(event.peer, data);
}

void ChatServer::handleReceipt() {
    int id = static_cast<ClientData*>(event.peer->data)->getID();
    char *data = (char*)(event.packet->data);

	int data_type;

	sscanf(data,"%d|", &data_type);

	switch(data_type) {
		case 1: {
            char msg[100];
            sscanf(data, "1|%[^\n]", msg);

            std::string data = "1|" + std::to_string(id) + "|" + msg;
            broadcastPacket(data);
			break;
        }
		case 2: {
            char username[80];
            sscanf(data, "2|%[^\n]", username);

            std::string data = "2|" + std::to_string(id) + "|" + username;

            broadcastPacket(data);
            client_map[id]->setUsername(username);
			break;
        }
	}
}

void ChatServer::handleDisconnection() {
    std::string data = "4|" + std::to_string(static_cast<ClientData*>(event.peer->data)->getID());
    broadcastPacket(data);
}
