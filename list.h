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

typedef struct alarmstruct{
    player *p;
    int pfd;
    int *bip;
}alarmstruct;

typedef struct colour{
    int r;
    int g;
    int b;
}colour;


player * getlist();
/*card's list funtions*/
void addcard (player *p, int x, int y, char v[3]);
void removecard(player *p, int nremove);

/*communications functions*/
void sendstate(int pfd);
void sendall(play_response *resp, player *p);
void sendcolour(int r, int g, int b, int pfd);

/*player's list funtions*/
void addplayer(int fd);
int countlist();
void removebyfd(int pfd);
void removeplayer(int fd);
void removefirst();
void setscore();
int verifywinner();
void freeplist();
void printlist();

/*colours funtions*/
void colourvector();
void initcolour(int i);