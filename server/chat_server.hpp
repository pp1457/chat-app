#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include <map>

#include "enet_server.hpp"

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

class ChatServer: public ENetServer {

private:
    int client_cnt;
    std::map<int, ClientData*> client_map;

    void handleConnection() override;
    void handleReceipt() override;
    void handleDisconnection() override;

public:
    ChatServer(): client_cnt(0) {}
};


#endif
