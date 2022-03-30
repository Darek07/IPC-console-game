//
// Created by serene on 29.01.2022.
//

#ifndef SERVER_CLIENT_COMMON_H
#define SERVER_CLIENT_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>


// consts
#define VISIBILITY 5

#define INFO_WIDTH 61
#define INFO_HEIGHT 17

#define LEGEND_HEIGHT 10

#define WALL (' '|A_REVERSE)
#define BUSH '#'
#define BEAST '*'
#define CAMPSITE 'A'

#define COIN 'c'
#define TREASURE 't'
#define LARGE_TREASURE 'T'
#define DROPPED_T 'D'

#define FIFO_DIR(F) ("/tmp/"F)
#define CONNECT_R FIFO_DIR("connect_wr")
#define CONNECT_WR FIFO_DIR("connect_r")
#define PL1_F_R FIFO_DIR("player1_fifo_to_client")
#define PL2_F_R FIFO_DIR("player2_fifo_to_client")
#define PL3_F_R FIFO_DIR("player3_fifo_to_client")
#define PL4_F_R FIFO_DIR("player4_fifo_to_client")

#define PL1_F_WR FIFO_DIR("player1_fifo_to_server")
#define PL2_F_WR FIFO_DIR("player2_fifo_to_server")
#define PL3_F_WR FIFO_DIR("player3_fifo_to_server")
#define PL4_F_WR FIFO_DIR("player4_fifo_to_server")

enum move {UP, DOWN, LEFT, RIGHT};

typedef struct {
    pid_t server_pid;
    int server_round;
    int active_users;
} server_t;

typedef struct {
    int init_x, init_y;
    int x, y;
    int num;
    int PID; // type: pid_t?
    int type; // 1 - Human, 2 - CPU
    int deaths;
    int ccarried; // coins carried
    int cbrought; // coins brought
    int slowed;
} player_t;

typedef struct {
    server_t server_info;
    player_t player;
    wchar_t player_map[VISIBILITY][VISIBILITY];
} client_sharing_t;

#endif //SERVER_CLIENT_COMMON_H
