scp rgm47014@ssh.uems.br:/Downloads/a Downloads
move "David A. Patterson - Organização e Projeto de Computadores-CAMPUS - GRUPO ELSEVIER (2009).pdf" a.pdf

// AUTOR: Victor Keony Okabayashi Takatu

#include "gfx/gfx.h"
#include <stdio.h>  /* printf */
#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1024
#define HEIGHT 768



struct s_arq_no {
    int32_t chave:28;
    int32_t bal:2;
    uint32_t esq:1;
    uint32_t dir:1;
};

typedef struct s_no {
    int32_t chave:28;
    int32_t bal:2;
    int32_t reservado:2; /* sem uso */
    struct s_no* esq;
    struct s_no* dir;
} Node;

//operacoes
int search (int, Node*, Node**);
void insert(int, Node**, int*);
Node *create(int);
void desalocateTree(Node*);
void removeNode(Node*, Node**, Node*, int*, Node*);

//getters
Node *getAncestor(Node*, Node*);
Node *getMin(Node*);
Node *getMax(Node*);
Node *getSuccessor(Node*, int);
Node *getPredecessor(Node*, int);

//rotacao
int checkRotation(Node **, int);

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
	int h = 0;

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
				insert(val, &root, &h);
			break;

			case 2: //remocao
				printf("\nInforme o valor a ser removido da arvore:\n --> ");
				scanf("%d", &val);
				if (search(val, root, &ptr) == 1) {
					removeNode(ptr, &root, NULL, &h, root);
					printf("[SUCESSO] Elemento %d removido da arvore\n", val);
				}
				else printf("[ERRO] Elemento %d nao existe na arvore\n", val);
				h = 0;
			break;

			case 3:
				ptr = getMax(root);
				if (ptr)
					printf("\nMaior valor da arvore: %d\n", ptr->chave);
				ptr = getMin(root);
				if (ptr)
					printf("Menor valor da arvore: %d\n\n", ptr->chave);
				else printf("[ERRO] Arvore vazia\n");
			break;

			case 4:
				printf("\nInforme o valor para descobrir seu sucessor e predecessor: ");
				scanf("%d", &val);
				if (search(val, root, &ptr) != 1) {
					printf("[ERRO] Elemento %d nao existe na arvore", val);
					
				} else {
					ptr = getSuccessor(root, val);
					if (ptr) printf("Sucessor de %d: %d\n", val, ptr->chave);
					else printf("[ERRO] %d nao possui sucessor\n", val);

					ptr = getPredecessor(root, val);
					if (ptr) printf("Predecessor de %d: %d\n", val, ptr->chave);
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

	if (node->chave == val) return 1; //encontrado o elemento exato

	if (val < node->chave && node->esq == NULL) return 2; //menor elemento
	else if (val < node->chave && node->esq) return search(val, node->esq, ret); //buscar o menor

	if (val > node->chave && node->dir == NULL) return 3;
	
	else if (val > node->chave && node->dir) return search(val, node->dir, ret);

	return 0;
}

//cria um no com chave `chave`
Node *create(int chave){
	Node *newNode = malloc(sizeof(Node));
	newNode->chave = chave;
	newNode->esq = NULL;
	newNode->dir = NULL;
	newNode->bal = 0;
	return newNode;
}

int height(Node *pt) {
	if (!pt) return 0;
	int left = height(pt->esq);
	int right = height(pt->dir);
	if (left > right) return left + 1;
	else return right + 1;

}

void rotateRight(Node **pt) {
	Node *ptu = (*pt)->esq;
	if (ptu->bal == -1 || ptu->bal == 0) {
		(*pt)->esq = ptu->dir;
		ptu->dir = *pt;
		(*pt)->bal = height((*pt)->dir) - height((*pt)->esq); //adaptado para a remocao
		*pt = ptu;
		
	}
	else {
		Node *ptv = ptu->dir;
		ptu->dir = ptv->esq;
		ptv->esq = ptu;
		(*pt)->esq = ptv->dir;
		ptv->dir = *pt;
		(*pt)->bal = height((*pt)->dir) - height((*pt)->esq); //tambem adaptado para remocao
		ptu->bal = height(ptu->dir) - height(ptu->esq);  //remocao
		*pt = ptv;
	}
	(*pt)->bal = height((*pt)->dir) - height((*pt)->esq);
}

void rotateLeft(Node **pt) {
	Node *ptu = (*pt)->dir;
	if (ptu->bal == 1 || ptu->bal == 0) {
		printf("rotacao simples em %d\n", (*pt)->chave);
		(*pt)->dir = ptu->esq;
		ptu->esq = *pt;
		(*pt)->bal = height((*pt)->dir) - height((*pt)->esq);
		*pt = ptu;
	}
	else {
		printf("rotacao dupla\n");
		Node *ptv = ptu->esq;
		ptu->esq = ptv->dir;
		ptv->dir = ptu;
		(*pt)->dir = ptv->esq;
		ptv->esq = *pt;
		(*pt)->bal = height((*pt)->dir) - height((*pt)->esq);
		ptu->bal = height(ptu->dir) - height(ptu->esq);
		*pt = ptv;
	}
	(*pt)->bal = height((*pt)->dir) - height((*pt)->esq);
}

