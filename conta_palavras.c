#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 100
#define HASH_SIZE 1000

// Estrutura para armazenar cada palavra e sua contagem
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
void countWordsFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return;
    }

    char word[MAX_WORD_LEN];
    while (fscanf(file, "%99s", word) == 1) {
        cleanWord(word);
        if (strlen(word) > 0) {
            insertWord(word);
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
    if (argc < 2) {
        printf("Uso: %s <arquivo_de_texto>\n", argv[0]);
        return 1;
    }

    countWordsFromFile(argv[1]);
    printWordCounts();
    freeMemory();
    return 0;
}
