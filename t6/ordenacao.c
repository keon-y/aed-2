#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void swap(int32_t*, unsigned long long, unsigned long long);
void printArr(unsigned long long size, int32_t arr[size]);
void bubbleSort(unsigned long long size, int32_t list[size]);
void bubbleSortOptimized(unsigned long long size, int32_t list[size]);
void insertionSort(long long size, int32_t list[size]);
void mergeSort(unsigned long long size, int32_t arr[size], int32_t temp[size], int ini, int fim);
void merge(unsigned long long size, int32_t arr[size], int32_t temp[size], int ini1, int ini2, int fim2);
void quickSortLast(unsigned long long size, int32_t arr[size], int ini, int fim);
int randomPivot(int ini, int fim);
void quickSortRandom(unsigned long long size, int32_t arr[size], int ini, int fim);
int choosePivot(int32_t arr[], int a, int b, int c);
void quickSortMedian(unsigned long long size, int32_t arr[size], int ini, int fim);
void descer(int32_t arr[], int i, long long n);
void heapSort(long long size, int32_t arr[size]);


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
    unsigned long long size = 0;
    if (fstat(fileno(ptr), &st) == -1) {
        printf("Erro ao tentar descobrir o tamanho de %s!\n", argv[2]);
        return 0;
    }
    size = st.st_size / sizeof(int32_t);
    int32_t *list = malloc(sizeof(int32_t) * size);
    int32_t *temp;
    if (atoi(argv[1]) == 4)
        temp = malloc(sizeof(int32_t) * size);

    for (int i = 0; i < size; i++) 
        fread(&list[i], sizeof(int32_t), 1, ptr);
    fclose(ptr);

    clock_t begin, end;
    double time_spent;

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
            begin = clock();
            bubbleSort(size, list);
            end = clock();
            break;
        case 2:
            begin = clock();
            bubbleSortOptimized(size, list);
            end = clock();
            break;
        case 3:
            begin = clock();
            insertionSort(size, list);
            end = clock();
            break;
        case 4:
            begin = clock();
            mergeSort(size, list, temp, 0, size - 1);
            end = clock();
            break;
        case 5:
            begin = clock();
            quickSortLast(size, list, 0, size - 1);
            end = clock();
            break;
        case 6:
            begin = clock();
            quickSortRandom(size, list, 0, size-1);
            end = clock();
            break;
        case 7:
            begin = clock();
            quickSortMedian(size, list ,0, size-1);
            end = clock();
            break;
        case 8:
            begin = clock();
            heapSort(size - 1, list);
            end = clock();
            break;

        default:
            printf("Escolha inexistente!\n");
            return 0;
    }

    
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f\n", time_spent);


    FILE *dest = fopen(argv[3], "wb");
    for (int i = 0; i < size; i++)
        fwrite(&list[i], sizeof(int32_t), 1, dest);
    fclose(dest);

    free(list);
    if (atoi(argv[1]) == 4)
        free(temp);
    
    return 0;
}

void swap(int32_t arr[], unsigned long long index1, unsigned long long index2){
    int32_t temp = arr[index1];
    arr[index1] = arr[index2];
    arr[index2] = temp;

    //printf("Trocando %d com %d\n", *(arr + index1), *(arr + index2));
}

void printArr(unsigned long long size, int32_t arr[size]) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void bubbleSort(unsigned long long size, int32_t list[size]){
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1; j++) {
            if (list[j] > list[j + 1]) {
                swap(list, j, j+1);
            }
        }
    }
}

void bubbleSortOptimized(unsigned long long size, int32_t list[size]){
    bool changed = true;
    int j, end = size - 1, lastChangedIndex = size;
    while (changed) {
        j = 0;
        changed = false;
        for (j = 0; j < end; j++) {
            if (list[j] > list[j + 1]) {
                swap(&list[0], j, j+1);
                changed = true;
                lastChangedIndex = j;
            }
        }
        end = lastChangedIndex;
    }
}

void insertionSort(long long size, int32_t list[size]) {
    int32_t prov;
    int j;
    for (int i = 1; i < size; i++) {
        prov = list[i];
        j = i - 1;

        while (j >= 0 && prov < list[j]) { 
            list[j+1] = list[j];
            j = j - 1;
        }
        list[j + 1] = prov;
    }
}

void mergeSort(unsigned long long size, int32_t arr[size], int32_t temp[size], int ini, int fim){
    if (ini < fim) {
        int meio = (ini + fim) / 2;
        mergeSort(size, arr, temp, ini, meio);
        mergeSort(size, arr, temp, meio + 1, fim);
        merge(size, arr, temp, ini, meio + 1, fim);
    }
}

