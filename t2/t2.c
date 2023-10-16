#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <math.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1024
#define HEIGHT 768

typedef struct Node {
	int address; 
	int size; //b
	struct Node *next;
} NodeAvailable;

typedef struct DoubleNode {
	int id;
	int address_start;
	int address_end;
	int rgb[3]; //desenhar na tela
	struct DoubleNode *next;
	struct DoubleNode *previous;
} NodeAllocated;

//encadeamento simples
NodeAvailable *createSingleNode(int, int);
NodeAvailable *searchSingleList(int, NodeAvailable *);
void insertSingleList(NodeAvailable *, NodeAvailable **);
int removeSingleList(int, NodeAvailable **, NodeAllocated **, int);

//encadeamento duplo
NodeAllocated *createDoubleNode(int, int);
void insertDoublyList(NodeAllocated *, NodeAllocated **);
int removeDoublyList (int, NodeAllocated **, NodeAvailable **);

//interface grafica
void drawMainScreen();
int getPercentAllocated(int, NodeAllocated *);
void drawScreen(int, int*, int, NodeAllocated *, NodeAvailable *);
void drawAllocatedScreen(NodeAllocated *);
void drawArrowTo(int, int, int, int, int, int);
void drawGeneralMap(int, NodeAllocated *, NodeAvailable *);
void drawAvailableScreen(NodeAvailable *);

void pause();


int main()
{
	// inicializacao da Memoria
	srand(time(NULL));
	int memory_size;
	int screen = 0;
	int last_screen = -1;
	do {
		printf("Informe M: ");
		scanf("%d", &memory_size);
	} while (memory_size <= 0);
	
	gfx_init(WIDTH, HEIGHT, "");
	
	NodeAvailable *start_available = createSingleNode(0, memory_size);
	NodeAllocated *start_allocated = NULL;
	bool exit = false;
	int escolha;
	int success;
	
	system("clear");

	while (!exit) {
		if (last_screen != screen) 
			drawScreen(screen, &last_screen, memory_size, start_allocated, start_available);

		printf("1. Alocar\n2. Desalocar\n3. Tela inicial\n4. Ver disponiveis\n5. Ver mapa geral\n6. Ver alocados \n7. Sair\n--> ");
		scanf("%d", &escolha);

		//nao da pra usar um switch case pq tem o break do final
		if (escolha == 1) {
			int size;
			printf("Tamanho da alocacao: ");
			scanf("%d", &size);
			success = removeSingleList(size, &start_available, &start_allocated, 1);
			if(!success) {
				printf("\n[ERRO!] Nao foram encontrados blocos para alocar %d bytes!\n\n", size);
				pause();
			}
			last_screen = -1; //forcar um update
		}
		else if (escolha == 2) {
			int id;
			printf("ID da memoria: ");
			scanf("%d", &id);
			success = removeDoublyList(id, &start_allocated, &start_available);
			if (!success) {
				printf("\n[ERRO!] ID %d nao foi encontrado!\n\n", id);
				pause();
			}
			last_screen = -1;
		}
		else if (escolha == 3) //tela inicial
			screen = 0;
		
		else if (escolha == 4)  //tela disponiveis
			screen = 1;
			
		else if (escolha == 5) //mapa geral
			screen = 2;
				
		else if (escolha == 6) //tela alocados
			screen = 3;
			
		else if (escolha == 7){ //sair
			exit = true;
			break;
		}
		else {
			printf("\n[ERRO!] Escolha inexistente!\n\n");
			pause();
		}
		system("clear");
		gfx_clear();
	}

	//desalocar
	while (start_available) {
		NodeAvailable *ptAux = start_available;
		start_available = start_available->next;
		free(ptAux);
	}
	while (start_allocated) {
		NodeAllocated *ptAux = start_allocated;
		start_allocated = start_allocated->next;
		free(ptAux);
	}


	gfx_quit();
	return 0;
}

