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
    printf("New card x: %d   y: %d\n", p->clist->x, p->clist->y);
    printf("Card added to list\n");

}

/*nremove is the code corresponding to the play it can either be -1 or -2*/
void removecard(player *p, int nremove){
    int i = 0;
    printf("Inside removecard!\n");
    printf("pfd: %d\n", p->pfd);
    card *remove;

    /*while(p->clist!=NULL){
        printf("card_x: %d   card_y: %d\n", p->clist->x, p->clist->y);
        p->clist=p->clist->next;
    }*/

    while(i<nremove){
        printf("quantas vezes? :%d\n", i);
        remove=p->clist;
        printf("Removing card x: %d  y:%d\n", p->clist->x, p->clist->y);
        p->clist = p->clist->next;
        free(remove);
        i++;
    }

}

void sendstate(int pfd){
    player *aux = plist;
    card *auxcl=plist->clist;
    card change;
    int i=0;

    printf("Inside sendstate!\n\n");

    while(aux!=NULL){
        i++;
        aux=aux->next;
    }
    write(pfd, &i, sizeof(i));
    aux=plist;

    /*Sends the colour of next player's cards*/
    if(aux->next!=NULL){
        //sprintf(colour, "%d %d %d\n", aux->next->r, aux->next->g, aux->next->b);
        //printf("Next player card colour: %d %d %d\n\n", aux->next->r, aux->next->g, aux->next->b);
        //write(pfd, colour, sizeof(colour));
        sendcolour(aux->next->r, aux->next->g, aux->next->b, pfd);
    }

    /*while(aux->clist!=NULL){
        printf("card x: %d   card y: %d\n", aux->clist->x, aux->clist->y);
        aux->clist=aux->clist->next;
    }*/
            
    

    while(aux!=NULL){
        /*verifies if my pfd is different than the one
        I am sending data to*/
        printf("Inside while loop!\n");
        if(aux->pfd!=pfd){
        /*cycles through cards until it reaches the last one
        and sends the corresponding data*/
            while(auxcl!=NULL){
                printf("Sending this card: %d %d\n", auxcl->x, auxcl->y);
                write(pfd, auxcl, sizeof(*(auxcl)));
                auxcl=auxcl->next;
            }
            /*when it reaches the last card has to warn the client
            that the colour will be changed*/
            if(auxcl==NULL){
                if(aux->next!=NULL){
                    //card *change = (card*)malloc(sizeof(card));
                    change.x=-2;
                    change.y=-2;
                    change.v[0]='\0';
                    printf("Sending card to change colour!\n");
                    write(pfd, &change, sizeof(change));
                    //colour[0]='\0';
                    //sprintf(colour, "%d %d %d\n", aux->next->r, aux->next->g, aux->next->b);
                    //write(pfd, colour, sizeof(colour));
                    sendcolour(aux->next->r, aux->next->g, aux->next->b, pfd);
                }
            }
        }

        aux=aux->next;
        if (aux!=NULL) auxcl=aux->clist;

    }
    printf("Exit while!\n");
    /*when it reaches the end of cards to send warns the
    client that the last card has been sent*/
    
        //card *last = (card*)malloc(sizeof(card));
        printf("Sending last!\n");
        card last;
        last.x=-1;
        last.y=-1;
        last.v[0]='\0';
        write(pfd, &last, sizeof(last));
        //free(last);
    

}

void sendall(play_response *resp, player *p){
    player *aux=plist;

    while(aux!=NULL){
        //printf("Colour inside sendall: %s\n", colour);
        write(aux->pfd, resp, sizeof(*resp));
        //write(aux->pfd, colour, sizeof(colour));
        sendcolour(p->r, p->g, p->b, aux->pfd);
        aux=aux->next;
    }

}

void sendcolour(int r, int g, int b, int pfd){
    printf("Inside sendcolour: %d %d %d\n", r, g, b);
    write(pfd, &r, sizeof(int));
    write(pfd, &g, sizeof(int));
    write(pfd, &b, sizeof(int));
}


void addplayer(int pfd){
    player *new=(player *)malloc(sizeof(player));
    
    new->pfd=pfd;
    new->next=plist;
    new->score=0;
    new->resp=malloc(sizeof(new->resp));
    new->resp->play1[0]=-1;
    new->clist=NULL;
    plist=new;
    return;
}

int countlist(){
    player *aux=plist;
    int count=0;

    while(aux!=NULL){
        count++;
        aux=aux->next;
    }
    return count;
}

void removebyfd(int pfd){
    player *aux, *remove;
    
    if(plist->next==NULL){
        if(plist->pfd==pfd)
            free(plist);
            plist=NULL;
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
        printf("Player %d fd is: %d\nPlayer score is: %d\n",i, aux->pfd, aux->score);
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

int verifywinner(){

    int wfd=0, maxscore=0;
    player *aux=plist;

    while(aux!=NULL){
        printf("Max score player is: %d\nPlayer %d score is: %d\n", aux->pfd, aux->pfd, aux->score);
        if(aux->score > maxscore){
            maxscore = aux->score;
            wfd=aux->pfd;
        }
        aux=aux->next;
    }

    return wfd;

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

