#ifndef EH__ASSEMBLER
#define EH__ASSEMBLER

#include "lexer.h"

#define TAMANHO_ARQUIVO 516
#define OFFSET 2

typedef struct {
    uint16_t arquivo[TAMANHO_ARQUIVO / 2];
    Lexer* lexer;
} Assembler;



#endif