#ifndef ENET_CLIENT_HPP
#define ENET_CLIENT_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <atomic>

#include <enet/enet.h>

class ENetClient {

private:
    char server_ip[20];
    int server_port;

    ENetHost* client;
    ENetAddress address; 
    ENetEvent event; 
    ENetPeer* server;
    void connectToServer(char * ip, int port);
    virtual void parseData(char *data) = 0;

protected:
    std::atomic<bool> stopENetConnection;


public:
    void setupConnection();
    void sendPacket(std::string data);
    void keepENetConnection();
    void disconnect();

};
#endif
