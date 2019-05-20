#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct player{
    int fd;
    int r;
    int g;
    int b;
    int board_x;
    int board_y;
    //int pflag;
    int score;
    struct player *next;
} player;

void addplayer(player **plist, int fd);
void removeplayer(player **plist, int fd);
void printlist(player *plist);