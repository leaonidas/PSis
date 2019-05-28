#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


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


#define PORT 3000
#define WAITSP 5

int done=0;

static void handler(int signum){
    pthread_exit(NULL);
}

void *listenalarm(void *alrm){
    struct alarmstruct *alarmptr = (struct alarmstruct *) alrm;
    
    time_t before=time(NULL);
    char send[8];
    sprintf(send, "%d %d %d\n", 0, 0, 0);
    printf("In alarm!\n");
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

        if(p->resp->code==2){
        	(*p).score++;
        	addcard(p, board_x, board_y, p->resp->str_play2);
        }

        printf("resp play depois de board play %d\n", p->resp->play1[0]);
        printf("responde code: %d\n", p->resp->code);
        sprintf(colour, "%d %d %d\n", p->r, p->g, p->b);
        write(p->pfd, p->resp, sizeof(*(p->resp)));
        write(p->pfd, colour, sizeof(colour));
       	
        
        if(p->resp->code==1){
            alarmptr->resp=p->resp;
            alarmptr->pfd=p->pfd;
            alarmptr->bip=&bip;
            addcard(p, board_x, board_y, p->resp->str_play1);
            printf("After Adding card!\n");
            pthread_create(&alarm_thread, NULL, listenalarm, alarmptr);
            bip=1;
        }
        else if(bip==1){
            printf("entrou alarm kill\n");
            pthread_kill(alarm_thread, SIGUSR1);
            pthread_join(alarm_thread, NULL);
            removecard(p, -1);
            bip=0;
        }
        if(p->resp->code==3){
            (*p).score++;
            write(p->pfd, &p->score, sizeof(p->score));
            done=1;
            printf("done= %d\n", done);
            printf("saiu thread\n");
            write(p->pfd, colour, sizeof(colour));
            //free(alarmptr);
            //pthread_exit(pthread_self());

        }

        if(p->resp->code=-2){
        	removecard(p, p->resp->code);
        }
    }
}


void *accept_thread(void *pass){
    
    int dim=*(int*) pass;
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
        addplayer(pfd);
        printf("Added player to list!\n");
        initcolour(i);
        printlist();
        i++;

        /*send dim*/
        write(pfd, &dim, sizeof(dim));
        printf("Dim sent!\n");
        sendstate(pfd);

        pthread_create(&getlist()->plays_thread, NULL, plays, getlist());
        
    }
}

int main(int argc, char * argv[]){


    /*guardar fd de players(depois lista quiçá)*/	
    int pfd, nsec=0;
    struct colour c[5];

    /*verificação argumentos de entrada para dim*/
    if(argc<2){
        printf("Incorrect arguments!\n");
        exit(-1);
    }
    int dim=atoi(argv[1]);
    printf("%d\n", dim);

    /*fills the vector with colours to be assigned to each player*/
    colourvector();
    
    /*init board*/
    init_board(dim);

    int *pass;
    pass=malloc(sizeof(int));
    *pass=dim;

    pthread_t lst_thread;
    /*create thread to accept new connections*/
    pthread_create(&lst_thread, NULL, accept_thread, pass);
    
    while(1){
        while(!done){
        
        }
        sleep(10);
        init_board(dim);
        done=0;
        setscore();
    }


                            /******SAÍDA DO PROGRAMA!******/
    pthread_kill(lst_thread, SIGUSR1);
    pthread_join(lst_thread, NULL);

    printf("saiu main\n");
    freeplist();

    done=0;
    return 0;

}