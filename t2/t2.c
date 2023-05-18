#include "../gfx/gfx.h"
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


//encontra um elemento pelo atributo size em uma lista ordenada, retorna o ponteiro para o elemento ou o primeiro maior (em caso de não existir)
NodeAvailable *searchSingleList(int size, NodeAvailable *start) {
	NodeAvailable *ptr = start;
	
	while (ptr->next != NULL && !(!ptr || size <= ptr->size)) { //no menor ou igual do que o primeiro elemento da lista ou lista vazia
		if (ptr->next->size > size) break;
		ptr = ptr->next;
	}
	return ptr;
}

//insere um NODE em uma lista simplesmente encadeada ordenada de forma crescente
void insertSingleList(NodeAvailable *node, NodeAvailable **start) { //ponteiro para ponteiro em caso de precisar alterar o comeco da lista
	NodeAvailable *ptr = searchSingleList(node->size, *start);

	node->next = (!ptr || node->size < ptr->size) ? ptr : ptr->next;
	if (ptr == *start && node->size < ptr->size) 
		*start = node;
	else ptr->next = node;

}

void deleteSingleList(int size, NodeAvailable **start) {

	NodeAvailable *ptr = *start, *prev = NULL;
	if (!ptr) return;
	while (ptr != NULL) {
		if (size <= ptr->size) break;
		prev = ptr;
		ptr = ptr->next;
	}

	if (ptr->size < size) //impossivel alocar
		return;

	if (prev)
		prev->next = ptr->next;
	else *start = ptr->next;

	if (ptr->size != size) //cria um novo bloco caso o tamanho da memoria nao seja exatamente igual
		insertSingleList(createSingleNode(1, ptr->size - size, 200, 200), start); //testar em caso de start mudar
	free(ptr); //libera a memoria
}


int main()
{
	// inicializacao da Memoria
	int memory_size;
	printf("Informe M: ");
	scanf("%d", &memory_size);
	
	gfx_init(WIDTH, HEIGHT, "");
	
	NodeAvailable *start;
	start = createSingleNode(1, 5, 100, 200);

	insertSingleList(createSingleNode(1, 6, 200, 200), &start);
	insertSingleList(createSingleNode(1, 2, 300, 200), &start);
	insertSingleList(createSingleNode(1, 77, 500, 200), &start);
	insertSingleList(createSingleNode(1, 12, 700, 200), &start);
	insertSingleList(createSingleNode(1, 1, 700, 200), &start);
	insertSingleList(createSingleNode(1, 123, 700, 200), &start);
	insertSingleList(createSingleNode(1, 3123, 700, 200), &start);
	insertSingleList(createSingleNode(1, 0, 700, 200), &start);

	

	printNodes(start);
	printf("\n");
deleteSingleList(0, &start);
deleteSingleList(12, &start);
deleteSingleList(1000, &start);
	printNodes(start);

	drawAvailableBlock(start);




	sleep(10);
	gfx_quit();
	return 0;
}