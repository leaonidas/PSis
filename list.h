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

typedef struct acceptstruct{
    player *plist;
    int dim;
    struct colour *c;
}acceptstruct;

typedef struct colour{
    int r;
    int g;
    int b;
}colour;

void addplayer(player **plist, int fd);
void removeplayer(player **plist, int fd);
void removefirst(player **plist);
void printlist(player *plist);
void colourvector(colour *c);
void initcolour(player **plist, colour *clist, int i);