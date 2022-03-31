//
// Created by serene on 26.01.2022.
//

#include "client_common.h"
#include "client_display.h"

void* update_display(void* inp);

client_sharing_t client;

pthread_t display;

int main() {

    int fd[2];
    fd[0] = open(CONNECT_WR, O_WRONLY);
    if (fd[0] == -1) {
        printf("Couldn't open fifo file\n");
        return 1;
    }
    fd[1] = open(CONNECT_R, O_RDWR);
    if (fd[1] == -1) {
        printf("Couldn't open fifo file\n");
        return 1;
    }

    pid_t pid = getpid();
    if (write(fd[0], &pid, sizeof(pid_t)) < 0) {
        printf("Server not running\n");
        close(fd[0]);
        return 1;
    }

    read(fd[1], &client, sizeof(client_sharing_t));
    close(fd[0]);
    close(fd[1]);

    int i = client.player.num;
    const char *fifo[2];
    if (i == 1) { fifo[0] = PL1_F_WR; fifo[1] = PL1_F_R; }
    else if (i == 2) { fifo[0] = PL2_F_WR; fifo[1] = PL2_F_R; }
    else if (i == 3) { fifo[0] = PL3_F_WR; fifo[1] = PL3_F_R; }
    else if (i == 4) { fifo[0] = PL4_F_WR; fifo[1] = PL4_F_R; }

    fd[1] = open(fifo[1], O_RDONLY);
    if (fd[1] == -1) {
        printf("Couldn't open fifo %s\n", fifo[1]);
        return 1;
    }
    fd[0] = open(fifo[0], O_WRONLY);
    if (fd[0] == -1) {
        printf("Couldn't open fifo %s\n", fifo[0]);
        close(fd[1]);
        return 1;
    }

    init_display();

    pthread_create(&display, NULL, &update_display, &fd[1]);

    int c;
    while(1) {
        c = (int)getch();
        write(fd[0], &c, sizeof(int));
        if (c == (int)'q') break;
        sleep(1);
    }

    pthread_cancel(display);
    destroy_display();
    close(fd[0]);
    close(fd[1]);

    return 0;
}

void* update_display(void* inp) {
    int fd = *(int*)inp;
    while(1) {
        read(fd, &client, sizeof(client_sharing_t));
        update_map(client.player_map);
        update_info(client.player);
        update_header(client.server_info.server_pid, client.server_info.server_round);
        refresh_all();
    }
}
