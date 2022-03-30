//
// Created by serene on 29.01.2022.
//

#include "server_objects.h"

// beasts, players, coins
coin_t coins[MAX_COINS] = {0};
beast_t beasts[MAX_BEASTS] = {0};

WINDOW *wmap = NULL;
int width = -1;
int height = -1;


beast_t* get_beasts() {
    return beasts;
}

coin_t* get_coins() {
    return coins;
}

void init_player(player_t *player) {
    do {
        player->init_x = rand() % width;
        player->init_y = rand() % height;
    } while (mvwinch(wmap, player->init_y + OFFSET, player->init_x + OFFSET) != ' ');
    player->x = player->init_x;
    player->y = player->init_y;
    player->type = 1;
    player->deaths = 0;
    player->ccarried = 0;
    player->cbrought = 0;
    player->slowed = 0;
}


void get_player_map(client_sharing_t *client) {
    int x = client->player.x;
    int y = client->player.y;
    wchar_t c;
    for (int i = 0; i < VISIBILITY; i++) {
        for (int j = 0; j < VISIBILITY; j++) {
            c = mvwinch(wmap, y - 2 + i + OFFSET, x - 2 + j + OFFSET);
            client->player_map[i][j] = (c == (client->player.num + '0') ? ' ' : c);
        }
    }
    client->player_map[2][2] = client->player.num + '0';
}

void death_player (player_t *player) {
    // drop treasure
    if (player->ccarried > 0) {
        int i = 0;
        if (mvwinch(wmap, player->y, player->y) == 'D') {
            while (i < MAX_COINS && coins[i].x != player->x && coins[i].y != player->y) i++;
            coins[i].value + player->ccarried;
            return;
        }else {
            while (i < MAX_COINS - 1 && coins[i].is_on_map) i++;

            coins[i].x = player->x;
            coins[i].y = player->y;
            coins[i].type = DROPPED;
            coins[i].value = player->ccarried;
            coins[i].is_on_map = 1;
        }
    }

    player->deaths++;
    player->x = player->init_x;
    player->y = player->init_y;
    player->ccarried = 0;
}


void move_player(player_t *player, enum move dir) {
    if (player->slowed) {
        player->slowed--;
        return;
    }

    int tmp_x, tmp_y;
    switch (dir) {
        case UP:
            tmp_x = player->x;
            tmp_y = player->y - 1;
            break;
        case RIGHT:
            tmp_x = player->x + 1;
            tmp_y = player->y;
            break;
        case DOWN:
            tmp_x = player->x;
            tmp_y = player->y + 1;
            break;
        case LEFT:
            tmp_x = player->x - 1;
            tmp_y = player->y;
            break;
        default:
            tmp_x = player->x;
            tmp_y = player->y;
            break;
    }
    wchar_t c = mvwinch(wmap, tmp_y + OFFSET, tmp_x + OFFSET);
    if (c == WALL) return;
    if (c == COIN || c == TREASURE || c == LARGE_TREASURE || c == DROPPED) {
        for (int i = 0; i < MAX_COINS; i++) {
            if (coins[i].x == tmp_x && coins[i].y == tmp_y) {
                coins[i].is_on_map = 0;
                player->ccarried += coins[i].value;
            }
        }
    } else if (c == BEAST) {
        death_player(player);
        return;
    } else if (c == CAMPSITE) {
        player->cbrought += player->ccarried;
        player->ccarried = 0;
    } else if (c == BUSH) {
        player->slowed = 2;
    }
    player->x = tmp_x;
    player->y = tmp_y;
}

void players_collision(player_t players[MAX_PLAYERS]) {
    for (int j = 0; j < MAX_PLAYERS - 1; j++) {
        if (players[j].x == players[j+1].x && players[j].y == players[j+1].y) {
            death_player(&players[j]);
            death_player(&players[j+1]);
        }
    }
}

void init_objects_map(WINDOW* w, int wi, int h) {
    wmap = w;
    width = wi;
    height = h;
}

