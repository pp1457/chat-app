#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include <map>
#include <thread>
#include <vector>

#include "enet_client.hpp"
#include "chat_screen.hpp"

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

class ChatClient: public ENetClient {

private:
    ChatScreen chatScreen;
    int client_id;
    std::string client_username;

    std::map<int, ClientData*> client_map;

    std::thread ts;

    bool stopMainLoop;

    void parseData(char *data) override;

public:
    void init();
    void run();
    void end();
};

#endif
