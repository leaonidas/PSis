#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "list.h"
#include "UI_library.h"

player *plist = NULL;
struct colour c[5];


player * getlist(){
    return plist;
}


void addcard (player *p, int x, int y, char v[3]){

    card *new=(card*)malloc(sizeof(card));

    new->x=x;
    new->y=y;
    strcpy(new->v, v);
    new->next=p->clist;
    p->clist=new;
    free(new);

}

/*nremove is the code corresponding to the play it can either be -1 or -2*/
void removecard(player *p, int nremove){
    int i = 0;
    nremove = nremove*(-1);
    card *top;

    while(i<nremove){
        top = p->clist->next;
        free(p->clist);
        p->clist=top;
        i++;
    }
}

void sendstate(int pfd){
    player *aux = plist;
    char colour[8];
    colour[0]='\0';

    printf("Inside sendstate!\n");

    if(aux->next!=NULL){
        sprintf(colour, "%d %d %d\n", aux->next->r, aux->next->g, aux->next->b);
        write(pfd, colour, sizeof(colour));
        printf("Entered here. Shouldn't!\n");
    }

    while(aux!=NULL){
        /*verifies if my pfd is different than the one
        I am sending data to*/
        printf("Inside while loop!\n");
        if(aux->pfd==pfd){
            aux=aux->next;
            break;
        }
        /*cycles through cards until it reaches the last one
        and sends the corresponding data*/
        while(aux->clist!=NULL){
            printf("Sending card!\n");
            write(pfd, aux->clist, sizeof(aux->clist));
            aux->clist=aux->clist->next;
        }
        /*when it reaches the last card has to warn the client
        that the colour will be changed*/
        if(aux->clist==NULL){
            card *change = (card*)malloc(sizeof(card));
            change->x=-2;
            change->y=-2;
            change->v[0]='\0';
            printf("Sending card to change colour!\n");
            write(pfd, change, sizeof(change));
            free(change);
            sprintf(colour, "%d %d %d\n", aux->next->r, aux->next->g, aux->next->b);
            write(pfd, colour, sizeof(colour));
        }

        aux=aux->next;
    }
    printf("Exit while!\n");
    /*when it reaches the end of cards to send warns the
    client that the last card has been sent*/
    if(aux==NULL){
        //card *last = (card*)malloc(sizeof(card));
        printf("Sending last!\n");
        card last;
        last.x=-1;
        last.y=-1;
        last.v[0]='\0';
        write(pfd, &last, sizeof(last));
        //free(last);
    }

}

int fillboard(int sfd){
    int x, y, r, g, b;
    int i=0;
    char vaux[3], colour[8];
    card rcvcard;
    player *aux=plist;

    while(aux!=NULL){
        i++;
        aux=aux->next;
    }
    if(i>1){
        read(sfd, colour, sizeof(colour));
        sscanf(colour, "%d %d %d\n", &r, &g, &b);
        printf("In fillboard!\n");
    }
    

    while(1){
        read(sfd, &rcvcard, sizeof(rcvcard));
        printf("x: %d  y: %d\n", rcvcard.x, rcvcard.y);
        x=rcvcard.x;
        y=rcvcard.y;
        strcpy(vaux, rcvcard.v);

        /*reaches the end of all cards*/
        if(x==-1 && y==-1){
            return 1;
        }
        /*reaches the end of cards belonging to one player
        exits to change colour*/
        if(x==-2 && y==-2){
            read(sfd, colour, sizeof(colour));
            sscanf(colour, "%d %d %d\n", &r, &g, &b);
            break;
        }
        paint_card(x, y, r, g, b);
        write_card(x, y, vaux, 200, 200, 200);
    }
}


void addplayer(int pfd){
    player *new=(player *)malloc(sizeof(player));
    
    new->pfd=pfd;
    new->next=plist;
    new->score=0;
    new->resp=malloc(sizeof(new->resp));
    new->clist=NULL;
    plist=new;
    return;
}

void removebyfd(int pfd){
    player *aux, *remove;
    
    if(plist->next==NULL){
        if(plist->pfd==pfd)
            free(plist);
    }
    else{
        if(plist->pfd==pfd){
            remove=(plist);
            plist=plist->next;
            free(remove);
        }
        aux=plist;
        while(aux->next!=NULL){
            if(aux->next->pfd==pfd){
                remove=aux->next;
                aux->next=aux->next->next;
                free(remove);
            }
            aux=aux->next;
        }
    }
}

void removefirst(){
    player *remove;

    remove=plist;

    plist=plist->next;
    free(remove);
}

void printlist(){
    player *aux=plist;
    int i=0;
    
    while(aux!=NULL){
        printf("Player %d fd is: %d\n",i, aux->pfd);
        printf("Red: %d Green: %d Blue: %d\n", aux->r, aux->g, aux->b);
        i++;
        aux=aux->next;
    }
}

/*sets score to zero*/
void setscore(){
    player *aux=plist;
    while(aux!=NULL){
        aux->score=0;
        aux=aux->next;
    }
}

void freeplist(){
    player *aux=plist;
    while(aux!=NULL){
        free(aux->resp);
        removefirst(&plist);
        aux=plist;
    }
}



void colourvector(){

    /*green*/
    c[0].r=0;
    c[0].g=255;
    c[0].b=0;
    /*blue*/
    c[1].r=0;
    c[1].g=0;
    c[1].b=255;
    /*yellow*/
    c[2].r=255;
    c[2].g=255;
    c[2].b=0;
    /*pink*/
    c[3].r=255;
    c[3].g=105;
    c[3].b=180;
    /*orange*/
    c[4].r=255;
    c[4].g=102;
    c[4].b=0;

}

void initcolour(int i){

    plist->r=c[i].r;
    plist->g=c[i].g;
    plist->b=c[i].b;

}

