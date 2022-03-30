//
// Created by serene on 29.01.2022.
//

#ifndef SERVER_SERVER_DISPLAY_H
#define SERVER_SERVER_DISPLAY_H

#include "server_common.h"

void init_display();
void update_map(player_t players[MAX_PLAYERS]);
void update_info(player_t players[MAX_PLAYERS], int round);
void refresh_all();
void destroy_display();

#endif //SERVER_SERVER_DISPLAY_H