//lista simplesmente encadeada
NodeAvailable *createSingleNode(int address, int size) {
	NodeAvailable *node = (NodeAvailable*)malloc(sizeof(NodeAvailable));
	node->address = address;
	node->size = size;
	node->next = NULL;
	return node;
}

//insere um NODE em uma lista simplesmente encadeada ordenada de forma crescente
void insertSingleList(NodeAvailable *node, NodeAvailable **start) { //ponteiro para ponteiro em caso de precisar alterar o comeco da lista
	NodeAvailable *ptr = *start;

	if (!ptr) {
		*start = node;
		return;
	}

	while (ptr) { //checar se tem alguma juncao
		if (ptr->address + ptr->size == node->address || node->address + node->size == ptr->address) {
			node->address = ptr->address + ptr->size == node->address ? ptr->address : node->address;
			node->size = node->size + ptr->size;
			NodeAvailable *aux = ptr;
			ptr = ptr->next;
			removeSingleList(aux->size, start, NULL, 0);
			continue;
		}
		ptr = ptr->next;
	}
	ptr = *start;
	if (!ptr) { //se juntou com um bloco unico, portanto a lista tornou-se vazia
		*start = node;
		return;
	}
	if (ptr->size >= node->size) { //primeiro elemento eh maior que o no a ser inserido
		node->next = *start;
		*start = node;
		return;
	}
	while (ptr->next != NULL && ptr->next->size < node->size) 
		ptr = ptr->next;
	
	
	if (!ptr->next) {//ultimo elemento da lista
		ptr->next = node;
		return;
	}



	node->next = ptr->next;
	
	ptr->next = node;

}

int removeSingleList(int size, NodeAvailable **start, NodeAllocated **start_allocated, int type) {

	NodeAvailable *ptr = *start, *prev = NULL;
	if (!ptr) return 0;
	while (ptr && size > ptr->size) {
		prev = ptr;
		ptr = ptr->next;
	}

	if (!ptr) //impossivel alocar (percorreu a lista toda)
		return 0;

	if (prev) //se nao tiver prev ptr vai ser o primeiro elemento, portanto muda o start
		prev->next = ptr->next;
	else *start = ptr->next;

	if (type) {
		if (ptr->size != size) //cria um novo bloco caso o tamanho da memoria nao seja exatamente igual
			insertSingleList(createSingleNode(ptr->address + size, ptr->size - size), start);
		insertDoublyList(createDoubleNode(ptr->address, ptr->address + size - 1), start_allocated);
	}
	free(ptr); //libera a memoria
	return 1;
}

//criar
NodeAllocated *createDoubleNode(int start, int end){
	static int id = 0; //id unico que apenas aumenta a cada nova alocacao
	NodeAllocated *node = malloc(sizeof(NodeAllocated));
	node->id = id;
	node->address_start = start;
	node->address_end = end;
	for(int i = 0; i < 3; i++) {
		node->rgb[i] = 40 + (rand() %190);
	}
	node->next = NULL;
	node->previous = NULL;
	id++;
	return node;
}

//adicionar
void insertDoublyList(NodeAllocated *node, NodeAllocated **head) {
	if (!(*head)) { //lista vazia
		*head = node;
		return;
	}

	NodeAllocated *ptr = *head;
	if (ptr->id > node->id) { //inserir no comeco caso o primeiro elemento seja maior que node
		ptr->previous = node;
		node->next = ptr;
		*head = node;
		return;
	}

	while (ptr->next != NULL && !((ptr->next->id > node->id))) 
		ptr = ptr->next;
	
	NodeAllocated *aux = ptr->next;
	ptr->next = node;
	node->previous = ptr;
	node->next = aux;
	if (aux) //caso for o ultimo elemento, aux nao vai existir e acessara um endereco invalido
		aux->previous = node;

}

