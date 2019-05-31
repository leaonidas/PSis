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

int done=0, bye=0;

void *accept_thread(void *pass);

/*signal to exit thread*/
static void handler(int signum){
    pthread_exit(NULL);
}

/*thread to count the time between the first and secound picks
if player picks the secound card before the 5 second this thread is send a signal to exit by handler
if player doesnt pick a card in the 5 seconds that card is removed from the boards*/
void *listenalarm(void *alrm){
    struct alarmstruct *alarmptr = (struct alarmstruct *) alrm;
    
    time_t before=time(NULL);

    while(1){
        if(time(NULL)-before>=WAITSP){
        	/*sends card removal to all players and removes card from the list*/
            sendall(alarmptr->p->resp, alarmptr->p);
            removecard(alarmptr->p, 1);
            /*change value of code and first card*/
            alarmptr->p->resp->code=-1;
            alarmptr->p->resp->play1[0]=-1;
            changeplay(-1);

            *(alarmptr->bip)=0;

            pthread_exit(NULL);
        }
    }
    return 0;
}


/*thread that deals with the communications of one player*/
void *plays(void *pass){
    struct player *p = (struct player *) pass;
    struct play_response resp;

    printf("pfd: %d\n", p->pfd);

    signal(SIGUSR1, handler);
    int bip=0;
    player *aux;

    int board_x, board_y, wfd, n, m, numpl;


    pthread_t alarm_thread;
    alarmstruct *alarmptr=malloc(sizeof(struct alarmstruct));

    while(1){

    	/*while(countlist()<=1){
    		sleep(1);
    	}*/

    	printf("Ainda chegou aqui!\n");
        n=read(p->pfd, &board_x, sizeof(board_x));
        m=read(p->pfd, &board_y, sizeof(board_y));

        if(n==-1 || m==-1 || n==0 || m==0){
        	printf("Someone left the game!\n");
        	removebyfd(p->pfd);
        	numpl=countlist();
        			
        	if(numpl==0) {
        		done=1;
        		bye=1;
        	}
        	else if(numpl==1){
        		resp.code=-4;
        		write(getlist()->pfd, &resp, sizeof(resp));
        	}
        	free(alarmptr);
        	pthread_exit(NULL);

        }
        
        printf("BIP: %d\n", bip);
        printf("board_x= %d\n", board_x);
        printf("board_y= %d\n", board_y);
        printf("resp play depois de 5 sec %d\n", p->resp->play1[0]);

        board_play(board_x, board_y, p->resp);

        sendall(p->resp, p);

        if(p->resp->code==2){
        	(*p).score++;
        	p->resp->play1[0]=-1;
        	addcard(p, board_x, board_y, p->resp->str_play2);
        }

        printf("resp play depois de board play %d\n", p->resp->play1[0]);
        printf("responde code: %d\n", p->resp->code);

       	
        
        if(p->resp->code==1){
        	addcard(p, board_x, board_y, p->resp->str_play1);
            alarmptr->p=p;
            alarmptr->pfd=p->pfd;
            alarmptr->bip=&bip;
            //alarmptr->c=p->clist;
            pthread_create(&alarm_thread, NULL, listenalarm, alarmptr);
            bip=1;
        }
        else if(bip==1){
            printf("entrou alarm kill\n");
            pthread_kill(alarm_thread, SIGUSR1);
            pthread_join(alarm_thread, NULL);
            //removecard(p, -1);
            bip=0;
        }
        if(p->resp->code==3){
            (*p).score++;
            aux=getlist();
            printf("Game finish!\n\n");
            printf("\n");

            /*verifies winner and sends messages to all players*/
            wfd=verifywinner();
            while(aux!=NULL){
            	if(aux->pfd==wfd) write(aux->pfd, &aux->score, sizeof(p->score));
            	else{
            		aux->score=0;
            		write(aux->pfd, &aux->score, sizeof(p->score));
            	}
            	aux=aux->next;
            }
            /*removes all cards from the list*/
            aux=getlist();
            while(aux!=NULL){
            	while(aux->clist!=NULL){
            		removecard(aux, 1);
            	}
            	aux=aux->next;
            }
            done=1;
            printf("done= %d\n", done);
            printf("saiu thread\n");
        }

        if(p->resp->code==-2){
        	removecard(p, 1);
        	p->resp->play1[0]=-1;
        }
    }
}

/*socket to accept players connections*/
void *accept_thread(void *pass){
    
    int dim=*(int*) pass;
    int pfd, i=0;
    play_response resp;
    resp.code=-4;

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

        /*colour attribution*/
        initcolour(i);
        printlist();
        i++;

        /*send dim*/
        write(pfd, &dim, sizeof(dim));
        printf("Dim sent!\n");

        /*send current state of the board*/
        sendstate(pfd);

        if(countlist()==1) {
        	write(pfd, &resp, sizeof(resp));
        }
        else if(countlist()==2){
        	write(getlist()->next->pfd, &i, sizeof(int));
        }
        /*create thread to player*/
        pthread_create(&getlist()->plays_thread, NULL, plays, getlist());
        
    }
}

int main(int argc, char * argv[]){

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

    /*pointer to pass in thread*/
    int *pass;
    pass=malloc(sizeof(int));
    *pass=dim;

	/*create thread to accept new connections*/
    pthread_t lst_thread;
    pthread_create(&lst_thread, NULL, accept_thread, pass);
    

    /*game cycle*/
    while(!bye){
        while(!done){
        
        }
        /*game ended and another begins*/
        if(bye!=1){
        	sleep(10);
        	init_board(dim);
        	done=0;
        	setscore();
        }   
    }


    /*game exit*/
    pthread_kill(lst_thread, SIGUSR1);
    pthread_join(lst_thread, NULL);

    printf("saiu main\n");
    freeplist();

    done=0;
    return 0;

}