void merge(unsigned long long size, int32_t arr[size], int32_t temp[size], int ini1, int ini2, int fim2) {
    int fim1 = ini2 - 1;
    int _ini1 = ini1; //salvar o valor de ini1
    int ind = ini1;
    int nro = 0;

    while (ini1 <= fim1 && ini2 <= fim2) {
        if (arr[ini1] < arr[ini2]) 
            temp[ind++] = arr[ini1++];
        else temp[ind++] = arr[ini2++];
        nro++;
    }
    while (ini1 <= fim1) {
        temp[ind++] = arr[ini1++];
        nro++;
    }
    while (ini1 <= fim1) {
        temp[ind++] = arr[ini2++];
        nro++;
    }
    for (int i = 0; i < nro; i++) {
        arr[i + _ini1] = temp[i + _ini1];
    }

}

void quickSortLast(unsigned long long size, int32_t arr[size], int ini, int fim) {
    int i, j;
    if (fim - ini < 2) {
        if (fim - ini == 1) {
            if (arr[ini] > arr[fim]) {
                swap(arr, ini, fim);
            }
        }
    }
    else {
        int pivot = arr[fim];
        i = ini;
        j = fim - 1;
          while (j >= i) {
            while (arr[i] < pivot)
                i++;
            while (arr[j] > pivot && j >= 0)
                j--;
            if (j >= i) {
                swap(arr, i, j);
                i++;
                j--;
            }
        }
        swap(arr, i, fim);
        quickSortLast(size, arr, ini, i - 1);
        quickSortLast(size, arr, i + 1, fim);
    }
}


int randomPivot(int ini, int fim) {
    srand(time(NULL));
    return ini + rand() % (fim - ini + 1);
}

void quickSortRandom(unsigned long long size, int32_t arr[size], int ini, int fim) {
    int i, j;
    if (fim - ini < 2) {
        if (fim - ini == 1) {
            if (arr[ini] > arr[fim]) {
                swap(arr, ini, fim);
            }
        }
    } else {
        int pivotIndex = randomPivot(ini, fim);
        swap(arr, pivotIndex, fim);

        int pivot = arr[fim];
        i = ini;
        j = fim - 1;
        while (j >= i) {
            while (arr[i] < pivot)
                i++;
            while (arr[j] > pivot && j >= 0)
                j--;
            if (j >= i) {
                swap(arr, i, j);
                i++;
                j--;
            }
        }
        swap(arr, i, fim);
        quickSortRandom(size, arr, ini, i - 1);
        quickSortRandom(size, arr, i + 1, fim);
    }
}


int choosePivot(int32_t arr[], int a, int b, int c) {
    if ((arr[a] > arr[b]) ^ (arr[a] > arr[c])) 
        return a;
    else if ((arr[b] < arr[a]) ^ (arr[b] < arr[c])) 
        return b;
    else
        return c;
}

void quickSortMedian(unsigned long long size, int32_t arr[size], int ini, int fim) {
    int i, j;
    if (fim - ini < 2) {
        if (fim - ini == 1) {
            if (arr[ini] > arr[fim]) {
                swap(arr, ini, fim);
            }
        }
    } else {
        int med = choosePivot(arr, ini, (ini+fim)/2, fim);
        swap(arr, fim, med);
        int32_t pivot = arr[fim];
        i = ini;
        j = fim - 1;
        while (j >= i) {
            while (arr[i] < pivot && i < j)
                i++;
            while (arr[j] > pivot && j >= 0)
                j--;
            if (j >= i) {
                swap(arr, i, j);
                i++;
                j--;
            }
        }
        swap(arr, i, fim);
        quickSortMedian(size, arr, ini, i - 1);
        quickSortMedian(size, arr, i + 1, fim);
    }
}

void descer(int arr[], int i, long long n) {
    int j = 2 * i;  
    if (j < n) {
        if(arr[j+1] > arr[j]) j++;
        if (arr[i] < arr[j]) {
            swap(arr, j, i);
            descer(arr, j, n);
        } 
    }
}

// Função principal para ordenar um array usando o Heap Sort
void heapSort(long long size, int32_t arr[size]) {
    for (int i = size/2; i >= 0; i--){
        descer(arr, i, size);
    }
    long long m = size;
    while (m > 0) {
        swap(arr, 0, m);
        m--;
        descer(arr, 0, m);
    }
}   