int removeDoublyList (int id, NodeAllocated **head, NodeAvailable **available_start) {
	if (!(*head)) return 0; //impossivel remover de lista vazia
	
	NodeAllocated *ptr = *head;
	if (ptr->id == id) { //se o no a ser removido eh o primeiro
		if (ptr->next) //se for elemento unico da erro
			ptr->next->previous = NULL;
		*head = ptr->next;

		//voltar para a lista de memoria desalocada
		insertSingleList(
			createSingleNode(ptr->address_start, ptr->address_end - ptr->address_start + 1),
			available_start);

		free(ptr);
		return 1;
	}

	while (ptr != NULL && ptr->id != id) 
		ptr = ptr->next;

	if(ptr && ptr->id == id) {
		ptr->previous->next = ptr->next;
		if (ptr->next) //o primeiro elemento sempre eh checado, falta checar o ultimo
			ptr->next->previous = ptr->previous;

		insertSingleList(
			createSingleNode(ptr->address_start, ptr->address_end - ptr->address_start + 1),
			available_start);

		free(ptr);
		return 1;
	}
	return 0;
}

//interface grafica

//dado uma lista duplamente encadeada, pegar o tamanho dos blocos alocados e dividir pela memoria total para conseguir a porcentagem de memoria alocada
int getPercentAllocated(int m, NodeAllocated *head) {
	NodeAllocated *ptr = head;
	int sum = 0;
	while (ptr) {
		sum += (ptr->address_end - ptr->address_start) + 1;
		ptr = ptr->next;
	}
	return sum/(m*1.0)*100;
}

//left_to_left: se a flecha vai sair da esquerda e apontar para a esquerda, como em casos de apontar para a linha de baixo ou de cima
//left: se a flecha aponta para a esquerda
void drawArrowTo(int x_0, int y_0, int x, int y, int left, int bottom) {
	if (left){ //seta (apenas as duas diagonais) apontando pra esquerda
		gfx_line(x, y, x + 10, y - 8);
		gfx_line(x, y, x + 10, y + 8);
	}
	else{ //seta apontando pra direita
		gfx_line(x, y, x - 10, y - 8);
		gfx_line(x, y, x - 10, y + 8);
	}

	if (y_0 == y) //reta
		gfx_line(x_0, y_0, x, y);
	else if (left && bottom) { //vai vir de baixo e apontar para a esquerda
		gfx_line(x_0, y_0, x + 30, y_0);
		gfx_line(x+30, y_0, x + 30, y);
		gfx_line(x+30, y, x, y);
	}
	else if (left && !bottom) {//vem de cima e aponta pra esquerda
		gfx_line(x_0, y_0, x_0 + 30, y_0);
		gfx_line(x_0 + 30, y_0, x_0 + 30, y);
		gfx_line(x_0+30, y, x, y);
	}
	else if (!left && bottom) { //vai vir da direita e acima e vai apontar pra baixo e direita
		gfx_line(x_0, y_0, x - 30, y_0);
		gfx_line(x-30, y_0, x - 30, y);
		gfx_line(x-30, y, x, y);
	}
	else if (!left && !bottom) {//vem de cima e aponta pra direita
		gfx_line(x_0, y_0, x_0 - 30, y_0);
		gfx_line(x_0 - 30, y_0, x_0 - 30, y);
		gfx_line(x_0-30, y, x, y);
	}
}

