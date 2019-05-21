#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "board_library.h"

typedef struct alarmstruct{
    play_response *resp;
    int pfd;
    int *bip;
}alarmstruct;

typedef struct player{
	pthread_t plays_thread;
    int pfd;
    int r;
    int g;
    int b;
    play_response *resp;
    int score;
    struct player *next;
} player;

void addplayer(player **plist, int fd);
void removeplayer(player **plist, int fd);
void removefirst(player **plist);
void printlist(player *plist);