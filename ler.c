#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if (argc != 2) { //error handler basico para ver se o usuario colocou argumentos
        printf("Quantidade invalida de argumentos!\n");
        return 0;
    }

    //ler
    FILE *ptr = fopen(argv[1], "rb"); //tenta abrir o arquivo
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

    for (int i = 0; i < size; i++)
        printf("%d ", list[i]);
    printf("\n");
    


    return 0;
}