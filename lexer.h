#ifndef EH__LEXER
#define EH__LEXER

#include <inttypes.h>
#include <stdio.h>

#define COMENTARIO ';'
#define TAMANHO_TOKEN 10
#define TAMANHO_MAXIMO_MNEMONICO 3

typedef enum {
    NOP = 0x0,
    STA = 0x10,
    LDA = 0x20,
    ADD = 0x30,
    OR = 0x40,
    AND = 0x50,
    NOT = 0x60,
    JMP = 0x80,
    JN = 0x90,
    JZ = 0xA0,
    HLT = 0xF0,
    DESCONHECIDO = 0xFF // CASO NÃO EXISTA
} mnemonico;

typedef struct instrucao {
    mnemonico mnemonico;
    char token[TAMANHO_TOKEN + 1];
    struct instrucao* prox;
} Instrucao;

typedef struct data {
    char token[TAMANHO_TOKEN + 1];
    char tipo[TAMANHO_TOKEN + 1];
    uint8_t valor;
    int vazio;
    struct data* prox;
} Data;

typedef struct {
    FILE* fp;
    int linha;
    Data* data;
    Instrucao* code;
    uint8_t org;
} Lexer;

typedef struct {
    char texto[TAMANHO_TOKEN + 1];
    char ultimo;
    size_t tamanho;
} Palavra;

Lexer* criarLexer(char* arquivo);
void freeLexer(Lexer* lexer);
void printLexer(Lexer* lexer);

#endif