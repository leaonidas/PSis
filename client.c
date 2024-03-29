#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//#include "board_library.h"
#include "list.h"
#include "UI_library.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>


#define PORT 3000

int done=0, wait=0;

void recvcolour(int *r, int *g, int *b, int sfd){
    read(sfd, r, sizeof(int));
    read(sfd, g, sizeof(int));
    read(sfd, b, sizeof(int));
}

int fillboard(int sfd){
    int x, y, r, g, b;
    int i=0;
    char vaux[3], colour[14];
    card rcvcard;


    read(sfd, &i, sizeof(i));
    if(i>1){
        //read(sfd, colour, sizeof(colour));
        //sscanf(colour, "%d %d %d\n", &r, &g, &b);
        recvcolour(&r, &g, &b, sfd);
        printf("Colour received: %d %d %d\n", r, g, b);
    }
    

    while(1){
        read(sfd, &rcvcard, sizeof(rcvcard));
        printf("x: %d  y: %d  vaux: %s\n", rcvcard.x, rcvcard.y, rcvcard.v);
        x=rcvcard.x;
        y=rcvcard.y;
        strcpy(vaux, rcvcard.v);
        printf("vaux:  %s\n", vaux);

        /*reaches the end of all cards*/
        if(x==-1 && y==-1){
            return 1;
        }
        /*reaches the end of cards belonging to one player
        exits to change colour*/
        if(x==-2 && y==-2){
            /*read(sfd, colour, sizeof(colour));
            printf("Colour received: %d %d %d\n", r, g, b);
            sscanf(colour, "%d %d %d\n", &r, &g, &b);*/
            recvcolour(&r, &g, &b, sfd);
        }else{
            paint_card(x, y, r, g, b);
            write_card(x, y, vaux, 0, 0, 0);
        }
    }
}


void *listenserver(void *pass){
    
    /*int *sfdpointer=pass;
    int sfd=*sfdpointer;*/
    
    int sfd=*(int*) pass;
    int r, g, b, x, y, check;

    
    play_response resp;
    while(!done){
        /*receives play response*/
        
        read(sfd, &resp, sizeof(resp));
        /*read(sfd, plays, sizeof(plays));
        sscanf(plays, "%d %d %d\n", &r, &g, &b);
        printf("colour vector: %s\n", plays);
        printf("colour: %d %d %d\n", r, g, b);*/
        printf("Resp code: %d\n", resp.code);
        if (resp.code!=-4) recvcolour(&r, &g, &b, sfd);
        

        /*switch case response code*/
        switch (resp.code) {
            case 1:
                x=resp.play1[0];
                y=resp.play1[1];
                paint_card(resp.play1[0], resp.play1[1] , r, g, b);
                write_card(resp.play1[0], resp.play1[1], resp.str_play1, 200, 200, 200);
                break;
            case 3:
                printf("Closing the game!\n");
                done = 1;
            case 2:
                paint_card(resp.play1[0], resp.play1[1] , r, g, b);
                write_card(resp.play1[0], resp.play1[1], resp.str_play1, 0, 0, 0);
                paint_card(resp.play2[0], resp.play2[1] , r, g, b);
                write_card(resp.play2[0], resp.play2[1], resp.str_play2, 0, 0, 0);
                break;
            case -2:
                paint_card(resp.play1[0], resp.play1[1] , r, g, b);
                write_card(resp.play1[0], resp.play1[1], resp.str_play1, 255, 0, 0);
                paint_card(resp.play2[0], resp.play2[1] , r, g, b);
                write_card(resp.play2[0], resp.play2[1], resp.str_play2, 255, 0, 0);
                sleep(2);
                paint_card(resp.play1[0], resp.play1[1], 255, 255, 255);
                paint_card(resp.play2[0], resp.play2[1], 255, 255, 255);
                break;
            case -1:
                /*case 2nd card is not pick after 5 seconds*/
                //printf("in case -1 x:%d   y:%d\n", x, y);
                paint_card(resp.play1[0], resp.play1[1], 255, 255, 255);
                write_card(resp.play1[0], resp.play1[1], resp.str_play1, 255, 255, 255);
            case -4:
                wait=1;
                read(sfd, &check, sizeof(check));
                wait=0;
        }
    }
    //return 0;
    pthread_exit(NULL);
}




int main(int argc, char * argv[]){
    
    int dim, end=0, r, score;
    
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

    r=fillboard(sfd);

    pthread_t lst_thread;
    
    /*thread to receive messages from server*/
    /*passing arguments to the thread*/
    int *pass;
    pass=malloc(sizeof(int));
    *pass=sfd;
    //pthread_create(&lst_thread, NULL, listenserver, pass);
    
    while(!end){
        pthread_create(&lst_thread, NULL, listenserver, pass);
        while(!done){
            while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_QUIT: {
                        done=1;//SDL_TRUE;
                        end=1;//SDL_TRUE;
                        printf("Leaving the game!\n");
                        break;
                    }
                    case SDL_MOUSEBUTTONDOWN:{
                        /*gets the x and y from button and sends to server*/
                        int board_x, board_y;
                        get_board_card(event.button.x, event.button.y, &board_x, &board_y);
                        if(wait!=1)write(sfd, &board_x, sizeof(board_x));
                        if(wait!=1)write(sfd, &board_y, sizeof(board_y));
                    }
                }
            } 
        }
        if(end!=1){
            read(sfd, &score, sizeof(score));
            if(score!=0) printf("WINNER SCORE: %d\n", score);
            else printf("GAME OVER!\n");
            done=0;
            close_board_windows();
            sleep(10);
            create_board_window(300, 300, dim);
        }
    }
    
    printf("fim\n");
    close_board_windows();
    close(sfd);
    free(pass);
    
}