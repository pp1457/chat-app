#ifndef ENET_SERVER_HPP
#define ENET_SERVER_HPP

#include <iostream>
#include <string>
#include <cstring>

#include <enet/enet.h>

class ENetServer {

private:
    char m_ip[20];
    int m_port;

    ENetHost* server;
    ENetAddress address; 
    virtual void handleConnection() = 0;
    virtual void handleReceipt() = 0;
    virtual void handleDisconnection() = 0;
protected:
    ENetEvent event; 
    void sendPacket(ENetPeer *peer, std::string data);
    void broadcastPacket(std::string data);

public:
    void setupConnection();
    void run();
};
#endif
