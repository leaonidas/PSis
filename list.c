#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"


void addplayer(player **plist, int fd){
    player *new=(player *)malloc(sizeof(player));
    
    new->fd=fd;
    new->next=*plist;
    *plist=new;
    return;
}

void removeplayer(player **plist, int fd){
    player *aux, *remove;
    
    if((*plist)->next==NULL){
        if((*plist)->fd==fd)
            free(*plist);
    }
    else{
        if((*plist)->fd==fd){
            remove=(*plist);
            *plist=(*plist)->next;
            free(remove);
        }
        aux=*plist;
        while(aux->next!=NULL){
            if(aux->next->fd==fd){
                remove=aux->next;
                aux->next=aux->next->next;
                free(remove);
            }
            aux=aux->next;
        }
    }
}

void printlist(player *plist){
    player *aux=plist;
    int i=0;
    
    while(aux!=NULL){
        printf("Player %d fd is: %d\n",i, aux->fd);
        i++;
        aux=aux->next;
    }
}