void beast_move(beast_t *beast, player_t players[MAX_PLAYERS]) {
    if (!wmap || height == -1 || width == -1) return;
    double min = 5.0;
    int p = -1, tmp_x, tmp_y, dest;
    enum move dir;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].type) {
            double dist = sqrt(pow(players[i].x - beast->x, 2)
                               + pow(players[i].y - beast->y, 2));
            if (dist <= 3.0 && dist < min) {

                if (beast->x == players[i].x) {
                    dest = (beast->y - players[i].y) / 2;
                    if (dest && mvwinch(wmap, beast->y - dest + OFFSET, beast->x + OFFSET) == WALL)
                        continue;

                } else if (beast->y == players[i].y) {
                    dest = (beast->x - players[i].x) / 2;
                    if (dest && mvwinch(wmap, beast->y + OFFSET, beast->x - dest + OFFSET) == WALL)
                        continue;

                }
                min = dist;
                p = i;
            }
        }
    }
    if (p != -1) {
        if (beast->x == players[p].x) {
            beast->y -= ( dest == 0 ? beast->y - players[p].y : dest );
        } else if (beast->y == players[p].y) {
            beast->x -= ( dest == 0 ? beast->x - players[p].x : dest );
        }
        if (beast->x == players[p].x && beast->y == players[p].y)
            death_player(&players[p]);
    } else {
        do {
            dir = rand() % 5;
            switch (dir) {
                case UP:
                    tmp_x = beast->x;
                    tmp_y = beast->y - 1;
                    break;
                case RIGHT:
                    tmp_x = beast->x + 1;
                    tmp_y = beast->y;
                    break;
                case DOWN:
                    tmp_x = beast->x;
                    tmp_y = beast->y + 1;
                    break;
                case LEFT:
                    tmp_x = beast->x - 1;
                    tmp_y = beast->y;
                    break;
                default:
                    return;
            }
        } while (mvwinch(wmap, tmp_y + OFFSET, tmp_x + OFFSET) == WALL);
        beast->x = tmp_x;
        beast->y = tmp_y;
    }
}

void generate_beast() {
    if (!wmap || height == -1 || width == -1) return;
    int i = 0;
    while (i < MAX_BEASTS && beasts[i].is_on_map) i++;
    if (i == MAX_BEASTS) return;
    do {
        beasts[i].x = rand() % width;
        beasts[i].y = rand() % height;
    } while (mvwinch(wmap, beasts[i].y + OFFSET, beasts[i].x + OFFSET) != ' ');
    beasts[i].is_on_map = 1;
}

void generate_coin(int inp_type) {
    if (!wmap || height == -1 || width == -1) return;
    int i = 0;
    while (i < MAX_COINS && coins[i].is_on_map) i++;
    if (i == MAX_COINS) return;
    do {
        coins[i].x = rand() % width;
        coins[i].y = rand() % height;
    } while (mvwinch(wmap, coins[i].y + OFFSET, coins[i].x + OFFSET) != ' ');
    coins[i].is_on_map = 1;
    int type = inp_type == 0 ? rand() % 50 : inp_type;
    if (type < 25 || inp_type == COIN) {
        coins[i].type = COIN;
        coins[i].value = COIN_VALUE;
    }
    else if (type < 45 || inp_type == TREASURE) {
        coins[i].type = TREASURE;
        coins[i].value = TREASURE_VALUE;
    }
    else {
        coins[i].type = LARGE_TREASURE;
        coins[i].value = LARGE_T_VALUE;
    }
}

void generate_bushes() {
    int x, y;
    for (int i = 0; i < MAX_BUSHES; i++) {
        do {
            x = rand() % width;
            y = rand() % height;
        } while (mvwinch(wmap, y + OFFSET, x + OFFSET) != ' ');
        mvwaddch(wmap, y + OFFSET, x + OFFSET, BUSH);
    }
}