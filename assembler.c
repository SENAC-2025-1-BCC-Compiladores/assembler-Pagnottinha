#include <stdio.h>
#include <string.h>
#include "assembler.h"

void adicionarData(Assembler* assembler);
uint8_t tamanhoCodigo(Assembler* assembler);
int adicionarCodigo(Assembler* assembler);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Execucao: %s nome_arquivo\n", argv[0]);
        return 1;
    }

    Lexer* lexer = criarLexer(argv[1]);

    if (lexer == NULL) {
        return 1;
    }

    printLexer(lexer);

    Assembler assembler = {{0}, lexer};

    assembler.arquivo[0] = 0x4e03;
    assembler.arquivo[1] = 0x5244;

    adicionarData(&assembler);

    if (!adicionarCodigo(&assembler)) {
        freeLexer(assembler.lexer);
        return 1;
    }

    FILE* fp = fopen("Assembler.mem", "wb");

    if (fp == NULL) {
        freeLexer(assembler.lexer);
        return 1;
    }

    if(!fwrite(assembler.arquivo, 2, TAMANHO_ARQUIVO / 2, fp)) {
        printf("ERRO: falha ao escrever no arquivo.\n");
        freeLexer(assembler.lexer);
        fclose(fp);
        return 1;
    }

    freeLexer(lexer);
    fclose(fp);

    return 0;
}

void adicionarData(Assembler* assembler) {
    uint8_t tamanho = tamanhoCodigo(assembler);
    uint8_t contador = 0;

    Data* aux = assembler->lexer->data;
    while(aux != NULL) {
        uint8_t posicao = (assembler->lexer->org + tamanho + contador) % (TAMANHO_ARQUIVO / 2 - OFFSET) + OFFSET;
        printf("%s %x\n", aux->token, posicao - OFFSET);
        if (!aux->vazio) {
            assembler->arquivo[posicao] = (uint16_t) aux->valor;
        }

        contador++;
        aux = aux->prox;
    }

    return;
}

uint8_t tamanhoCodigo(Assembler* assembler) {
    uint8_t contagem = 0;

    Instrucao* aux = assembler->lexer->code;
    while(aux != NULL) {
        if (aux->token[0] != '\0') {
            contagem++;
        }

        contagem++;
        aux = aux->prox;
    }

    return contagem;
}

int adicionarCodigo(Assembler* assembler) {
    uint8_t tamanho = tamanhoCodigo(assembler);
    uint8_t posicao = assembler->lexer->org;
    Instrucao* aux = assembler->lexer->code;
    
    while(aux != NULL) {
        assembler->arquivo[posicao + OFFSET] = (uint16_t) aux->mnemonico;
        posicao++;

        if (aux->token[0] != '\0') {
            uint8_t contagemData = 0;

            Data* auxData = assembler->lexer->data;
            while(auxData != NULL) {
                if (strcmp(aux->token, auxData->token) == 0) {
                    uint8_t pos = (assembler->lexer->org + tamanho + contagemData) % (TAMANHO_ARQUIVO / 2 - OFFSET);
                    assembler->arquivo[posicao + OFFSET] = (uint16_t) pos;
                    break;
                }

                contagemData++;
                auxData = auxData->prox;
            }

            if (auxData == NULL) {
                char* endptr;
                errno = 0;
                uint16_t valor = strtol(aux->token, &endptr, 0);

                // valor pode ser um hexa ou indefinido (?)
                if (*endptr != '\0') {
                    printf("ERRO: numero desconhecido %s\n", aux->token);
                    return 0;
                }

                if (errno == ERANGE || valor > UINT8_MAX ) {
                    printf("ERRO: numero fora do intervalo possivel %x\n", valor);
                    return 0;
                }
                else {
                    assembler->arquivo[posicao + OFFSET] = (uint16_t) valor;
                }
            }

            posicao++;
        }

        aux = aux->prox;
    }

    return 1;
}