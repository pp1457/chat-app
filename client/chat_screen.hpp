#ifndef CHAT_SCREEN_HPP
#define CHAT_SCREEN_HPP

#include <iostream>
#include <stdio.h>
#include <string>
#include <ncurses.h>

class ChatScreen {
public:
    ChatScreen();
    ~ChatScreen();

    void init();
    void end();
    void postMessage(const char username[80], const char msg[100]);

    std::string checkBoxInput();

private:
    int msg_y = 0;
    WINDOW * inputwin = nullptr;
};

#endif

