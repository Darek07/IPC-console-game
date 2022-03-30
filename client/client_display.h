//
// Created by serene on 29.01.2022.
//

#ifndef SERVER_CLIENT_DISPLAY_H
#define SERVER_CLIENT_DISPLAY_H

#include "client_common.h"

void init_display();
void refresh_all();
void destroy_display();
void update_header(pid_t server_pid, int round);
void update_info(player_t player);
void update_map(wchar_t player_map[VISIBILITY][VISIBILITY]);

#endif //SERVER_CLIENT_DISPLAY_H
