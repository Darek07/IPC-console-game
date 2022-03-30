

#include "server_common.h"
#include "server_display.h"
#include "server_objects.h"
#include <string.h>
#include <errno.h>

server_t server;
player_t players[MAX_PLAYERS] = {0};
client_sharing_t client_share[MAX_PLAYERS] = {0};

void* client_communication(void* inp);
void* client_sending(void* inp);
void* client_joining(void* inp);
void* rounds_counting(void* inp);
void* update_display(void* inp);
void* beast_movement(void* inp);
void terminate();

pthread_t rounds;
pthread_t display;
pthread_t beasts_pth[MAX_BEASTS];
pthread_t joining;
pthread_t communication[MAX_PLAYERS];
pthread_t sending[MAX_PLAYERS];


pthread_mutex_t m_client;


int main() {
    srand(time(0));

    pthread_mutex_init(&m_client, NULL);

    server.server_round = 0;
    server.server_pid = getpid();
    server.active_users = 0;

    if (mkfifo(CONNECT_R, 0666) == -1 && errno != EEXIST) {
        printf("%s\n", strerror(errno));
        return 1;
    }
    if (mkfifo(CONNECT_WR, 0666) == -1 && errno != EEXIST) {
        printf("%s\n", strerror(errno));
        return 1;
    }
    int fd;
    fd = open(CONNECT_R, O_RDWR);
    if (fd == -1) {
        printf("Couldn't open fifo file %s to read\n", CONNECT_R);
        return 1;
    }

    init_display();
    generate_bushes();
    for (int i = 0; i < 50; i++)
        generate_coin(0);
    for (int i = 0; i < 5; i++)
        generate_beast();

    beast_t *bs = get_beasts();

    pthread_create(&rounds, NULL, &rounds_counting, NULL);
    pthread_create(&display, NULL, &update_display, NULL);
    for (int i = 0; i < MAX_BEASTS; i++)
        pthread_create(&beasts_pth[i], NULL, &beast_movement, &bs[i]);

    pthread_create(&joining, NULL, &client_joining, &fd);

    char c;
    while ((c = getch()) != 'q' && c != 'Q') {
        switch (c) {
            case 'B':
            case 'b':
                generate_beast();
                break;
            case 'T':
            case 't':
            case 'c':
                generate_coin(c);
                break;
            default:
                break;
        }
    }

    terminate();
    close(fd);
    return 0;
}

void terminate() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        pthread_cancel(communication[i]);
    }
    pthread_cancel(joining);
    for (int i = 0; i < MAX_BEASTS; i++)
        pthread_cancel(beasts_pth[i]);
    pthread_cancel(rounds);
    pthread_cancel(display);
    destroy_display();
    pthread_mutex_destroy(&m_client);
}


void* client_communication(void* inp) {
    int i = *(int*)inp;
    const char *fifo;
    if (i == 0) fifo = PL1_F_R;
    else if (i == 1) fifo = PL2_F_R;
    else if (i == 2) fifo = PL3_F_R;
    else if (i == 3) fifo = PL4_F_R;

    if (mkfifo(fifo, 0666) == -1 && errno != EEXIST) {
        printf("Cannot create fifo %s\n", fifo);
        return NULL;
    }

    int action;
    enum move dir;
    int fd;
    fd = open(fifo, O_RDONLY);
    if (fd == -1) {
        printf("Cannot open fifo %s\n", fifo);
        return NULL;
    }

    int err;
    while(1) {
        err = read(fd, &action, sizeof(int));
        if (action == EXIT || err <= 0) {
            pthread_mutex_lock(&m_client);
            server.active_users--;
            memset(&players[i], 0, sizeof(player_t));
            memset(&client_share[i], 0, sizeof(client_sharing_t));
            pthread_mutex_unlock(&m_client);
            break;
        }
        else if (action == MOVE_UP) dir = UP;
        else if (action == MOVE_RIGHT) dir = RIGHT;
        else if (action == MOVE_DOWN) dir = DOWN;
        else if (action == MOVE_LEFT) dir = LEFT;
        else dir = NONE;

        pthread_mutex_lock(&m_client);
        move_player(&client_share[i].player, dir);
        players[i] = client_share[i].player;
        players_collision(players);
        pthread_mutex_unlock(&m_client);
    }
    close(fd);
    return NULL;
}

void* client_sending(void* inp) {
    int i = *(int*)inp;
    const char *fifo;
    if (i == 0) fifo = PL1_F_WR;
    else if (i == 1) fifo = PL2_F_WR;
    else if (i == 2) fifo = PL3_F_WR;
    else if (i == 3) fifo = PL4_F_WR;

    if (mkfifo(fifo, 0666) == -1 && errno != EEXIST) {
        printf("Cannot create fifo %s\n", fifo);
        return NULL;
    }

    int fd = open(fifo, O_RDWR);
    if (fd == -1) {
        printf("Cannot open fifo %s\n", fifo);
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&m_client);
        get_player_map(&client_share[i]);
        client_share[i].server_info = server;
        client_share[i].player = players[i];
        if (write(fd, &client_share[i], sizeof(client_sharing_t)) <= 0) {
            pthread_mutex_unlock(&m_client);
            break;
        }
        pthread_mutex_unlock(&m_client);
        sleep(1);
    }
    close(fd);
    return NULL;
}

void* client_joining(void* inp) {
    int fd[2];
    fd [0] = *(int*)inp;


    int i;
    pid_t client_pid;
    while (1) {
        client_pid = 0;
        read(fd[0], &client_pid, sizeof(pid_t));
        if (client_pid && server.active_users != MAX_PLAYERS) {
            pthread_mutex_lock(&m_client);
            i = server.active_users;
            init_player(&players[i]);
            players[i].num = ++(server.active_users);
            players[i].PID = client_pid;
            client_share[i].player = players[i];
            client_share[i].server_info = server;
            pthread_create(&communication[i], NULL, &client_communication, &i);
            pthread_create(&sending[i], NULL, &client_sending, &i);

            fd[1] = open(CONNECT_WR, O_WRONLY);
            if (fd[1] == -1) {
                printf("Couldn't open fifo file %s to write\n", CONNECT_WR);
                close(fd[0]);
                pthread_mutex_unlock(&m_client);
                return NULL;
            }
            write(fd[1], &client_share[i], sizeof(client_sharing_t));
            close(fd[1]);

            pthread_mutex_unlock(&m_client);
        }
    }
}

void* beast_movement(void* inp) {
    beast_t *beast = (beast_t*)inp;
    while(1) {
        pthread_mutex_lock(&m_client);
        beast_move(beast, players);
        pthread_mutex_unlock(&m_client);
        sleep(1);
    }
}

void* update_display(void* inp) {
    while(1) {
        pthread_mutex_lock(&m_client);
        update_map(players);
        update_info(players, server.server_round);
        refresh_all();
        pthread_mutex_unlock(&m_client);
        sleep(1);
    }
}

void* rounds_counting(void* inp) {
    while (1) {
        pthread_mutex_lock(&m_client);
        server.server_round++;
        pthread_mutex_unlock(&m_client);
        sleep(1);
    }
}