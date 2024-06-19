#include "chat_screen.hpp"

ChatScreen::ChatScreen(){}

ChatScreen::~ChatScreen(){}

void ChatScreen::end() {
    clear();
    delwin(inputwin);
    endwin();
}

void ChatScreen::init() {
    initscr();
    nodelay(stdscr, TRUE);

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    inputwin = newwin(3, xMax-12, yMax-5, 5);
    box(inputwin, 0, 0);
    refresh();
    wrefresh(inputwin);
}

std::string ChatScreen::checkBoxInput() {
    wclear(inputwin);
    wrefresh(inputwin);
    box(inputwin, 0, 0);
    wrefresh(inputwin);

    char msg[100];
    mvwscanw(inputwin, 1, 1, "%[^\n]", msg);

    return msg;
}

void ChatScreen::postMessage(const char username[80], const char msg[100]) {
    mvprintw(msg_y, 1, "%s: %s", username, msg);
    refresh();
    msg_y++;
}