//insere um no com chave `chave` na arvore `root` 
void insert(int chave, Node **pt, int *h){
	if (!(*pt)) {
		*pt = create(chave);
		*h = 1;
	}
	if ((*pt)->chave == chave) return;
	if (chave < (*pt)->chave) {
		insert(chave, &((*pt)->esq), h);
		if (*h) 
			*h = checkRotation(pt, 0);
		 
	}
	else {
		insert(chave, &((*pt)->dir), h);
		if (*h)
			*h = checkRotation(pt, 1);
	}
}

//desaloca um no node e todos seus filhos
void desalocateTree(Node *node) {
	if (node->esq)
		desalocateTree(node->esq);
	if (node->dir)
		desalocateTree(node->dir);
	free(node);
}

//encontrar o no pai de um no `child` partindo de um no 'node'
Node *getAncestor(Node *node, Node *child) {
	Node *parent = NULL;
	while (node != child) {
		parent = node;
		node = node->chave >= child->chave ? node->esq : node->dir;
	}
	return parent;
}

//encontra o valor minimo de uma arvore/subarvore
Node *getMin(Node *node) {
	while (node->esq)
		node = node->esq;
	return node;
}

//encontra o valor maximo de uma arvore/subarvore
Node *getMax(Node *node) {
	while (node->dir)
		node = node->dir;
	return node;
}

//encontra o sucessor de um no de valor 'chave' partindo de um no `node`
Node *getSuccessor(Node *node, int chave){
	Node *successor = NULL;
	Node *ptr = NULL; //apontar para o no que contem a chave
	if (!node || search(chave, node, &ptr) != 1)
		return NULL; //ARVORE nao existe ou entao o valor nao existe na arvore
	
	if (ptr->dir) successor = getMin(ptr->dir); //tem subarvore direita

	else while (node != NULL) {
		if (chave >= node->chave) {
			node = node->dir;
		}
		else {
			successor = node;
			node = node->esq;
		}
	}
	return successor;
}

//encontra o predecessor de um no de valor `chave` partindo de um no `node`
Node *getPredecessor(Node *node, int chave){
	Node *predecessor = NULL;
	Node *ptr = NULL;
	if (!node || search(chave, node, &ptr) != 1)
		return NULL; //ARVORE nao existe ou entao o valor nao existe na arvore
	
	if (ptr->esq) predecessor = getMax(ptr->esq); //tem subarvore esquerda

	else while (node != NULL) {
		if (chave <= node->chave) {
			node = node->esq;
		}
		else {
			predecessor = node;
			node = node->dir;
		}
	}
	return predecessor;
}

//retorna h (pra ver se precisa checar)
int checkRotation(Node **pt, int plus) { //plus pra saber se adiciona 1 ou subtrai 1 do bal
	switch ((*pt)->bal) { //switch case dos slides
		case -1: //se for -1 e estiver subtraindo 1, precisa rotacionar
			if (!plus)
				rotateRight(pt);
			else (*pt)->bal = 0; //se nao rodar, volta pra 0 (pq adicionou 1)
			return 0;
		break;

		case 0:
			(*pt)->bal = plus ? 1 : -1;
			return 1;
		break;

		case 1: //rodar caso for adicionar 1
			if (plus) rotateLeft(pt);
			else (*pt)->bal = 0;
			return 0;
		break;
	}
	return 0;
}

void updateBal(Node **node, Node *ancestor, int isLeft) { //pra remocao
	short int h = height((*node)->dir) - height((*node)->esq);

	if (h != -2 && h != 2) {
		(*node)->bal = h;
		return;
	}

	if (h == -2) 
		rotateRight(node);
		
	
	else if (h == 2) 
		rotateLeft(node);

	if (isLeft && ancestor)
		ancestor->esq = *node;
	else if (ancestor)
		ancestor->dir = *node;
}

