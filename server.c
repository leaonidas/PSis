#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "board_library.h"
#include "UI_library.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 3000


int main(int argc, char * argv[]){

	/*select*/
	int=maxfd;
	fd_set rfds;

	/*guardar fd de players(depois lista quiçá)*/	
	int *players;

	/*verificação argumentos de entrada para dim*/
	if(argc<2){
		printf("Incorrect arguments!\n");
		exit(-1);
	}
	int dim=argv[1];


	/*criar socket de listen*/
	struct sockaddr_in local_addr;
	int lst_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd==-1){
		perror("socket: ");
		exit(-1);
	}
	local_addr.sin_family=AF_INET;
	local_addr.sin_port=htons(PORT);
	local_addr.sin_addr.s_addr=INADDR_ANY;

	/*bind*/
	if(bind(lst_fd, (struct sockaddr *)&local_addr, sizeof(local_addr))==-1){
		perror("bind: ");
		exit(-1);
	}
	/*listen*/
	listen(lst_fd, 5);

	while(1){

		FD_SET(&

			)

		players[0]=accept(lst_fd, NULL, NULL);
		printf("Player connected!\n");

	}

	//connect(fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));


}