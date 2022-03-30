//
// Created by serene on 29.01.2022.
//

#include "client_display.h"

WINDOW *wmap;
WINDOW *winfo;
WINDOW *wlegend;


void update_header(pid_t server_pid, int round) {
    mvwprintw(winfo, 1, 1, "Server's PID: %d", server_pid);
    mvwprintw(winfo, 3, 1, " Round number: %d", round);
}

void update_info(player_t player) {
    int y = 7, x = 18;

    mvwprintw(winfo, ++y, x, "%d", player.num);
    mvwprintw(winfo, ++y, x, "%s", player.type == 1
                                          ? "Human"
                                          : "CPU");
    mvwprintw(winfo, ++y, x, "%02d/%02d", player.x, player.y);
    mvwprintw(winfo, ++y, x, "%d", player.deaths);
    y++;
    mvwprintw(winfo, ++y, 1, " Coins found %d",player.ccarried);
    mvwprintw(winfo, ++y, 1, " Coins brought %d",player.cbrought);
}

void update_map(wchar_t player_map[VISIBILITY][VISIBILITY]) {
    for (int y = 0; y < VISIBILITY; y++)
        for (int x = 0; x < VISIBILITY; x++) {
            if (player_map[y][x] == WALL)
                mvwaddch(wmap, y + 2, x + 2, WALL);
            else
                mvwaddch(wmap, y + 2, x + 2, player_map[y][x]);
        }
}

void init_info() {
    winfo = newwin(INFO_HEIGHT, INFO_WIDTH, 0, VISIBILITY * 2);

    int y = 0;
    const int cols[] = { 1, 18 };

    mvwprintw(winfo, ++y, cols[0], "Server's PID: ");
    mvwprintw(winfo, ++y, cols[0], " Campsite X/Y: unknown");
    mvwprintw(winfo, ++y, cols[0], " Round number: %d", 0);

    y += 3;

    mvwprintw(winfo, ++y, cols[0], "Player:");
    mvwprintw(winfo, ++y, cols[0], " Number:");
    mvwprintw(winfo, ++y, cols[0], " Type:");
    mvwprintw(winfo, ++y, cols[0], " Curr X/Y");
    mvwprintw(winfo, ++y, cols[0], " Deaths");
    y++;
    mvwprintw(winfo, ++y, cols[0], " Coins found");
    mvwprintw(winfo, ++y, cols[0], " Coins brought");
}

void display_legend() {
    wlegend = newwin(LEGEND_HEIGHT, INFO_WIDTH, INFO_HEIGHT, VISIBILITY * 2);

    int y = 0, x = 1;

    mvwprintw(wlegend, ++y, x, "Legend:");
    mvwprintw(wlegend, ++y, x, " 1234 - players");
    mvwaddch(wlegend, ++y, x + 1, WALL);
    mvwprintw(wlegend, y, x + 2, "    - wall");
    mvwprintw(wlegend, ++y, x, " #    - bush");
    mvwprintw(wlegend, ++y, x, " *    - wild beast");
    mvwprintw(wlegend, ++y, x, " %c   - one coin", COIN);
    mvwprintw(wlegend, ++y, x, " %c   - treasure (10 coins)", TREASURE);
    mvwprintw(wlegend, ++y, x, " %c   - large treasure (50 coins)", LARGE_TREASURE);
    mvwprintw(wlegend, ++y, x, " A    - campsite");
    mvwprintw(wlegend, ++y, x, " D    - dropped treasure");
}

void init_display() {
    initscr();
    cbreak();// TODO: don't know why
    noecho();
    curs_set(0);

    wmap = newwin(VISIBILITY * 2, VISIBILITY * 2, 0, 0);
    init_info();
    display_legend();
    refresh_all();
}

void refresh_all() {
    refresh();
    wrefresh(wmap);
    wrefresh(winfo);
}

void destroy_display() {
    delwin(wmap);
    delwin(winfo);
    delwin(wlegend);
    endwin();
}
