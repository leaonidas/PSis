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
        i++;
        aux=aux->next;
    }
}

