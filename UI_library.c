#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int screen_width;
int screen_height;
int n_ronw_cols;
	int row_height;
		int col_width;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;


/*receives the cards coordinates and prints there the text
also receives rgb info*/
void write_card(int  board_x, int board_y, char * text, int r, int g, int b){
	SDL_Rect rect;

	rect.x = board_x * col_width;
	rect.y = board_y * row_height;
	rect.w = col_width+1;
	rect.h = row_height+1;


	TTF_Font * font = TTF_OpenFont("arial.ttf", row_height);

	int text_x = board_x * col_width;
	int text_y = board_y * row_height;

	SDL_Color color = { r, g, b };
 	SDL_Surface * surface = TTF_RenderText_Solid(font, text, color);

	SDL_Texture* Background_Tx = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface); /* we got the texture now -> free surface */


	SDL_RenderCopy(renderer, Background_Tx, NULL, &rect);
	SDL_RenderPresent(renderer);


}

/*receives cards coordinates and paints it
also receives rgb info*/
void paint_card(int  board_x, int board_y , int r, int g, int b){
	SDL_Rect rect;

	rect.x = board_x * col_width;
	rect.y = board_y * row_height;
	rect.w = col_width+1;
	rect.h = row_height+1;
	SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &rect);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(renderer, &rect);

	SDL_RenderPresent(renderer);


}

/*receives cards coordinates and clears it
paints it white*/
void clear_card(int  board_x, int board_y){
	paint_card(board_x, board_y , 255, 255, 255);

}

/*receives as arguments the mouse click coordinates
returns corresponding board position*/
void get_board_card(int mouse_x, int mouse_y, int * board_x, int *board_y){
	*board_x = mouse_x / col_width;
	*board_y = mouse_y / row_height;
}


/*creates a window with width*height pixels
draws board with cards faced down*/
int create_board_window(int width, int height,  int dim){

	screen_width = width;
	screen_height = height;
	n_ronw_cols = dim;
	row_height = height /n_ronw_cols;
	col_width = width /n_ronw_cols;
screen_width = n_ronw_cols * col_width +1;
screen_height = n_ronw_cols *row_height +1;

	if (SDL_CreateWindowAndRenderer(screen_width, screen_height, 0, &window, &renderer)  != 0) {
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		exit(-1);
	}


	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);


	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	for (int i = 0; i <n_ronw_cols+1; i++){
		SDL_RenderDrawLine(renderer, 0, i*row_height, screen_width, i*row_height);
	}

	for (int i = 0; i <n_ronw_cols+1; i++){
		SDL_RenderDrawLine(renderer, i*col_width, 0, i*col_width, screen_height);
	}
	SDL_RenderPresent(renderer);
	return(0);
}

/*destroys the window used to represent the board*/
void close_board_windows(){
	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}
	if (window) {
		SDL_DestroyWindow(window);
	}
	return;
}
