#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void addplayer(player **plist, int fd){
    player *new=(player *)malloc(sizeof(player));
    
    new->fd=fd;
    new->next=*plist;
    *plist=new;
    return;
}

void removeplayer(player **plist, int fd){
    player *aux, *remove;
    
    if(*plist->next==NULL){
        if(*plist->fd==fd)
            free(*plist);
    }
    else{
        if(*plist->fd==fd){
            remove=(*plist);
            *plist=*plist->next;
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