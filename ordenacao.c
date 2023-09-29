#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

void bubbleSort(size_t size, int32_t list[size]);

int main(int argc, char *argv[]) {
    if (argc != 4) { //error handler basico para ver se o usuario colocou argumentos
        printf("Quantidade invalida de argumentos!\n");
        return 0;
    }

    //ler
    FILE *ptr = fopen(argv[2], "rb"); //tenta abrir o arquivo
    if (!ptr) {
        printf("Arquivo %s nao existe no diretorio!\n", argv[2]);
        return 0;
    }

    struct stat st;
    size_t size = 0;
    if (fstat(fileno(ptr), &st) == -1) {
        printf("Erro ao tentar descobrir o tamanho de %s!\n", argv[2]);
        return 0;
    }
    size = st.st_size / sizeof(int32_t);
    int32_t list[size];

    for (int i = 0; i < size; i++) 
        fread(&list[i], sizeof(int32_t), 1, ptr);
    fclose(ptr);

    /*
        1. Bubble-sort original;
        2. Bubble-sort melhorado;
        3. Insertion-sort;
        4. Mergesort;
        5. Quicksort com pivô sendo o último elemento;
        6. Quicksort com pivô sendo um elemento aleatório;
        7. Quicksort com pivô sendo a mediana de três;
        8. Heapsort.
    */

    switch (atoi(argv[1])) {
        case 1:
            
            bubbleSort(size, list);
            break;
        default:
            printf("Escolha inexistente!\n");
            return 0;
    }

    



    FILE *dest = fopen(argv[3], "wb");
    for (int i = 0; i < size; i++)
        fwrite(&list[i], sizeof(int32_t), 1, dest); //VER DEPOIS a lista pode mudar, dependendo do sort outra lista pode ser criada
    fclose(dest);
    
    return 0;
}

void bubbleSort(size_t size, int32_t list[size]){
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1 - i; j++) {
            if (list[j] > list[j + 1]) {
                int32_t temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}