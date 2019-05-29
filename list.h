#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "board_library.h"

typedef struct card{
    int x;
    int y;
    char v[3];
    struct card *next;
}card;


typedef struct alarmstruct{
    play_response *resp;
    //card *c;
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
    card *clist;
    struct player *next;
} player;

typedef struct colour{
    int r;
    int g;
    int b;
}colour;

/*typedef struct card{
    int x;
    int y;
    char v[3];
    struct card *next;
}card;*/

player * getlist();
void addcard (player *p, int x, int y, char v[3]);
void removecard(player *p, int nremove);
void sendstate(int pfd);
void sendall(play_response *resp, char *colour);
//int fillboard(int sfd);


void addplayer(int fd);
void removeplayer(int fd);
void removefirst();
void setscore();
void freeplist();
void printlist();
void colourvector();
void initcolour(int i);