//mapa geral da memoria
void drawGeneralMap(int memory_size, NodeAllocated *start_allocated, NodeAvailable *start_available){
	gfx_set_color(112, 128, 144);
	int memory_width = 1000;
	int memory_height = 200;
	int memory_left_x = WIDTH/2 - memory_width/2;
	float pixels_per_address = 1000 / ((memory_size-1)*1.0); //pixels por unidade de endereco (bytes)
	int shadow_colors[3]; //rgb da sombra
	int text_width, text_height, font_size;
	gfx_filled_rectangle(memory_left_x, HEIGHT/2 - memory_height/2, memory_left_x + memory_width, HEIGHT/2 + memory_height/2);
	NodeAllocated *ptr = start_allocated;
	

	ptr = start_allocated;
	while (ptr) {
		//gradiente das cores
		for (int i = 0; i < 3; i++)
			shadow_colors[i] = ptr->rgb[i] - 30;
		//sombra
		gfx_set_color(shadow_colors[0], shadow_colors[1], shadow_colors[2]);	
		gfx_filled_rectangle(memory_left_x + (ptr->address_start -1) * pixels_per_address,
		HEIGHT/2 - memory_height/2,
		memory_left_x + (ptr->address_end * pixels_per_address),
		HEIGHT/2 + memory_height/2);

		//superior (parte mais clara)
		gfx_set_color(ptr->rgb[0] , ptr->rgb[1], ptr->rgb[2]);
		gfx_filled_rectangle(memory_left_x + ((ptr->address_start -1) * pixels_per_address),
		HEIGHT/2 - memory_height/2 - 60,
		memory_left_x + (ptr->address_end * pixels_per_address),
		HEIGHT/2 + memory_height/2 - 60);

		//texto ID centralizado no meio do bloco
		gfx_set_color(255,255, 255);
		int id_len = snprintf(NULL, 0, "%d", ptr->id);
		char id[id_len + 1];
		sprintf(id, "%d", ptr->id);

		//calcular o tamanho do bloco em pixels
		int block_size = (memory_left_x + (ptr->address_end * pixels_per_address)) - (memory_left_x + ((ptr->address_start -1) * pixels_per_address));
		
		font_size = block_size < 11 ? 10 - id_len : 10;
		gfx_set_font_size(font_size);
		gfx_get_text_size(id, &text_width, &text_height);
		
		gfx_text(memory_left_x + ((ptr->address_start -1) * pixels_per_address) + block_size/2 - text_width/2, 
		HEIGHT/2 - memory_height/2 - 10, id);

		//endereco inicial e endereco final
		int len_start = snprintf(NULL, 0, "%d", ptr->address_start);
		int len_end = snprintf(NULL, 0, "%d", ptr->address_end);
		char address_start[len_start+1];
		char address_end[len_end+1];
		
		sprintf(address_start, "%d", ptr->address_start);
		sprintf(address_end, "%d", ptr->address_end);

		
		
		if (ptr->address_start != ptr->address_end) {//so fazer o texto de endereco final se os dois forem diferentes (nao for 1 byte)
			font_size = block_size <= 20 ? 9 - len_start : 9;
			gfx_set_font_size(font_size);
			gfx_get_text_size(address_end, &text_width, &text_height);
			gfx_text(memory_left_x + ((ptr->address_start - 1) * pixels_per_address) + len_start, HEIGHT/2 - memory_height/2 - 70, address_start);
			gfx_text(memory_left_x + (ptr->address_end * pixels_per_address) - text_width - len_end, HEIGHT/2 - memory_height/2 - 70, address_end);
		}
		else {
			font_size = block_size < 11 ? 9 - len_start : 9;
			gfx_set_font_size(font_size);
			gfx_get_text_size(address_start, &text_width, &text_height);
			gfx_text(memory_left_x + ((ptr->address_start -1) * pixels_per_address) + block_size/2 - text_width/2, HEIGHT/2 - memory_height/2 - 70, address_start);
		}
		ptr = ptr->next;
	}

	//fazer agora os blocos livres
	NodeAvailable *ptr_disp = start_available;
	while (ptr_disp){
		gfx_set_color(255, 255, 255);
		gfx_set_font_size(9);
		int len_start = snprintf(NULL, 0, "%d", ptr_disp->address);
		int len_end = snprintf(NULL, 0, "%d", ptr_disp->address + ptr_disp->size - 1);
		char address_start[len_start+1];
		char address_end[len_end+1];
		sprintf(address_start, "%d", ptr_disp->address);
		sprintf(address_end, "%d", ptr_disp->address + ptr_disp->size - 1);

		int tam_len = snprintf(NULL, 0, "%d", ptr_disp->size);
		char tam[tam_len + 1];
		sprintf(tam, "%d", ptr_disp->size);
		gfx_set_font_size(10);
		//calcular comprimento do bloco disponivel pra colocar um texto no inicio e no final
		int block_size = (memory_left_x + ((ptr_disp->address + ptr_disp->size - 1) * pixels_per_address)) - (memory_left_x + ((ptr_disp->address - 1) * pixels_per_address));
		gfx_text( ((ptr_disp->address - 1) * pixels_per_address) + block_size/2 + memory_left_x,  HEIGHT/2 - memory_height/2 - 30, tam);

		if (ptr_disp->size != 1) { //so colocar 2 textos se o bloco tiver tamanho maior que 1
			gfx_text(memory_left_x + ((ptr_disp->address - 1) * pixels_per_address)+ (2*len_start), HEIGHT/2 + memory_height/2 + 10, address_start);
			gfx_text(memory_left_x + ((ptr_disp->address + ptr_disp->size - 1) * pixels_per_address) - (5*len_end), HEIGHT/2 + memory_height/2 + 10, address_end);
		} else gfx_text(memory_left_x + ((ptr_disp->address - 1) * pixels_per_address) - (len_start) + 3, HEIGHT/2 + memory_height/2 + 10, address_start);
		ptr_disp = ptr_disp->next;
	}

	//infografico (para como ler as informacoes na tela)
	//memoria vazia
	gfx_set_color(112, 128, 144);
	gfx_filled_rectangle(100, HEIGHT-200, 200, HEIGHT-110);
	gfx_set_font_size(10);
	gfx_set_color(255,255,255);
	gfx_text(80, HEIGHT-100, "End. inicial");
	gfx_text(180, HEIGHT-100, "End. final");
	gfx_text(125, HEIGHT-220, "Tamanho");

	//memoria alocada
	gfx_set_color(110, 110, 34);
	gfx_filled_rectangle(WIDTH-100, HEIGHT-200, WIDTH-200, HEIGHT-80); //sombra
	gfx_set_color(141, 144, 62);
	gfx_filled_rectangle(WIDTH-100, HEIGHT-200, WIDTH-200, HEIGHT-110);
	gfx_set_color(255,255,255);
	gfx_text(WIDTH-230, HEIGHT-220, "End. inicial");
	gfx_text(WIDTH-130, HEIGHT-220, "End. final");
	gfx_text(WIDTH-150, HEIGHT-160, "ID");

	gfx_paint();
}

