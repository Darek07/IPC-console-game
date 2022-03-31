//
// Created by serene on 29.01.2022.
//

#include "server_display.h"
#include "server_objects.h"

//! WARNING: CHANGE PATH HERE
#define MAZEFILE "./server/maze"

WINDOW *wmap;
WINDOW *winfo;
WINDOW *wlegend;

int m_height = 0;
int m_width = 0;


void update_map(player_t players[MAX_PLAYERS]) {
    // clear
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            wchar_t ch = mvwinch(wmap, y + OFFSET, x + OFFSET);
            if (ch != WALL && ch != BUSH)
                mvwaddch(wmap, y + OFFSET, x + OFFSET, ' ');
        }
    }

    beast_t *beasts = get_beasts();
    coin_t *coins = get_coins();

    for (int i = 0; i < MAX_COINS; i++) {
        if (coins[i].is_on_map) {
            mvwaddch(wmap, coins[i].y + OFFSET, coins[i].x + OFFSET, coins[i].type);
        }
    }

    mvwaddch(wmap, CAMPSITE_Y + OFFSET, CAMPSITE_X + OFFSET, CAMPSITE);

    for (int i = 0; i < MAX_PLAYERS; i++)
        if (players[i].type)
            mvwaddch(wmap, players[i].y + OFFSET, players[i].x + OFFSET, players[i].num + '0');

    for (int i = 0; i < MAX_BEASTS; i++)
        if (beasts[i].is_on_map)
            mvwaddch(wmap, beasts[i].y + OFFSET, beasts[i].x + OFFSET, BEAST);

}

void update_info(player_t players[MAX_PLAYERS], int round) {

    int y = 3;
    const int cols[] = { 1, 15, 25, 35, 45 };

    mvwprintw(winfo, y, cols[0], " Round number: %d", round);
    y += 4;

    for (int i = 1; i < 5; i++) {
        if (players[i-1].type) {
            mvwprintw(winfo, ++y, cols[i], "%d", players[i-1].PID);
            mvwprintw(winfo, ++y, cols[i], "%s", players[i-1].type == 1
                                                 ? "Human"
                                                 : "CPU");
            mvwprintw(winfo, ++y, cols[i], "%02d/%02d", players[i-1].x, players[i-1].y);
            mvwprintw(winfo, ++y, cols[i], "%d", players[i-1].deaths);
            y += 2;
            mvwprintw(winfo, ++y, cols[i], "%d", players[i-1].ccarried);
            mvwprintw(winfo, ++y, cols[i], "%d", players[i-1].cbrought);
        } else {
            mvwprintw(winfo, ++y, cols[i], "-     ");
            mvwprintw(winfo, ++y, cols[i], "-     ");
            mvwprintw(winfo, ++y, cols[i], "--/--");
            mvwprintw(winfo, ++y, cols[i], "-     ");
            y += 2;
            mvwprintw(winfo, ++y, cols[i], "      ");
            mvwprintw(winfo, ++y, cols[i], "      ");
        }
        y -= 8;
    }
}

void init_map() {
    // load map
    char *filename = MAZEFILE;
    FILE *fmaze = fopen(filename, "r");
    if (!fmaze) return;

    while (fgetc(fmaze) != '\n')
        m_width++;

    fseek(fmaze, 0, SEEK_END);
    m_height = (int)ftell(fmaze) / m_width;

    rewind(fmaze);

    wmap = newwin(m_height+2, m_width+2, 0, 0);

    // display map
    for(int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            char c = (char)fgetc(fmaze);
            mvwaddch(wmap, y + OFFSET, x + OFFSET, c == BUSH
                                         ? WALL
                                         : c);
        }
        fgetc(fmaze);
    }
    fclose(fmaze);
}

void init_info() {
    winfo = newwin(INFO_HEIGHT, INFO_WIDTH, OFFSET, m_width + OFFSET * 2);

    int y = 0;
    const int cols[] = { 1, 15, 25, 35, 45 };

    mvwprintw(winfo, ++y, cols[0], "Server's PID: %d", getpid());
    mvwprintw(winfo, ++y, cols[0], " Campsite X/Y: %d/%d", CAMPSITE_X, CAMPSITE_Y);
    mvwprintw(winfo, ++y, cols[0], " Round number: %d", 0);

    y += 3;

    mvwprintw(winfo, ++y, cols[0], "Parameter:");
    mvwprintw(winfo, y, cols[1], "Player 1");
    mvwprintw(winfo, y, cols[2], "Player 2");
    mvwprintw(winfo, y, cols[3], "Player 3");
    mvwprintw(winfo, y, cols[4], "Player 4");
    mvwprintw(winfo, ++y, cols[0], " PID");
    mvwprintw(winfo, ++y, cols[0], " Type");
    mvwprintw(winfo, ++y, cols[0], " Curr X/Y");
    mvwprintw(winfo, ++y, cols[0], " Deaths");
    y++;
    mvwprintw(winfo, ++y, cols[0], " Coins");
    mvwprintw(winfo, ++y, cols[0] + 4, " carried");
    mvwprintw(winfo, ++y, cols[0] + 4, " brought");
}

void display_legend() {
    wlegend = newwin(LEGEND_HEIGHT, INFO_WIDTH, INFO_HEIGHT + OFFSET, m_width + OFFSET * 2);

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
    cbreak();
    noecho();
    curs_set(0);

    init_map();
    init_info();
    display_legend();
    refresh_all();

    init_objects_map(wmap, m_width, m_height);
}

void refresh_all() {
    refresh();
    wrefresh(wmap);
    wrefresh(winfo);
    wrefresh(wlegend);
}

void destroy_display() {
    delwin(wmap);
    delwin(winfo);
    delwin(wlegend);
    endwin();
}
