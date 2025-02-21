#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include <omp.h>

#define MAX_WORD_LEN 100
#define HASH_SIZE 1000
#define WORD_BYTES 5
typedef struct WordNode {
    char word[MAX_WORD_LEN];
    int count;
    struct WordNode *next;
} WordNode;

// Tabela hash para armazenar palavras
WordNode *hashTable[HASH_SIZE];

// Função de hash simples (djb2)
unsigned int hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

// Insere ou atualiza a contagem de uma palavra na tabela hash
void insertWord(const char *word) {
    unsigned int index = hash(word);
    WordNode *node = hashTable[index];

    while (node) {
        if (strcmp(node->word, word) == 0) {
            node->count++;
            return;
        }
        node = node->next;
    }

    // Se a palavra não existe, cria um novo nó
    WordNode *newNode = (WordNode *)malloc(sizeof(WordNode));
    strcpy(newNode->word, word);
    newNode->count = 1;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

// Remove pontuação e converte para minúsculas
void cleanWord(char *word) {
    int i, j = 0;
    for (i = 0; word[i]; i++) {
        if (isalnum(word[i])) {
            word[j++] = tolower(word[i]);
        }
    }
    word[j] = '\0';
}

// Lê o arquivo e conta as palavras
void countWordsFromFile(const char *filename, int rank, int size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return;
    }

    // Determina o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Calcula o número total de palavras no arquivo
    long totalWords = fileSize / WORD_BYTES;
    // Divide as palavras entre os processos MPI
    long wordsPerProcess = totalWords / size;
    long startWord = rank * wordsPerProcess;
    long endWord = (rank == size - 1) ? totalWords : startWord + wordsPerProcess;

    // Move o ponteiro do arquivo para a posição inicial do processo
    fseek(file, startWord * WORD_BYTES, SEEK_SET);

    // Buffer para armazenar as palavras lidas por este processo
    char word[WORD_BYTES + 1]; // +1 para o terminador nulo

    // Paraleliza o processamento das palavras com OpenMP
    #pragma omp parallel for private(word)
    for (long i = startWord; i < endWord; i++) {
        fseek(file, i * WORD_BYTES, SEEK_SET); // Posiciona o ponteiro do arquivo
        fread(word, 1, WORD_BYTES, file); // Lê a palavra
        word[WORD_BYTES] = '\0'; // Adiciona terminador nulo

        cleanWord(word); // Limpa a palavra (remove pontuação e converte para minúsculas)
        if (strlen(word) > 0) {
            #pragma omp critical
            insertWord(word); // Insere a palavra na tabela hash
        }
    }

    fclose(file);
}

// Função de comparação para ordenação
int compareWords(const void *a, const void *b) {
    WordNode *wa = *(WordNode **)a;
    WordNode *wb = *(WordNode **)b;
    return strcmp(wa->word, wb->word);
}

// Exibe a contagem de palavras ordenada
void printWordCounts() {
    WordNode *wordArray[HASH_SIZE];
    int count = 0;

    for (int i = 0; i < HASH_SIZE; i++) {
        WordNode *node = hashTable[i];
        while (node) {
            wordArray[count++] = node;
            node = node->next;
        }
    }

    qsort(wordArray, count, sizeof(WordNode *), compareWords);

    for (int i = 0; i < count; i++) {
        printf("%s: %d\n", wordArray[i]->word, wordArray[i]->count);
    }
}

// Libera a memória
void freeMemory() {
    for (int i = 0; i < HASH_SIZE; i++) {
        WordNode *node = hashTable[i];
        while (node) {
            WordNode *temp = node;
            node = node->next;
            free(temp);
        }
    }
}

// Função principal
int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) {
            printf("Uso: %s <arquivo_de_texto>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    // Inicializa a tabela hash
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i] = NULL;
    }

    // Cada processo MPI processa uma parte do arquivo
    countWordsFromFile(argv[1], rank, size);

    // Combina os resultados no processo 0
    if (rank == 0) {
        // Recebe as contagens de palavras dos outros processos
        for (int i = 1; i < size; i++) {
            for (int j = 0; j < HASH_SIZE; j++) {
                WordNode *node;
                int count;
                MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int k = 0; k < count; k++) {
                    node = (WordNode *)malloc(sizeof(WordNode));
                    MPI_Recv(node->word, MAX_WORD_LEN, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&node->count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    insertWord(node->word);
                    free(node);
                }
            }
        }
        printWordCounts();
    } else {
        // Envia as contagens de palavras para o processo 0
        for (int i = 0; i < HASH_SIZE; i++) {
            WordNode *node = hashTable[i];
            int count = 0;
            while (node) {
                count++;
                node = node->next;
            }
            MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            node = hashTable[i];
            while (node) {
                MPI_Send(node->word, MAX_WORD_LEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
                MPI_Send(&node->count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                node = node->next;
            }
        }
    }

    freeMemory();
    MPI_Finalize();
    return 0;
}