void drawAvailableScreen(NodeAvailable *start_available){
	NodeAvailable *ptr = start_available;
	gfx_set_color(255, 255, 255);
	gfx_set_font_size(13);
	int counter = 0;
	int total = 0; //daria pra fazer uma variavel estatica
	int distanceNodes = 50; //cada no ficara a 50px de distancia um do outro
	int nodeWidth = 140; 
	int nodeHeight = 80; //altura fixa
	int x; //centro do bloco
	int y = HEIGHT/2;
	int max_per_line = WIDTH/(nodeWidth + distanceNodes);
	int total_lines;
	int next_rect_width = nodeWidth/5;
	int old_x = 0;
	int old_y = 0;
	int next_rect_x;
	int text_width, text_height;
	while (ptr){
		total++; //calcular o total eh importante para saber a posicao ideal de cada elemento
		ptr = ptr->next;
	}
	total_lines = ((total-1)/max_per_line); //se for um numero quebrado eh arredondado pra baixo, 
	//portanto se total_lines * max_per_line vai ser menor do que um elemento SE esse elemento estiver em uma linha nao completa
	y = HEIGHT/2 - total_lines * (nodeHeight/2 + distanceNodes/2);
	
	/*
		Nas linhas pares (0, 2, ...) os elementos seguem em ordem esquerda para direita
		Nas linhas impares (1, 2, ..) os elementos seguem da direita para esquerda
		Portanto, em toda troca de linha (counter % max == 0) eh necessario recalcular o X inicial e incrementar Y para descer a linha
	*/
	
	ptr = start_available;
	while (ptr) {
		int isOddLine = (counter / max_per_line) % 2 == 1;
		if (counter % max_per_line == 0) {
			if (isOddLine)
				x = counter + 1< total_lines * max_per_line ? 
				WIDTH/2 + (distanceNodes/2 + nodeWidth/2) * (max_per_line - 1) : 
				WIDTH/2 + (distanceNodes/2 + nodeWidth/2) * (total - counter - 1);
			else 
				x = counter + 1 < total_lines * max_per_line ? 
				WIDTH/2 - (distanceNodes/2 + nodeWidth/2) * (max_per_line - 1) : 
				WIDTH/2 - (distanceNodes/2 + nodeWidth/2) * (total - counter - 1);
			if (counter) //na primeira iteracao nao fazer, apenas nas demais (counter > 0)
				y += nodeHeight + distanceNodes;
		}

		gfx_rectangle(x - nodeWidth/2, 
		y - nodeHeight/2, 
		x + nodeWidth/2, 
		y + nodeHeight/2);

		
		//retangulo menor de onde sai a flecha para o proximo
		int multiplier = isOddLine ? -1 : 1;
		next_rect_x = isOddLine ? x - nodeWidth/2: x + nodeWidth/2;
		gfx_rectangle(next_rect_x - (next_rect_width * multiplier),
		y - nodeHeight/2, 
		next_rect_x, 
		y + nodeHeight/2);
		
		//textos
		char endereco[30] = "End. inicial: ";
		char num_endereco[11];
		sprintf(num_endereco, "%d", ptr->address);
		strcat(endereco, num_endereco);
		char tamanho[20] = "Tamanho: ";
		char num_tamanho[11];
		sprintf(num_tamanho, "%d", ptr->size);
		strcat(tamanho, num_tamanho);
		

		int multiplicador = isOddLine ? 1 : -1;
		gfx_get_text_size(endereco, &text_width, &text_height);
		gfx_text(x + (multiplicador*next_rect_width/2) - text_width/2, y + nodeHeight/2 - 30, endereco);

		gfx_get_text_size(tamanho, &text_width, &text_height);
		gfx_text(x + (multiplicador*next_rect_width/2) - text_width/2, y - nodeHeight/2 + 20, tamanho);

		if (counter) {
			/*	se for um ultimo elemento:
					par: ligar x antigo + metade com x novo + metade
					impar: ligar x antigo - metade do tamanho com x antigo + metade
				se for uma linha par ligar o x antigo + metade do tamanho com x novo - metade do tamanho
				se for uma linha impar ligar o x antigo - metade do tamanho com x novo + metade do tamanho

			*/
			if (counter % max_per_line  == 0 && isOddLine)
				drawArrowTo(old_x + nodeWidth/2 - next_rect_width/2, old_y, x+ nodeWidth/2, y, isOddLine, 0);
			else if (counter % max_per_line == 0 && !isOddLine)
				drawArrowTo(old_x - nodeWidth/2 + next_rect_width/2, old_y, x - nodeWidth/2, y, isOddLine, 0);
			else if (isOddLine)
				drawArrowTo(old_x - nodeWidth/2 + next_rect_width/2, old_y, x + nodeWidth/2, y, isOddLine, 0);
			else
				drawArrowTo(old_x + nodeWidth/2 - next_rect_width/2, old_y, x - nodeWidth/2, y, isOddLine, 0);
		}
		old_x = x;
		old_y = y;

		x = isOddLine ? x - nodeWidth - distanceNodes : x + nodeWidth + distanceNodes;
		ptr = ptr->next;
		counter++;
	}
	gfx_paint();
}

