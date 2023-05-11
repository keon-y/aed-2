#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <math.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIDTH 1024
#define HEIGHT 768

typedef struct Node {
	int address; 
	int size; //b
	struct Node *next;

	//fazer funcao para calcular a pos x e y quando mais elementos entram na lista
	int x; //ponto central X
	int y; //ponto central Y
	int height; //tamanho fixo provavelmente
	int width;
} NodeAvailable;

typedef struct DoubleNode {
	int id;
	int address_start;
	int address_finish;
	struct DoubleNode *next;
	struct DoubleNode *previous;
} NodeAllocated;

//funcao para criar seta apontando para um no
/*
void createArrowTo(NodeAvailable * node) {

} */

void drawAvailableBlock(NodeAvailable * node) {
	gfx_rectangle((*node).x - (*node).width/2, (*node).y - (*node).height/2, (*node).x + (*node).width/2, (*node).y + (*node).height/2);
	gfx_paint();
}

void printNodes(NodeAvailable *start){
	NodeAvailable *ptr = start;
	while (ptr != NULL) {
		printf("ADDRESS: %d  SIZE: %d \n", ptr->address, ptr->size);
		ptr = ptr->next;
	}
}



//lista simplesmente encadeada
NodeAvailable *createSingleNode(int address, int size, int x, int y) {
	NodeAvailable *node = (NodeAvailable*)malloc(sizeof(NodeAvailable));
	node->address = address;
	node->size = size;
	node->next = NULL;
	
	//a funcao de tamanho provavelmente entra aqui (calculo usando a variavel size)
	node->height = 100;
	node->width = 300;
	node->x = x;
	node->y = y;

	return node;
}

//verificar juncao de blocos
/*
NodeAvailable *searchSingleList(int size, ListAvailable *list) {
	NodeAvailable *node = list->start;
}*/

//insere um NODE em uma lista simplesmente encadeada ordenada de forma crescente
void insertSingleList(NodeAvailable *node, NodeAvailable **start) { //ponteiro para ponteiro em caso de precisar alterar o comeco da lista
	NodeAvailable *ptr = *start;
	if (!ptr || node->size < ptr->size) { //no menor do que o primeiro elemento da lista ou lista vazia
		node->next = *start; //se nao tiver elementos na lista entao o prox do node eh nulo
		*start = node;
		return;
	}

	while (ptr->next != NULL) {
		if (ptr->next->size > node->size) break;
		ptr = ptr->next;
	}
	node->next = ptr->next;
	ptr->next = node;

}

void deleteSingleList(int size, NodeAvailable **start) {
	
}


int main()
{
	// inicializacao da Memoria
	int memory_size;
	printf("Informe M: ");
	scanf("%d", &memory_size);
	
	gfx_init(WIDTH, HEIGHT, "");
	NodeAvailable *start = createSingleNode(1, 5, 100, 200);

	insertSingleList(createSingleNode(1, 6, 200, 200), &start);
	insertSingleList(createSingleNode(1, 2, 300, 200), &start);
	insertSingleList(createSingleNode(1, 77, 500, 200), &start);
	insertSingleList(createSingleNode(1, 12, 700, 200), &start);
	insertSingleList(createSingleNode(1, 1, 700, 200), &start);
	insertSingleList(createSingleNode(1, 123, 700, 200), &start);
	printNodes(start);
	drawAvailableBlock(start);




	sleep(10);
	gfx_quit();
	return 0;
}