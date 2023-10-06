#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    int32_t arr[argc - 2];
    for (int i = 0; i < argc - 2; i++)
        arr[i] = atoi(argv[i + 2]);

    FILE *dest = fopen(argv[1], "wb");
    for (int i = 0; i < argc - 2; i++)
        fwrite(&arr[i], sizeof(int32_t), 1, dest); //VER DEPOIS a lista pode mudar, dependendo do sort outra lista pode ser criada
    fclose(dest);
    
    return 0;
}
