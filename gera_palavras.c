#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PALAVRAS_BASE 71 // Número de palavras na lista base

// Lista base de palavras
const char *palavrasBase[NUM_PALAVRAS_BASE] = {
    "amor", "feliz", "triste", "sol", "lua", "estrela", "mar", "vento", "chuva", "neve", "fogo", "terra", "ar",
    "montanha", "vale", "rio", "lago", "oceano", "floresta", "deserto", "cidade", "vila", "praia", "ilha", "campo",
    "estrada", "ponte", "túnel", "carro", "ônibus", "bicicleta", "moto", "avião", "barco", "navio", "trem", "metrô",
    "futebol", "basquete", "voleibol", "xadrez", "corrida", "natação", "ginástica", "ciclismo", "boxe", "judô",
    "computador", "internet", "teclado", "mouse", "tela", "impressora", "telefone", "celular", "câmera", "vídeo",
    "música", "dança", "teatro", "cinema", "livro", "poesia", "pintura", "escultura", "fotografia", "arte",
    "matemática", "física", "química", "biologia"
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
    int quantidade = 1000000; // Quantidade de palavras a gerar
    criarArquivo("words.txt", quantidade); // Cria o arquivo words.txt
    printf("Arquivo 'words.txt' criado com sucesso!\n");
    return 0;
}

