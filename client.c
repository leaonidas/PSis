#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "board_library.h"
#include "UI_library.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 3000

int done=0;


void *listenserver(void *pass){
    
    /*int *sfdpointer=pass;
    int sfd=*sfdpointer;*/
    
    int sfd=*(int*) pass;
    
    play_response resp;
    while(!done){
        /*receives play response*/
        
        read(sfd, &resp, sizeof(resp));
        printf("code in resp: %d\n", resp.code);
                    
        /*switch case response code*/
        switch (resp.code) {
            case 1:
                paint_card(resp.play1[0], resp.play1[1] , 7, 200, 100);
                write_card(resp.play1[0], resp.play1[1], resp.str_play1, 200, 200, 200);
                break;
            case 3:
                done = 1;
            case 2:
                paint_card(resp.play1[0], resp.play1[1] , 107, 200, 100);
                write_card(resp.play1[0], resp.play1[1], resp.str_play1, 0, 0, 0);
                paint_card(resp.play2[0], resp.play2[1] , 107, 200, 100);
                write_card(resp.play2[0], resp.play2[1], resp.str_play2, 0, 0, 0);
                break;
            case -2:
                paint_card(resp.play1[0], resp.play1[1] , 107, 200, 100);
                write_card(resp.play1[0], resp.play1[1], resp.str_play1, 255, 0, 0);
                paint_card(resp.play2[0], resp.play2[1] , 107, 200, 100);
                write_card(resp.play2[0], resp.play2[1], resp.str_play2, 255, 0, 0);
                sleep(2);
                paint_card(resp.play1[0], resp.play1[1], 255, 255, 255);
                paint_card(resp.play2[0], resp.play2[1], 255, 255, 255);
                break;
            case -1:
                /*case 2nd card is not pick after 5 seconds*/
                paint_card(resp.play1[0], resp.play1[1], 255, 255, 255);
                write_card(resp.play1[0], resp.play1[1], resp.str_play1, 255, 0, 0);
        }
    }
    //return 0;
    pthread_exit(pthread_self());
}




int main(int argc, char * argv[]){
    
    int dim, close=0;
    
    /*init events*/
    SDL_Event event;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }
    if(TTF_Init()==-1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }
    
    /*arguments*/
    if(argc<2){
        printf("Incorrect arguments!\n");
        exit(-1);
    }

    /*create socket*/
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sfd==-1){
        perror("socket: ");
        exit(-1);
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(PORT);
    inet_aton(argv[1], &server_addr.sin_addr);

    /*connect*/
    if(connect(sfd, (const struct sockaddr *) &server_addr, sizeof(server_addr))==-1){
        printf("Error connecting!\n");
        exit(-1);
    }
    printf("Connected!\n");

    /*read dimension and create board*/
    read(sfd, &dim, sizeof(dim));
    
    printf("dim= %d\n", dim);
    
    create_board_window(300, 300, dim);

    pthread_t lst_thread;
    
    /*thread to receive messages from server*/
    /*passing arguments to the thread*/
    int *pass;
    pass=malloc(sizeof(int));
    *pass=sfd;
    //pthread_create(&lst_thread, NULL, listenserver, pass);
    
    while(!close){
        pthread_create(&lst_thread, NULL, listenserver, pass);
        while(!done){
            while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_QUIT: {
                        done=SDL_TRUE;
                        close=SDL_TRUE;
                        break;
                    }
                    case SDL_MOUSEBUTTONDOWN:{
                        /*gets the x and y from button and sends to server*/
                        int board_x, board_y;
                        get_board_card(event.button.x, event.button.y, &board_x, &board_y);
                        write(sfd, &board_x, sizeof(board_x));
                        write(sfd, &board_y, sizeof(board_y));
                    }
                }
            } 
        }

        int score;
        read(sfd, &score, sizeof(score));
        printf("WINNER SCORE: %d\n", score);
        done=0;
        close_board_windows();
        sleep(10);
        create_board_window(300, 300, dim);
    }
    
    printf("fim\n");
    close_board_windows();
    
    free(pass);
    
}