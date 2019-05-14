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

	struct sockaddr_in server_addr;

	if(argc<2){
		printf("Incorrect arguments!\n");
		exit(-1);
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if(fd==-1){
		perror("socket: ");
		exit(-1);
	}

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(PORT);
	inet_aton(argv[1], &server_addr.sin_addr);

	if(connect(fd, (const struct sockaddr *) &server_addr, sizeof(server_addr))==-1){
		printf("Error connecting!\n");
		exit(-1);
	}
	printf("Connected!\n");

	while(1){
		
		
		
	}

	//


}