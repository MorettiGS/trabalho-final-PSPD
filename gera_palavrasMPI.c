#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PALAVRAS_BASE 71 // Número de palavras na lista base

// Lista base de palavras
const char *palavrasBase[NUM_PALAVRAS_BASE] = {
    "amor", "solo", "luau" "mara","neve", "fogo",
     "vale", "riso", "lago",  "vila", "ilha", "moto", "trem", "boxe", "judô", "tela",
     "arte",
};

// Função para gerar palavras aleatórias
void gerarPalavras(int quantidade, FILE *arquivo) {
    srand(time(NULL)); // Inicializa o gerador de números aleatórios
    for (int i = 0; i < quantidade; i++) {
        // Escolhe uma palavra aleatória da lista base
        int index = rand() % NUM_PALAVRAS_BASE;
        fprintf(arquivo, "%s\n", palavrasBase[index]); // Escreve a palavra no arquivo
    }
}

// Função para criar o arquivo
void criarArquivo(const char *nomeArquivo, int quantidade) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // Abre o arquivo para escrita
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }
    gerarPalavras(quantidade, arquivo); // Gera e escreve as palavras
    fclose(arquivo); // Fecha o arquivo
}

int main() {
    int quantidade = 20000000; // Quantidade de palavras a gerar
    criarArquivo("words.txt", quantidade); // Cria o arquivo words.txt
    printf("Arquivo 'words.txt' criado com sucesso!\n");
    return 0;
}



