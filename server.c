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

	struct sockaddr_in local_addr;
	int *players;

	if(argc<2){
		printf("Incorrect arguments!\n");
		exit(-1);
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if(fd==-1){
		perror("socket: ");
		exit(-1);
	}

	local_addr.sin_family=AF_INET;
	local_addr.sin_port=htons(PORT);
	local_addr.sin_addr.s_addr=INADDR_ANY;

	if(bind(fd, (struct sockaddr *)&local_addr, sizeof(local_addr))==-1){
		perror("bind: ");
		exit(-1);
	}

	listen(fd, 5);

	while(1){

		players[0]=accept(fd, NULL, NULL);
		printf("Player connected!\n");

		

	}

	//connect(fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));


}