// AUTOR: Victor Keony Okabayashi Takatu

#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>

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

//operacoes
int search (int, Node*, Node**);
void insert(int, Node**);
Node *create(int);
void desalocateTree(Node*);
void removeNode(Node*, Node**);

//getters
Node *getAncestor(Node*, Node*);
Node *getMin(Node*);
Node *getMax(Node*);
Node *getSuccessor(Node*, int);
Node *getPredecessor(Node*, int);

//utils
void paintNode(Node*, int, int, int);
void pause();

//manipulacao de arquivos
void readTree(Node**, char*);
void preorderRead(Node*, FILE*, int);
void preorderSave(Node*, FILE*);
void saveTree(Node*, char*);

int main() {
	
	Node *root = NULL;
	char str[30];

	gfx_init(WIDTH, HEIGHT, "");

	int escolha, val;
	Node *ptr;

	do {
		gfx_clear();
		paintNode(root, WIDTH/2, 90, 1);
		gfx_paint();
		printf("1. Inserir\n2. Remover\n3. Buscar maior e menor\n4. Buscar sucessor e predecessor\n5. Ler arquivo\n6. Salvar arquivo\n7. Sair\n--> ");
		scanf("%d", &escolha);
		switch (escolha){
			case 1: //insercao
				printf("\nInforme o valor a ser inserido na arvore: \n --> ");
				scanf("%d", &val);
				insert(val, &root);
			break;

			case 2: //remocao
				printf("\nInforme o valor a ser removido da arvore:\n --> ");
				scanf("%d", &val);
				if (search(val, root, &ptr) == 1) {
					removeNode(ptr, &root);
					printf("[SUCESSO] Elemento %d removido da arvore\n", val);
				}
				else printf("[ERRO] Elemento %d nao existe na arvore\n", val);
			break;

			case 3:
				ptr = getMax(root);
				if (ptr)
					printf("\nMaior valor da arvore: %d\n", ptr->key);
				ptr = getMin(root);
				if (ptr)
					printf("Menor valor da arvore: %d\n\n", ptr->key);
				else printf("[ERRO] Arvore vazia\n");
			break;

			case 4:
				printf("\nInforme o valor para descobrir seu sucessor e predecessor: ");
				scanf("%d", &val);
				if (search(val, root, &ptr) != 1) {
					printf("[ERRO] Elemento %d nao existe na arvore", val);
					
				} else {
					ptr = getSuccessor(root, val);
					if (ptr) printf("Sucessor de %d: %d\n", val, ptr->key);
					else printf("[ERRO] %d nao possui sucessor\n", val);

					ptr = getPredecessor(root, val);
					if (ptr) printf("Predecessor de %d: %d\n", val, ptr->key);
					else printf("[ERRO] %d nao possui predecessor\n", val);
				}
			break;

			case 5:
				printf("\nInforme o nome do arquivo:\n --> ");
				scanf("%s", str);
				readTree(&root, str);
			break;

			case 6:
				printf("\nInforme o nome do arquivo:\n --> ");
				scanf("%s", str);
				saveTree(root, str);
			break;

			case 7:
			break;

			default:
				printf("[ERRO] Opcao inexistente");
		}
		printf("\n\n");
		if (escolha != 7) //nao pedir para pressionar tecla quando for sair
			pause();
	} while (escolha != 7);
	gfx_quit();
	if (root)
		desalocateTree(root);
	

	return 1;
}


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