//remove um no `node`
void removeNode(Node *node, Node **pt, Node *ancestor, int *h, Node *root) {
	if (!node) return;
	if (node != *pt && node->chave < (*pt)->chave) { 
		removeNode(node, &(*pt)->esq, *pt, h, root);
		if (*h) {
			int isLeft;
			if (ancestor) isLeft = ancestor->esq == (*pt);
			updateBal(pt, ancestor, isLeft);
		}
	}
	else if (node != *pt && node->chave > (*pt)->chave) {
		removeNode(node, &(*pt)->dir, *pt, h, root);
		if (*h) {
			int isLeft;
			if (ancestor) isLeft = ancestor->esq == (*pt);
			updateBal(pt, ancestor, isLeft);
		}
	}
	else {
		Node *successor = getSuccessor(*pt, node->chave);
		int isLeftNode;
		if (ancestor)
			isLeftNode = ancestor->esq == node; //variavel para saber se `node` eh um filho da direita ou esquerda

		if (!node->esq && !node->dir) { //nao tem subarvores
			if (ancestor && isLeftNode) //setar como lambda no ancestral
				ancestor->esq = NULL;
			else if (ancestor) 
				ancestor->dir = NULL;
			if (ancestor) //se for a raiz nao precisa checar rotacoes
				*h = 1;
			*pt = NULL; 
		}

		else if (!node->esq) {//caso (a) (nao tem filho esquerdo)
			if (ancestor && isLeftNode) //caso for um filho da esquerda do ancestral, substituir o filho da esquerda do ancestral
				ancestor->esq = node->dir;
			else if (ancestor)
				ancestor->dir = node->dir;
			*pt = node->dir;
			if (ancestor) *h = 1;
		}

		else if (node->esq && !node->dir) {//caso (b) (apenas um filho a direita)
			if (ancestor && isLeftNode)
				ancestor->esq = node->esq;
			else if (ancestor)
				ancestor->dir = node->esq;
			*pt = node->esq;
			if (ancestor) *h = 1;
		}
		
		else if (node->dir && node->dir == successor) { //caso c
			if (successor->dir != NULL) *h = 1;
			successor->bal = node->bal; //herda o balanço e subtrai 1 (pq a subarvore esquerda diminuiu 1)
			successor->esq = node->esq; //herdar a subarvore esquerda, mesmo se for lambda, successor nunca vai ter subarvore esquerda
			checkRotation(&successor, 0); // bal -1
			if (ancestor && ancestor->chave > node->chave) //z eh a chave esquerda do ancestral
				ancestor->esq = successor;
			else if (ancestor) ancestor->dir = successor; //chave direita
			*pt = successor;
		}

		//caso d
		else {
			int isLeft;
			Node *successor_ancestor = getAncestor(root, successor);
			Node *successor_ancestor_ancestor = successor_ancestor ? getAncestor(root, successor_ancestor) : NULL; //mudar o esq ou dir depois de fazer a rotacao!
			//se o no a ser removido eh o ancestral do ancestral do sucessor, entao quando for substituido sera o proprio sucessor
			if (node == successor_ancestor_ancestor) successor_ancestor_ancestor = successor; 
			if (successor_ancestor_ancestor)
				isLeft = successor_ancestor_ancestor->esq == successor_ancestor;

			if (successor_ancestor->dir != NULL) *h = 1;
			if (successor_ancestor) {// SEMPRE o sucessor vai ser um filho da esquerda
				successor_ancestor->esq = successor->dir; //sucessor NUNCA vai ter uma subarvore esquerda

			}
			if (ancestor)
				ancestor->dir = successor;
			successor->esq = node->esq;
			successor->dir = node->dir;
			*pt = successor;

			//nao precisa de if pq sempre vai existir (caso nao exista entra no caso C)
			updateBal(&successor_ancestor, successor_ancestor_ancestor, isLeft);

			
			if (successor_ancestor_ancestor)
				successor_ancestor_ancestor->bal = height(successor_ancestor_ancestor->dir) - height(successor_ancestor_ancestor->esq);
			
		}
		if (*pt) 
			(*pt)->bal = height((*pt)->dir) - height((*pt)->esq);
		free(node);
	}
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
	if (node->esq) { 
		gfx_line(x, y, x - (200/depth), y + 80);
		paintNode(node->esq, x - (200/depth), y + 80, depth + 1);
	}
	if (node->dir) {
		gfx_line(x, y, x + (200/depth), y + 80);
		paintNode(node->dir, x + (200/depth), y + 80, depth + 1);
	}
	gfx_set_color(0, 0, 0);
	gfx_filled_ellipse(x, y, 22, 22);
	gfx_set_color(255,255,255);
	gfx_ellipse(x, y, 22, 22);
	sprintf(text, "%d", node->chave);
	gfx_get_text_size(text, &text_w, &text_h);
	gfx_text(x - text_w/2, y - text_h/2, text);

	sprintf(text, "%d", node->bal);
	gfx_get_text_size(text, &text_w, &text_h);
	gfx_text(x - text_w/2, y - text_h/2 + 10, text);

}

//funcao recursiva para construir a arvore
void preorderRead(Node *ancestor, FILE *ptr, int isLeft) {
	struct s_arq_no *binaryNode = malloc(sizeof(struct s_arq_no));
	fread(binaryNode, sizeof(struct s_arq_no), 1, ptr);

	Node *newNode = malloc(sizeof(Node)); //alocar o novo no
	newNode->chave = binaryNode->chave;
	newNode->esq = NULL;
	newNode->dir = NULL;
	newNode->bal = binaryNode->bal;

	if (isLeft)	ancestor->esq = newNode;
	else ancestor->dir = newNode;
	

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
	(*root)->bal = binaryNode->bal;
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
	binaryNode->chave = node->chave;
	binaryNode->esq = node->esq != NULL;
	binaryNode->dir = node->dir != NULL;
	binaryNode->bal = node->bal;

	fwrite(binaryNode, sizeof(struct s_arq_no), 1, ptr);
	if (node->esq) preorderSave(node->esq, ptr);
	if (node->dir) preorderSave(node->dir, ptr);
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
