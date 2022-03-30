//
// Created by serene on 29.01.2022.
//

#ifndef SERVER_SERVER_OBJECTS_H
#define SERVER_SERVER_OBJECTS_H

#include "server_common.h"

void generate_bushes();
void init_player(player_t *player);
void get_player_map(client_sharing_t *client);
void move_player(player_t *player, enum move dir);
void death_player(player_t *player);
void players_collision(player_t players[MAX_PLAYERS]);
void init_objects_map(WINDOW* w, int wi, int h);
void beast_move(beast_t *beast, player_t players[MAX_PLAYERS]);
void generate_beast();
void generate_coin(int inp_type);
beast_t* get_beasts();
coin_t* get_coins();

#endif //SERVER_SERVER_OBJECTS_H
