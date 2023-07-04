#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <math.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 1024
#define HEIGHT 768



struct s_arq_no
{
    int32_t chave:30;
    uint32_t esq:1;
    uint32_t dir:1;
};

typedef struct Node {
	int key;
	struct Node *left;
	struct Node *right;
} Node;

/*  Procura um valor val e armazena o ponteiro para o no em ret
	retorna um valor f:
	0 para arvore vazia
	1 para elemento encontrado
	2 para val MENOR que a chave do no
	3 para val MAIOR que a chave do no
*/
int search(int val, Node *node, Node **ret) {
	if (!node) return 0;
	*ret = node;

	if (node->key == val) return 1; //encontrado o elemento exato

	if (val < node->key && node->left == NULL) return 2; //menor elemento
	else if (val < node->key && node->left) return search(val, node->left, ret); //buscar o menor

	if (val > node->key && node->right == NULL) return 3;
	
	else if (val > node->key && node->right) return search(val, node->right, ret);

	return 0;
}

Node *create(int key){
	Node *newNode = malloc(sizeof(Node));
	newNode->key = key;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

int insert(int key, Node **root){

	Node *newNode = create(key);
	Node *pt = *root;
	int status = search(key, pt, &pt); //os codigos da busca (1 para igual, 2 para menor, etc)
	printf("status: %d\n", status);

	switch(status) {
		case 1:
			printf("insercao invalida");
			return 0;
		case 0:
			*root = newNode;
			break;
		case 2:
			pt->left = newNode;
			break;
		case 3:
			pt->right = newNode;
			break;
	}

	return 1;
}

//checa se dado no eh uma folha
int checkLeaf(Node *node){
	return node->left == NULL && node->right == NULL;
}

//encontrar o no pai de um no `child`
Node *getAncestor(Node *root, Node *child) {
	Node *parent = NULL;
	while (root != child) {
		parent = root;
		root = root->key >= child->key ? root->left : root->right;
	}
	return parent;
}

Node *getMin(Node *root) {
	while (root->left)
		root = root->left;
	return root;
}

Node *getMax(Node *root) {
	while (root->right)
		root = root->right;
	return root;
}

Node *getSuccessor(Node *root, int key){
	Node *successor = NULL;
	Node *ptr = NULL; //apontar para o no que contem a chave
	if (!root || search(key, root, &ptr) != 1)
		return NULL; //ARVORE nao existe ou entao o valor nao existe na arvore
	
	if (ptr->right) successor = getMin(ptr->right); //tem subarvore direita

	else while (root != NULL) {
		if (key >= root->key) {
			root = root->right;
		}
		else {
			successor = root;
			root = root->left;
		}
	}
	return successor;
}

Node *getPredecessor(Node *root, int key){
	Node *predecessor = NULL;
	Node *ptr = NULL;
	if (!root || search(key, root, &ptr) != 1)
		return NULL; //ARVORE nao existe ou entao o valor nao existe na arvore
	
	if (ptr->left) predecessor = getMax(ptr->left); //tem subarvore esquerda

	else while (root != NULL) {
		if (key <= root->key) {
			root = root->left;
		}
		else {
			predecessor = root;
			root = root->right;
		}
	}
	return predecessor;
}

void removeNode(Node *node, Node **root) {
	if (!node) return;

	Node *ancestor = getAncestor(*root, node);
	Node *successor = getSuccessor(*root, node->key);
	int isLeftNode;
	if (ancestor)
		isLeftNode = ancestor->left == node; //variavel para saber se `node` eh um filho da direita ou esquerda
	//TODO: ponteiro pra ponteiro que aponta para o no para alterar (right ou left do ancestor)
	if (!node->left && !node->right) { //nao tem subarvores
		if (ancestor && isLeftNode) //setar como lambda no ancestral
			ancestor->left = NULL;
		else if (ancestor) 
			ancestor->right = NULL;
		if (node == *root)
			*root = NULL;
	}

	else if (!node->left) {//caso (a) (nao tem filho esquerdo)
		if (ancestor && isLeftNode) //caso for um filho da esquerda do ancestral, substituir o filho da esquerda do ancestral
			ancestor->left = node->right;
		else if (ancestor)
			ancestor->right = node->right;
		if (node == *root)
			*root = node->right;
	}

	else if (node->left && !node->right) {//caso (b) (apenas um filho a direita)
		if (ancestor && isLeftNode)
			ancestor->left = node->left;
		else if (ancestor)
			ancestor->right = node->left;
		if (node == *root)
			*root = node->left;
	}
	
	else if (node->right && node->right == successor) { //caso c
		successor->left = node->left; //herdar a subarvore esquerda, mesmo se for lambda, successor nunca vai ter subarvore esquerda
		if (ancestor && ancestor->key > node->key) //z eh a chave esquerda do ancestral
			ancestor->left = successor;
		else if (ancestor) ancestor->right = successor; //chave direita
		if (node == *root)
			*root = successor;
	}

	//caso d
	else {
		Node *successor_ancestor = getAncestor(*root, successor);
		if (successor_ancestor) // SEMPRE o sucessor vai ser um filho da esquerda
			successor_ancestor->left = successor->right; //sucessor NUNCA vai ter uma subarvore esquerda
		if (ancestor)
			ancestor->right = successor;
		successor->left = node->left;
		successor->right = node->right;
		if (node == *root)
			*root = successor;
	}
	free(node);

}

void paintNode(Node *node, int x, int y, int depth) {
	if (!node) return;
	if (node->left) {
		printf("%d left -> %d\n", node->key, node->left->key);
		gfx_line(x, y, x - (200/depth), y + 80);
		paintNode(node->left, x - (200/depth), y + 80, depth + 1);
	}
	if (node->right) {
		printf("%d right -> %d\n", node->key, node->right->key);
		gfx_line(x, y, x + (200/depth), y + 80);
		paintNode(node->right, x + (200/depth), y + 80, depth + 1);
	}
	gfx_set_color(0, 0, 0);
	gfx_filled_ellipse(x,y,30,30);
	gfx_set_color(255,255,255);
	gfx_ellipse(x, y, 30, 30);
	char text[30];
	int text_w, text_h;
	sprintf(text, "%d", node->key);
	gfx_get_text_size(text, &text_w, &text_h);
	gfx_text(x - text_w/2, y - text_h/2, text);
}

int main() {
	
	Node *root = NULL;

	insert(3, &root);
	insert(1, &root);
	insert(7, &root);
	insert(2, &root);
	insert(5, &root);
	insert(4, &root);
	insert(6, &root);
	insert(9, &root);
	insert(8, &root);

	gfx_init(WIDTH, HEIGHT, "");

	int escolha, val;
	Node *ptr;

	do {
		gfx_clear();
		paintNode(root, WIDTH/2, 90, 1);
		gfx_paint();
		printf("1. Inserir\n2. Remover\n3. Buscar maior e menor\n4. Buscar sucessor e predecessor\n5. Sair\n--> ");
		scanf("%d", &escolha);
		switch (escolha){
			case 1: //insercao
				printf("Informe o valor a ser inserido na arvore: ");
				scanf("%d", &val);
				insert(val, &root);
			break;

			case 2: //remocao
				printf("Informe o valor a ser removido da arvore: ");
				scanf("%d", &val);
				if (search(val, root, &ptr) == 1)
					removeNode(ptr, &root);
			break;

			case 3:
				ptr = getMax(root);
				if (ptr)
					printf("Maior valor da arvore: %d\n", ptr->key);
				ptr = getMin(root);
				if (ptr)
					printf("Menor valor da arvore: %d\n", ptr->key);
			break;

			case 4:
				printf("Informe o valor para descobrir seu sucessor e predecessor: ");
				scanf("%d", &val);
				if (search(val, root, &ptr) != 1) {
					printf("Elemento nao existe na arvore!");
					break;
				}
				ptr = getSuccessor(root, val);
				if (ptr) printf("Sucessor de %d: %d\n", val, ptr->key);

				ptr = getPredecessor(root, val);
				if (ptr) printf("Predecessor de %d: %d\n", val, ptr->key);
			break;

			case 5:
			break;
			default:
				printf("Opcao inexistente");
		}


	} while (escolha != 5);
	gfx_quit();
	

	return 1;
}