void drawScreen(int screen, int* last_screen, int memory_size, NodeAllocated *start_allocated, NodeAvailable * start_available) {
	
	switch (screen) {
		case 0: //tela inicial
			drawMainScreen(getPercentAllocated(memory_size, start_allocated));
		break;
		case 1: //tela disponiveis
			drawAvailableScreen(start_available);
		break;
    case 2: //mapa geral
			drawGeneralMap(memory_size, start_allocated, start_available);
		break;
		case 3:
			drawAllocatedScreen(start_allocated);
		break;

	}
	*last_screen = screen;
}

void drawAllocatedScreen(NodeAllocated *head){
	NodeAllocated *ptr = head;
	if (!ptr) return;
	gfx_set_font_size(10);
	int counter = 0;
	int total = 0; //daria pra fazer uma variavel estatica
	int node_distance = 50; //cada no ficara a 50px de distancia um do outro
	int node_width = 140; 
	int node_height = 80; //altura fixa
	int x;
	int y = HEIGHT/2;
	int max_per_line = WIDTH/(node_width + node_distance);
	int total_lines;
	int next_rect_width = node_width/5;
	int old_x = 0;
	int old_y = 0;
	int text_width, text_height;
	
	while (ptr){
		total++; //calcular o total eh importante para saber a posicao ideal de cada elemento
		ptr = ptr->next;
	}

	total_lines = ((total-1)/max_per_line); //se for um numero quebrado eh arredondado pra baixo, 
	//portanto se total_lines * max_per_line vai ser menor do que um elemento SE esse elemento estiver em uma linha nao completa
	y = HEIGHT/2 - total_lines * (node_height/2 + node_distance/2);
	

	
	ptr = head;
	while (ptr) {
		int isOddLine = (counter / max_per_line) % 2 == 1;
		if (counter % max_per_line == 0) {
			if (isOddLine)
				x = counter + 1< total_lines * max_per_line ? 
				WIDTH/2 + (node_distance/2 + node_width/2) * (max_per_line - 1) : 
				WIDTH/2 + (node_distance/2 + node_width/2) * (total - counter - 1);
			else 
				x = counter + 1 < total_lines * max_per_line ? 
				WIDTH/2 - (node_distance/2 + node_width/2) * (max_per_line - 1) : 
				WIDTH/2 - (node_distance/2 + node_width/2) * (total - counter - 1);
			if (counter) //na primeira iteracao nao fazer
			y += node_height + node_distance;
		}
		gfx_set_color(ptr->rgb[0], ptr->rgb[1], ptr->rgb[2]);
		gfx_rectangle(x - node_width/2, 
		y - node_height/2, 
		x + node_width/2, 
		y + node_height/2);

		
		//retangulos menores de onde sai a flecha pro prox
		gfx_rectangle(x - node_width/2,
		y - node_height/2, 
		(x - node_width/2) + next_rect_width, 
		y + node_height/2);

		gfx_rectangle(x + node_width/2,
		y - node_height/2, 
		(x + node_width/2) - next_rect_width, 
		y + node_height/2);

		char endereco_inicial[30] = "End. inicio: ";
		char num_endereco[11];
		sprintf(num_endereco, "%d", ptr->address_start);
		strcat(endereco_inicial, num_endereco);

		char endereco_final[30] = "End. fim: ";
		char num_final[11];
		sprintf(num_final, "%d", ptr->address_end);
		strcat(endereco_final, num_final);
		
		char id[12] = "ID: ";
		char num_id[11];
		sprintf(num_id, "%d", ptr->id);
		strcat(id, num_id);

		gfx_set_color(255, 255, 255);
		gfx_get_text_size(endereco_inicial, &text_width, &text_height);
		gfx_text(x - text_width/2, y - 30, endereco_inicial);
		
		gfx_get_text_size(endereco_final, &text_width, &text_height);
		gfx_text(x - text_width/2, y - 7, endereco_final);

		gfx_get_text_size(id, &text_width, &text_height);
		gfx_text(x - text_width/2, y + 15, id);

		if (counter) {
			/*	se for um ultimo elemento:
					par: ligar x antigo + metade com x novo + metade
					impar: ligar x antigo - metade do tamanho com x antigo + metade
				se for uma linha par ligar o x antigo + metade do tamanho com x novo - metade do tamanho
				se for uma linha impar ligar o x antigo - metade do tamanho com x novo + metade do tamanho

			*/
			if (counter % max_per_line  == 0 && !isOddLine){
				drawArrowTo(old_x - node_width/2 + next_rect_width/2, old_y+10, x - node_width/2, y-10, 0, 0);
				drawArrowTo(x + next_rect_width/2 - node_width/2, y+10, old_x - node_width/2, old_y-10, 0, 1);
			}
			else if(counter % max_per_line == 0 && isOddLine){
			
				drawArrowTo(old_x + node_width/2 - next_rect_width/2, old_y+10, x + node_width/2, y-10, 1, 0);
				drawArrowTo(x - next_rect_width/2 + node_width/2, y+10, old_x + node_width/2, old_y-10, 1, 1);
			}
			else if (!isOddLine){
				drawArrowTo(old_x + node_width/2 - next_rect_width/2, old_y-10, x - node_width/2, y-10, 0, 0);
				drawArrowTo(x + next_rect_width/2 - node_width/2, y+10, old_x + node_width/2, old_y+10, 1, 0);
			}
			else {
				drawArrowTo(old_x - node_width/2 + next_rect_width/2, old_y-10, x + node_width/2, y-10, 1, 0);
				drawArrowTo(x - next_rect_width/2 + node_width/2, y+10, old_x - node_width/2, old_y+10, 0, 0);
			}
		}
		old_x = x;
		old_y = y;

		x = isOddLine ? x - node_width - node_distance : x + node_width + node_distance;
		ptr = ptr->next;
		counter++;
	}
	gfx_paint();

}