//cria um no com chave `key`
Node *create(int key){
	Node *newNode = malloc(sizeof(Node));
	newNode->key = key;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

//insere um no com chave `key` na arvore `root` 
void insert(int key, Node **root){

	Node *newNode = create(key);
	Node *pt = *root;
	int status = search(key, pt, &pt); //os codigos da busca (1 para igual, 2 para menor, etc)

	switch(status) {
		case 1:
			printf("[ERRO] %d ja existe na arvore\n", key);
			break;
		case 0:
			*root = newNode;
			printf("[SUCESSO] Arvore criada com raiz %d\n", key);
			break;
		case 2:
			pt->left = newNode;
			printf("[SUCESSO] No %d adicionado a arvore\n", key);
			break;
		case 3:
			pt->right = newNode;
			printf("[SUCESSO] No %d adicionado a arvore\n", key);
			break;
	}
}

//desaloca um no node e todos seus filhos
void desalocateTree(Node *node) {
	if (node->left)
		desalocateTree(node->left);
	if (node->right)
		desalocateTree(node->right);
	free(node);
}

//encontrar o no pai de um no `child` partindo de um no 'node'
Node *getAncestor(Node *node, Node *child) {
	Node *parent = NULL;
	while (node != child) {
		parent = node;
		node = node->key >= child->key ? node->left : node->right;
	}
	return parent;
}

//encontra o valor minimo de uma arvore/subarvore
Node *getMin(Node *node) {
	while (node->left)
		node = node->left;
	return node;
}

//encontra o valor maximo de uma arvore/subarvore
Node *getMax(Node *node) {
	while (node->right)
		node = node->right;
	return node;
}

//encontra o sucessor de um no de valor 'key' partindo de um no `node`
Node *getSuccessor(Node *node, int key){
	Node *successor = NULL;
	Node *ptr = NULL; //apontar para o no que contem a chave
	if (!node || search(key, node, &ptr) != 1)
		return NULL; //ARVORE nao existe ou entao o valor nao existe na arvore
	
	if (ptr->right) successor = getMin(ptr->right); //tem subarvore direita

	else while (node != NULL) {
		if (key >= node->key) {
			node = node->right;
		}
		else {
			successor = node;
			node = node->left;
		}
	}
	return successor;
}

//encontra o predecessor de um no de valor `key` partindo de um no `node`
Node *getPredecessor(Node *node, int key){
	Node *predecessor = NULL;
	Node *ptr = NULL;
	if (!node || search(key, node, &ptr) != 1)
		return NULL; //ARVORE nao existe ou entao o valor nao existe na arvore
	
	if (ptr->left) predecessor = getMax(ptr->left); //tem subarvore esquerda

	else while (node != NULL) {
		if (key <= node->key) {
			node = node->left;
		}
		else {
			predecessor = node;
			node = node->right;
		}
	}
	return predecessor;
}

//remove um no `node` de uma arvore `root`
void removeNode(Node *node, Node **root) {
	if (!node) return;

	Node *ancestor = getAncestor(*root, node);
	Node *successor = getSuccessor(*root, node->key);
	int isLeftNode;
	if (ancestor)
		isLeftNode = ancestor->left == node; //variavel para saber se `node` eh um filho da direita ou esquerda

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

//printa a arvore na tela
void paintNode(Node *node, int x, int y, int depth) {
	char text[30];
	int text_w, text_h;
	if (!node) { //arvore vazia
		gfx_set_font_size(40);
		gfx_get_text_size("Arvore Vazia", &text_w, &text_h);
		gfx_text(WIDTH/2 - text_w/2, HEIGHT/2 - text_h/2, "Arvore Vazia");
		return;
	}
	gfx_set_font_size(10);

	//pintar em pos ordem
	if (node->left) { 
		gfx_line(x, y, x - (200/depth), y + 80);
		paintNode(node->left, x - (200/depth), y + 80, depth + 1);
	}
	if (node->right) {
		gfx_line(x, y, x + (200/depth), y + 80);
		paintNode(node->right, x + (200/depth), y + 80, depth + 1);
	}
	gfx_set_color(0, 0, 0);
	gfx_filled_ellipse(x, y, 22, 22);
	gfx_set_color(255,255,255);
	gfx_ellipse(x, y, 22, 22);
	sprintf(text, "%d", node->key);
	gfx_get_text_size(text, &text_w, &text_h);
	gfx_text(x - text_w/2, y - text_h/2, text);
}

//funcao recursiva para construir a arvore
void preorderRead(Node *ancestor, FILE *ptr, int isLeft) {
	struct s_arq_no *binaryNode = malloc(sizeof(struct s_arq_no));
	fread(binaryNode, sizeof(struct s_arq_no), 1, ptr);

	Node *newNode = malloc(sizeof(Node)); //alocar o novo no
	newNode->key = binaryNode->chave;
	newNode->left = NULL;
	newNode->right = NULL;

	if (isLeft)	ancestor->left = newNode;
	else ancestor->right = newNode;
	

	if (binaryNode->esq) {
		preorderRead(newNode, ptr, 1);
	}
	if (binaryNode->dir) {
		preorderRead(newNode, ptr, 0);
	}
	free(binaryNode);
}

//funcao para abrir o arquivo `filename` e setar a raiz de uma arvore `root`
void readTree(Node **root, char *filename){
	FILE *ptr = fopen(filename, "rb");
	if (!ptr) {
		printf("[ERRO] Arquivo nao encontrado\n");
		return;
	}

	fseek(ptr, 0, SEEK_END); //saber se tem UM no no arquivo (no minimo)
	size_t size = ftell(ptr);
	fseek(ptr, 0, SEEK_SET);

	if (sizeof(struct s_arq_no) > size) { //carregar um arquivo vazio significa desalocar a arvore
		desalocateTree(*root);
		*root = NULL;
		printf("[ERRO] Arquivo vazio\n");
		return;
	}

	struct s_arq_no *binaryNode = malloc(sizeof(struct s_arq_no));
	fread(binaryNode, sizeof(struct s_arq_no), 1, ptr);

	//desalocar a raiz antiga (caso exista)
	if (*root)
		desalocateTree(*root);
	
	*root = malloc(sizeof(Node));
	*root = create(binaryNode->chave);
	if (binaryNode->esq)
		preorderRead(*root, ptr, 1);
	if (binaryNode->dir)
		preorderRead(*root, ptr, 0);
	
	free(binaryNode);
	fclose(ptr);
	printf("[SUCESSO] Arquivo %s carregado\n", filename);
}

void preorderSave(Node *node, FILE *ptr) {
	struct s_arq_no *binaryNode = malloc(sizeof(struct s_arq_no));
	binaryNode->chave = node->key;
	binaryNode->esq = node->left != NULL;
	binaryNode->dir = node->right != NULL;
	fwrite(binaryNode, sizeof(struct s_arq_no), 1, ptr);
	if (node->left) preorderSave(node->left, ptr);
	if (node->right) preorderSave(node->right, ptr);
	free(binaryNode);
}

void saveTree(Node *root, char *filename){

	FILE *ptr = fopen(filename, "wb+");
	if (!ptr) return;
	if (!root) fputc(0, ptr); //arquivo vazio
	else preorderSave(root, ptr);


	fclose(ptr);
	printf("[SUCESSO] Arquivo %s salvo\n", filename);
}

void pause(){
	fflush(stdout);
	system("read -p \"Pressione uma tecla para continuar\" var");
}
