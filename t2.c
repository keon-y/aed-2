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
	int address_end;
	struct DoubleNode *next;
	struct DoubleNode *previous;
} NodeAllocated;


void printSingleNodes(NodeAvailable *start);
NodeAvailable *createSingleNode(int address, int size, int x, int y);
NodeAvailable *searchSingleList(int size, NodeAvailable *start);
void insertSingleList(NodeAvailable *node, NodeAvailable **start);
void removeSingleList(int size, NodeAvailable **start, NodeAllocated **start_allocated, int type);
NodeAllocated *createDoubleNode(int start, int end);
void printDoubleNodes(NodeAllocated *start);
void insertDoublyList(NodeAllocated *node, NodeAllocated **head);
void removeDoublyList (int id, NodeAllocated **head, NodeAvailable **available_start);

int main()
{
	// inicializacao da Memoria
	int memory_size;
	printf("Informe M: ");
	scanf("%d", &memory_size);
	
	gfx_init(WIDTH, HEIGHT, "");
	
	NodeAvailable *start_available = createSingleNode(0, memory_size, 0, 0);
	NodeAllocated *start_allocated = NULL;

	int exit = 0;
	int escolha;
	while (!exit) {
		system("clear");
		printf("Disponiveis:\n");
		printSingleNodes(start_available);
		printf("\nAlocados\n");
		printDoubleNodes(start_allocated);

		printf("\n\n1. Alocar\n2. Desalocar\n3. Sair\n--> ");
		scanf("%d", &escolha);
		if (escolha == 1) {
			int size;
			printf("Tamanho da alocacao: ");
			scanf("%d", &size);

			removeSingleList(size, &start_available, &start_allocated, 1);
		}
		else if (escolha == 2) {
			int id;
			printf("ID da memoria: ");
			scanf("%d", &id);

			removeDoublyList(id, &start_allocated, &start_available);
		}
		else 
			exit = 1;
		
	}




	gfx_quit();
	return 0;
}


void drawAvailableBlock(NodeAvailable * node) {
	gfx_rectangle((*node).x - (*node).width/2, (*node).y - (*node).height/2, (*node).x + (*node).width/2, (*node).y + (*node).height/2);
	gfx_paint();
}

void printSingleNodes(NodeAvailable *start){
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



//insere um NODE em uma lista simplesmente encadeada ordenada de forma crescente
void insertSingleList(NodeAvailable *node, NodeAvailable **start) { //ponteiro para ponteiro em caso de precisar alterar o comeco da lista
	NodeAvailable *ptr = *start;

	if (!ptr) {
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
	//TESTAR se mudar o inicio da lista da segmentation fault
	
	ptr->next = node;

}

void removeSingleList(int size, NodeAvailable **start, NodeAllocated **start_allocated, int type) {

	NodeAvailable *ptr = *start, *prev = NULL;
	if (!ptr) return;
	while (ptr != NULL && !(size <= ptr->size)) {
		prev = ptr;
		ptr = ptr->next;
	}
	//testar se ptr->size < size eh realmente necessario na condicao
	if (!ptr || ptr->size < size) //impossivel alocar (percorreu a lista toda)
		return;

	if (prev) //se nao tiver prev ptr vai ser o primeiro elemento, portanto muda o start
		prev->next = ptr->next;
	else *start = ptr->next;
	//TESTAR o novo endereco inicial
	if (type) {
		if (ptr->size != size) //cria um novo bloco caso o tamanho da memoria nao seja exatamente igual
			insertSingleList(createSingleNode(ptr->address + size, ptr->size - size, 200, 200), start);
		insertDoublyList(createDoubleNode(ptr->address, ptr->address + size - 1), start_allocated);
	}
	free(ptr); //libera a memoria
}

//criar
NodeAllocated *createDoubleNode(int start, int end){
	static int id = 0; //id unico que apenas aumenta a cada nova alocacao
	NodeAllocated *node = malloc(sizeof(NodeAllocated));
	node->id = id;
	node->address_start = start;
	node->address_end = end;
	node->next = NULL;
	node->previous = NULL;
	id++;
	return node;
}

//printar
void printDoubleNodes(NodeAllocated *start){
	NodeAllocated *ptr = start;
	while (ptr){
		printf("ID: %d, ADDRESS START: %d, ADDRESS END: %d\n", ptr->id, ptr->address_start, ptr->address_end);
		ptr = ptr->next;
	}
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

void removeDoublyList (int id, NodeAllocated **head, NodeAvailable **available_start) {
	if (!(*head)) return; //impossivel remover de lista vazia
	
	NodeAllocated *ptr = *head;
	if (ptr->id == id) { //se o no a ser removido eh o primeiro
		if (ptr->next) //se for elemento unico da erro
			ptr->next->previous = NULL;
		*head = ptr->next;

		//voltar para a lista de memoria desalocada
		insertSingleList(
			createSingleNode(ptr->address_start, ptr->address_end - ptr->address_start + 1, 0, 0),
			available_start);

		free(ptr);
		return;
	}

	while (ptr != NULL && ptr->id != id) 
		ptr = ptr->next;

	if(ptr && ptr->id == id) {
		ptr->previous->next = ptr->next;
		if (ptr->next) //o primeiro elemento sempre eh checado, falta checar o ultimo
			ptr->next->previous = ptr->previous;

		insertSingleList(
			createSingleNode(ptr->address_start, ptr->address_end - ptr->address_start + 1, 0, 0),
			available_start);

		free(ptr);
		return;
	}
	printf("Valor inexistente na lista!\n");
}