void drawMainScreen(int porcent){
	const int offset = 60; //distancia em pixels entre a parede e a borda do rect
	const int size = 200;
	int lenght = snprintf(NULL, 0, "%d", porcent);
	char *str = malloc(lenght + 1);
	const int memoryOffset = 4;
	const int memoryWidth = (WIDTH - 2 * offset - 400) * 0.01; // tamanho individual de cada memoria
	char txt[30] = "Memoria em uso: ";
	int text_width, text_height;

	gfx_set_color(255, 255, 255);
	//adicionar a porcentagem de memoria e o % em seguida
	snprintf(str, lenght + 1, "%d", porcent);
	strcat(txt, str);
	strcat(txt, "%");
	gfx_set_font_size(30);

	//fazer o texto
	gfx_get_text_size(txt, &text_width,&text_height);
	gfx_text(WIDTH/2 - text_width/2, HEIGHT/2 - 200, txt);

	//contorno do bloco principal
	gfx_rectangle(0 + offset, HEIGHT/2 - size/2, WIDTH - offset, HEIGHT/2 + size/2);
	
	gfx_set_color(124 + porcent, 252 - porcent * 2, 0); //verde ao vermelho dependendo da porcentagem
	for (int i = 0; i < porcent; i++)  // cada bloco individual (do 0% ao 100%)
		gfx_filled_rectangle(0 + offset + memoryOffset + (memoryWidth + memoryOffset) * i, 
						HEIGHT/2 - size/2 + 5,
		 			 	offset + memoryWidth + memoryOffset +(memoryWidth + memoryOffset) * i,
						HEIGHT / 2 + size/2 - 5);
	
	gfx_paint();
}

void pause(){
	fflush(stdout);
	system("read -p \"Pressione uma tecla para continuar\" var");
}