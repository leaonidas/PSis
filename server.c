#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//#include "server_library.h"

//#include "board_library.h"
#include "list.h"
#include "UI_library.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
//#include "list.h"



#define PORT 3000
#define WAITSP 5

int done=0;

static void handler(int signum){
    pthread_exit(NULL);
}

void *listenalarm(void *pass){
    struct alarmstruct *alarmptr = (struct alarmstruct *) pass;
    
    time_t before=time(NULL);
    char send[8];
    sprintf(send, "%d %d %d\n", 0, 0, 0);

    while(1){
        if(time(NULL)-before>=WAITSP){
            printf("ENTROU\n");
            alarmptr->resp->code=-1;
            write(alarmptr->pfd, alarmptr->resp, sizeof(*alarmptr->resp));
            write(alarmptr->pfd, send, sizeof(send));
            alarmptr->resp->play1[0]=-1;
            *(alarmptr->bip)=0;
            changeplay(-1);
            pthread_exit(NULL);
        }
    }
    return 0;
}

void *plays(void *pass){
    struct player *p = (struct player *) pass;

    signal(SIGUSR1, handler);

    int board_x, board_y;
    int bip=0;
    char colour[8];
    colour[0]='\0';

    pthread_t alarm_thread;
    alarmstruct *alarmptr=malloc(sizeof(struct alarmstruct));

    while(1){

        read(p->pfd, &board_x, sizeof(board_x));
        read(p->pfd, &board_y, sizeof(board_y));
        
        printf("BIP: %d\n", bip);
        printf("board_x= %d\n", board_x);
        printf("board_y= %d\n", board_y);
        printf("resp play depois de 5 sec %d\n", p->resp->play1[0]);


        *(p->resp)=board_play(board_x, board_y);

        if(p->resp->code==2) p->score++;

        printf("resp play depois de board play %d\n", p->resp->play1[0]);
        printf("responde code: %d\n", p->resp->code);
        sprintf(colour, "%d %d %d\n", p->r, p->g, p->b);
        printf("%s\n", colour);
        write(p->pfd, p->resp, sizeof(*(p->resp)));
        write(p->pfd, colour, sizeof(colour));
       	
        
        if(p->resp->code==1){
            alarmptr->resp=p->resp;
            alarmptr->pfd=p->pfd;
            alarmptr->bip=&bip;
            pthread_create(&alarm_thread, NULL, listenalarm, alarmptr);
            bip=1;
        }
        else if(bip==1){
            printf("entrou alarm kill\n");
            pthread_kill(alarm_thread, SIGUSR1);
            pthread_join(alarm_thread, NULL);
            bip=0;
        }
        if(p->resp->code==3){
            p->score++;
            write(p->pfd, &p->score, sizeof(p->score));
            done=1;
            printf("done= %d\n", done);
            printf("saiu thread\n");
            write(p->pfd, colour, sizeof(colour));
            //free(alarmptr);
            //pthread_exit(pthread_self());

        }
    }
}


void *accept_thread(void *pass){
    
    struct acceptstruct *acpt = (struct acceptstruct *) pass;
    int pfd, i=0;
    
    /*criar socket de listen*/
    struct sockaddr_in local_addr;
    int lst_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(lst_fd==-1){
        perror("socket: ");
        exit(-1);
    }
    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(PORT);
    local_addr.sin_addr.s_addr=INADDR_ANY;

    /*bind*/
    if(bind(lst_fd, (struct sockaddr *)&local_addr,   sizeof(local_addr))==-1){
      perror("bind: ");
      exit(-1);
    }
    
    /*listen*/
    listen(lst_fd, 5);
    
    while(1){
        
        /*add player*/
        pfd=accept(lst_fd, NULL, NULL);
        printf("Player connected!\n");
    
        /*adds player one to the list*/
        addplayer(&(acpt->plist), pfd);
        initcolour(&(acpt->plist), acpt->c, i);
        printlist(acpt->plist);
        i++;

        /*send dim*/
        write(pfd, &acpt->dim, sizeof(acpt->dim));

        pthread_create(&acpt->plist->plays_thread, NULL, plays, acpt->plist);
        
    }
}

int main(int argc, char * argv[]){


    /*guardar fd de players(depois lista quiçá)*/	
    int pfd, nsec=0;
    struct colour c[5];
    //player *plist;


    /*verificação argumentos de entrada para dim*/
    if(argc<2){
        printf("Incorrect arguments!\n");
        exit(-1);
    }
    int dim=atoi(argv[1]);
    printf("%d\n", dim);

    /*fills the vector with colours to be assigned to each player*/
    colourvector(c);
    for(int i=0; i<5; i++){
    	printf("r: %d g: %d b: %d\n", c[i].r, c[i].g, c[i].b);
    }
    
    /*init board*/
    init_board(dim);
    
    acceptstruct *pass = malloc(sizeof(acceptstruct));
    pass->plist=NULL;
    pass->dim=dim;
    pass->c=c;

    pthread_t lst_thread;
    /*create thread to accept new connections*/
    pthread_create(&lst_thread, NULL, accept_thread, pass);
    
    while(1){
        while(!done){
        
        }
        sleep(10);
        init_board(dim);
        done=0;
        player *aux=pass->plist;
        while(aux!=NULL){
            aux->score=0;
            aux=aux->next;
        }
    }
    

                            /******SAÍDA DO PROGRAMA!******/
    pthread_kill(lst_thread, SIGUSR1);
    pthread_join(lst_thread, NULL);

    printf("saiu main\n");
    /*Frees allocated memory*/
    player *aux=pass->plist;
    while(aux!=NULL){
        free(aux->resp);
        removefirst(&pass->plist);
        aux=pass->plist;
    }

    done=0;
    return 0;

}