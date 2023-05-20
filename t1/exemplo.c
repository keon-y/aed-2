#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <math.h>
#include <SDL/SDL.h>
#include <stdbool.h>

//cosseno e seno de 45 graus
#define OFFSET sqrt(2)/2

void drawCircle(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int radius);
void drawSquare(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int size);
void squareInCenter(int x, int y, int size);
void clearScreen(short backButton);
void create_shape_choice();
void create_recursive_choice();
void create_circle_screen(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int radius);
void create_square_screen(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int size);
int read_shape_choice(int x, int y);
int read_recursive_choice(int x, int y);
int read_return_button(int x, int y);

int main()
{
	const int WINDOW_HEIGHT = 600;
	const int WINDOW_WIDTH = 600;
	const int RADIUS = 256;
	short escolha, rec;
	SDL_Event event;
	bool exit = false;
	int x, y;
	int screen = 0, last_screen = -1;
	bool mousedown = false;
	gfx_init(WINDOW_WIDTH, WINDOW_HEIGHT, "");
	gfx_set_font_size(40);


	while (!exit){
		while(SDL_PollEvent(&event) >= 0){

			if (event.type == SDL_QUIT) { 
				exit = true;
				break;
			}
			if (last_screen != screen) {
				if (screen == 0)
					create_shape_choice();
				else if (screen == 1)
					create_recursive_choice();
				else if (screen == 2) {
					if (escolha == 1)
						create_circle_screen(0, rec, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, RADIUS);
					else create_square_screen(0, rec, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 200);
				}
				last_screen = screen;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
				mousedown = true;
			
			if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && mousedown){
				SDL_GetMouseState(&x, &y);
				if (screen == 0) {
					escolha = read_shape_choice(x, y);
					if (escolha) screen++;
				}
				else if (screen == 1) {
					rec = read_recursive_choice(x, y);
					if (rec == -1)
						screen--;
					else if (rec != -2)
						screen++;
				}
				else if (screen == 2) {
					if (read_return_button(x, y)){
						screen--;
					}
				}
				mousedown = false;
			}
		}
	}
	gfx_quit();
	return 0;
}

//Função recursiva para desenhar círculos, os parâmetros são unsigned pois se houver um valor negativo, o código está errado
void drawCircle(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int radius) {
	gfx_ellipse(x, y, radius, radius);
	if (iteration < limit) {
		//quatro círculos, com o centro x +- raio/2 e y +- raio/2
		//OFFSET é definido como sqrt(2)/2 (seno e cosseno de 45)
		int radiusDif = (radius - radius/2);
		drawCircle(iteration+1, limit, x + radiusDif * OFFSET, y + radiusDif * OFFSET, radius/2);
		drawCircle(iteration+1, limit, x + radiusDif * OFFSET, y - radiusDif * OFFSET, radius/2);
		drawCircle(iteration+1, limit, x - radiusDif * OFFSET, y + radiusDif * OFFSET, radius/2);
		drawCircle(iteration+1, limit, x - radiusDif * OFFSET, y - radiusDif * OFFSET, radius/2);
	}
	
}
//Função recursiva para desenhar quadrados
void drawSquare(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int size){
	if (iteration < limit) {
		drawSquare(iteration + 1, limit, x , y + size/2 + size/4, size/2); //quadrado de baixo
		drawSquare(iteration + 1, limit, x , y - size/2 - size/4, size/2); //quadrado de cima
		drawSquare(iteration + 1, limit, x + size/2 + size/4, y, size/2);  //quadrado da direita
		drawSquare(iteration + 1, limit, x - size/2 - size/4, y, size/2);  //quadrado da esquerda
	}

	// cria o quadrado preto
	gfx_set_color(0,0,0);
	gfx_filled_rectangle(x - size/2, y - size/2, x + size/2, y + size/2);
	

	//cria o contorno branco
	gfx_set_color(255,255,255);
	gfx_rectangle(x - size/2, y - size/2, x + size/2, y + size/2);
	
}

//criar um contorno de quadrado no centro x, y
void squareInCenter(int x, int y, int size){
	gfx_rectangle(x - size/2, y - size/2, x + size/2, y + size/2);
}

//limpa a tela e adicionar um botão de retorno
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

//cria a tela de escolha de formato (quadrado ou circulo)
void create_shape_choice(){
	clearScreen(0);
	gfx_ellipse(150, 250, 70, 70);
	gfx_text(90, 320, "Circulo");

	squareInCenter(450, 245, 135);
	gfx_text(360, 320, "Quadrado");

	gfx_paint();

}

//cria a tela de escolha de recursões (0 a 10);
void create_recursive_choice(){
	clearScreen(1);

	gfx_text(220, 120, "Recursoes");
	//botão 0 recursões
	squareInCenter(310, 240, 70);
	gfx_text(300, 210, "0");


	//botoes de 1 a 10
	for (int i = 1, x = 130, y = 330; i <= 10; i++, x += 90){
		//contorno
		squareInCenter(x, y, 70);

		//produzir o texto com o numero (converter int pra string)
		int lenght = snprintf(NULL, 0, "%d", i);
		char *str = malloc(lenght + 1);
		snprintf( str, lenght + 1, "%d", i);

		//posicionamento do texto (o numero 10 recebe um tratamento diferente pois é maior)
		int textX = i == 10 ? x - 20 : x - 10;
		gfx_text(textX, y - 30, str);

		//reiniciar X e aumentar Y para fazer a segunda linha
		if (i == 5) {
			y += 90; 
			x = 40;
		}
		free(str);
	}
	
	gfx_paint();
}

//função que limpa a tela e chama a função recursiva de circulos
void create_circle_screen(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int radius){
	clearScreen(1);
	drawCircle(iteration, limit, x, y, radius);
	gfx_paint();
}

//função que limpa a tela e chama a função recursiva de quadrados
void create_square_screen(unsigned short iteration, unsigned short limit, unsigned int x, unsigned int y, unsigned int size){
	clearScreen(1);
	drawSquare(iteration, limit, x, y, size);
	gfx_paint();
}

//retorna a escolha (1 para círculo e 2 para quadrado), em caso de clique avulso retorna 0
int read_shape_choice(int x, int y){
	if (x >= 50 && x <= 250 && y >= 200 && y <= 400)
		return 1;		
	if (x >= 350 && x <= 550 && y >= 200 && y <= 400)
		return 2;
	return 0;
}

//retorna a quantidade de recursões (0 - 10), caso retorne -1 o usuário clicou no botão de voltar, clique avulso retorna -2
int read_recursive_choice(int x, int y){
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
	if (read_return_button(x, y))
		return -1;
	
	return -2;
}

int read_return_button(int x, int y) {
	if (x >= 10 && x <= 100 && y >= 10 && y <= 40)
		return 1;
	return 0;
}