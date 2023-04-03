#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <unistd.h> /* sleep  */
#include <math.h>
#include <SDL/SDL.h>
//cosseno e seno de 45 graus
//3 1  4 -3  1 3
//7 2   9 -7 
#define OFFSET sqrt(2)/2

void drawCircle(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int radius);
void drawSquare(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int size);
void squareInCenter(int x, int y, int size);
void clearScreen(short backButton);
short create_shape_choice();
short create_recursive_choice();
short create_circle_screen(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int radius);
short create_square_screen(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int size);

int main()
{
	const int WINDOW_HEIGHT = 600;
	const int WINDOW_WIDTH = 600;
	const int RADIUS = 256;
	short escolha, rec;
	gfx_init(WINDOW_WIDTH, WINDOW_HEIGHT, "");
	
	clearScreen(0);
	gfx_set_font_size(40);
	while (1){
		escolha = create_shape_choice();
		clearScreen(1);
		while (1) {
			rec = create_recursive_choice();
			clearScreen(1);

			if (rec == -1) {//voltarnm
				clearScreen(0);
				break;
			}
			
			short voltar = 0;

			if (escolha == 1)
				voltar = create_circle_screen(0, rec, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, RADIUS);
			else if (escolha == 2)
				voltar = create_square_screen(0, rec, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 286);
			if (voltar) {
				clearScreen(1);
				continue;
			}
				
			
		}
		
	}

	return 0;
}

void drawCircle(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int radius) {
	gfx_ellipse(x, y, radius, radius);
	if (iteration < limit) {
		//quatro círculos, com o centro x +- raio/2 e y +- raio/2
		int radiusDif = (radius - radius/2);
		drawCircle(iteration+1, limit, x + radiusDif * OFFSET, y + radiusDif * OFFSET, radius/2);
		drawCircle(iteration+1, limit, x + radiusDif * OFFSET, y - radiusDif * OFFSET, radius/2);
		drawCircle(iteration+1, limit, x - radiusDif * OFFSET, y + radiusDif * OFFSET, radius/2);
		drawCircle(iteration+1, limit, x - radiusDif * OFFSET, y - radiusDif * OFFSET, radius/2);
	}
	
}

void drawSquare(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int size){
	
	if (iteration < limit) {
		drawSquare(iteration + 1, limit, x , y + size/2 + size/4, size/2);
		drawSquare(iteration + 1, limit, x , y - size/2 - size/4, size/2);
		drawSquare(iteration + 1, limit, x + size/2 + size/4, y, size/2);
		drawSquare(iteration + 1, limit, x - size/2 - size/4, y, size/2);
	}
	// cria o retângulo preto
	gfx_set_color(0,0,0);
	gfx_filled_rectangle(x - size/2, y - size/2, x + size/2, y + size/2);
	

	//cria o contorno branco
	gfx_set_color(255,255,255);
	gfx_rectangle(x - size/2, y - size/2, x + size/2, y + size/2);
	
}
void squareInCenter(int x, int y, int size){
	gfx_rectangle(x - size/2, y - size/2, x + size/2, y + size/2);

}
//parâmetro: 0 ou 1 (sem botão de voltar ou com botão)
void clearScreen(short backButton){
	gfx_clear();
	if (backButton){
		gfx_rectangle(10, 10, 100, 40);
		unsigned short oldSize = gfx_get_font_size();
		gfx_set_font_size(20);
		gfx_text(30, 15, "Voltar");
		gfx_paint();
		gfx_set_font_size(oldSize);
	}
}

short create_shape_choice(){
	SDL_Event event;
	gfx_ellipse(150, 250, 70, 70);
	gfx_text(90, 320, "Circulo");

	squareInCenter(450, 245, 135);
	gfx_text(360, 320, "Quadrado");

	gfx_paint();

	while (1){
		if (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT)
				gfx_quit();
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                int x, y;
                SDL_GetMouseState( &x, &y );
				if (x >= 50 && x <= 250 && y >= 200 && y <= 400)
					return 1;
				
				if (x >= 350 && x <= 550 && y >= 200 && y <= 400)
					return 2;
				
            }
		}	
	}
}


short create_recursive_choice(){

	SDL_Event event;
	squareInCenter(310, 240, 70);
	gfx_text(300, 210, "0");
	gfx_text(220, 120, "Recursoes");

	for (int i = 1, x = 130, y = 330; i <= 10; i++, x += 90){
		squareInCenter(x, y, 70);
		//produzir o texto com o numero (converter int pra string)
		int lenght = snprintf(NULL, 0, "%d", i);
		char *str = malloc(lenght + 1);
		snprintf( str, lenght + 1, "%d", i);
		//ajustar o posicionamento do número 10
		int textX = i == 10 ? x - 20 : x - 10;
		gfx_text(textX, y - 30, str);
		//reiniciar o valor de x na segunda linha
		if (i == 5) {
			y += 90; 
			x = 40;
		}
		free(str);
	}
	
	gfx_paint();
	while(1){
		if (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT)
				gfx_quit();
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
				//botao 0
				if (x >= 275 && x <= 345 && y >= 205 && y <= 275)
					return 0;
				//botao 1 ao 10
				//na segunda linha x0 precisa reiniciar e y0 precisa aumentar, portanto x0 = 1 e y0 = 90
				for (int i = 1, x0 = 1, y0 = 0; i <= 10; i++, x0 = i == 6 ? 1 : x0 + 1, y0 = i >= 6 ? 90 : 0){
					if (x >= 5 + 90 * x0 && x <= 75 + 90 * x0 && y >= 295 + y0 && y <= 365 + y0)
						return i;
				}
				//botao de voltar
				if (x >= 10 && x <= 100 && y >= 10 && y <= 40)
					return -1;
            }	
		}
	}
}

short create_circle_screen(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int radius){
	SDL_Event event;
	drawCircle(iteration, limit, x, y, radius);
	gfx_paint();
	while (1){
		if (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT)
				gfx_quit();
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
				
				//voltar
				if (mouse_x >= 10 && mouse_x <= 100 && mouse_y >= 10 && mouse_y <= 40){
					return 1;
				}
            }
		}	
	}
	return 0;
}
short create_square_screen(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int size){
	SDL_Event event;
	drawSquare(iteration, limit, x, y, size);
	gfx_paint();
	while (1){
		if (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT) 
				gfx_quit();
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
				//voltar
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
				if (mouse_x >= 10 && mouse_x <= 100 && mouse_y >= 10 && mouse_y <= 40){
					return 1;
				}
            }
		}	
	}
	return 0;
}