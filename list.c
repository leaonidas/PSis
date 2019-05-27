#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"


void addplayer(player **plist, int pfd){
    player *new=(player *)malloc(sizeof(player));
    
    new->pfd=pfd;
    new->next=*plist;
    new->score=0;
    new->resp=malloc(sizeof(new->resp));
    *plist=new;
    return;
}

void removebyfd(player **plist, int pfd){
    player *aux, *remove;
    
    if((*plist)->next==NULL){
        if((*plist)->pfd==pfd)
            free(*plist);
    }
    else{
        if((*plist)->pfd==pfd){
            remove=(*plist);
            *plist=(*plist)->next;
            free(remove);
        }
        aux=*plist;
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

void removefirst(player **plist){
    player *remove;

    remove=*plist;

    *plist=(*plist)->next;
    free(remove);
}

void printlist(player *plist){
    player *aux=plist;
    int i=0;
    
    while(aux!=NULL){
        printf("Player %d fd is: %d\n",i, aux->pfd);
        printf("Red: %d Green: %d Blue: %d\n", aux->r, aux->g, aux->b);
        i++;
        aux=aux->next;
    }
}

void colourvector(colour *c){

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

void initcolour(player **plist, colour *clist, int i){

    (*plist)->r=clist[i].r;
    (*plist)->g=clist[i].g;
    (*plist)->b=clist[i].b;

}

