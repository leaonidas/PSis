#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "board_library.h"
#include "UI_library.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>

#define PORT 3000
#define WAITSP 5

int count=0;

void ALARMhandler(int sig){
    signal(SIGALRM, SIG_IGN);
    count=-1;
    printf("alarmou a gritar\n");
    printf("count tem de ser -1= %d\n", count);
    signal(SIGALRM, ALARMhandler);
}

int main(int argc, char * argv[]){


    /*guardar fd de players(depois lista quiçá)*/	
    int pfd, done=0, nsec=0;
    //player *plist;


    /*verificação argumentos de entrada para dim*/
    if(argc<2){
        printf("Incorrect arguments!\n");
        exit(-1);
    }
    int dim=atoi(argv[1]);

    printf("%d\n", dim);

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
    
    /*init board*/
    init_board(dim);
    
    /*listen*/
    listen(lst_fd, 5);
    
    /*add player*/
    pfd=accept(lst_fd, NULL, NULL);
    printf("Player connected!\n");

        
    write(pfd, &dim, sizeof(dim));
    int score=0;
    play_response resp;
    
    while(!done){
        
        if(count==-1){
            printf("ENTROU\n");
            resp.code=-1;
            write(pfd, &resp, sizeof(resp));
            resp.play1[0]=-1;
            count=0;
        }
        
        int board_x, board_y;
        read(pfd, &board_x, sizeof(board_x));
        read(pfd, &board_y, sizeof(board_y));
        
        
        printf("board_x= %d\n", board_x);
        printf("board_x= %d\n", board_y);
        
        resp=board_play(board_x, board_y);
        if(resp.code==2) score++;
        printf("responde code: %d\n", resp.code);
        write(pfd, &resp, sizeof(resp));
        
        if(resp.code==1){
            signal(SIGALRM, ALARMhandler);
            alarm(WAITSP);
        }
        else{
            alarm(0);
        }
        if(resp.code==3){
            score++;
            write(pfd, &score, sizeof(score));
            done=1;
        }
        
    }
    

	//connect(fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));


}