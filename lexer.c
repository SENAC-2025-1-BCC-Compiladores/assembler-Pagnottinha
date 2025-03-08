#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include <string.h>

int pularLinha(Lexer* lexer);
int chegarNoPonto(Lexer* lexer);
Palavra lerPalavra(Lexer* lexer);
int verificarBloco(Lexer* lexer, char* bloco);
int pegarData(Lexer* lexer);
int pegarOrigem(Lexer* lexer);
int pegarInstrucoes(Lexer* lexer);

int VerificaLetra(char ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); }
int VerificaNumero(char ch) { return ch >= '0' && ch <= '9'; }
int VerificaAlfaNumerico(char ch) { return VerificaLetra(ch) || VerificaNumero(ch); }
int VerificaHexadecimal(char ch) { return VerificaNumero(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'); }

/*

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

    freeLexer(lexer);

    return 0;
}

*/

Lexer* criarLexer(char* arquivo) {
    FILE* fp = fopen(arquivo, "r+");

    if (fp == NULL) {
        printf("ERRO: falha ao abrir o arquivo %s\n", arquivo);
        return NULL;
    }

    Lexer* lexer = (Lexer*) malloc(sizeof(Lexer)); //{fp, 1, NULL, NULL, 0};
    lexer->fp = fp;
    lexer->linha = 1;
    lexer->data = NULL;
    lexer->code = NULL;
    lexer->org = 0;

    // procura o "."
    if (!chegarNoPonto(lexer)) {
        freeLexer(lexer);
        return NULL;
    }

    // Pegar a palavra data (o . já foi lido)
    if (!verificarBloco(lexer, "data")) {
        freeLexer(lexer);
        return NULL;
    }
    
    if (!pegarData(lexer)) {
        freeLexer(lexer);
        return NULL;
    }

    if(!verificarBloco(lexer, "code")) {
        freeLexer(lexer);
        return NULL;
    }

    if (!chegarNoPonto(lexer)) {
        freeLexer(lexer);
        return NULL;
    }

    if (!pegarOrigem(lexer)) {
        freeLexer(lexer);
        return NULL;
    }
    
    if (!pegarInstrucoes(lexer)) {
        freeLexer(lexer);
        return NULL;
    }

    return lexer;
}

int pularLinha(Lexer* lexer) {
    char ch;
    while((ch = fgetc(lexer->fp)) != -1) {
        if (ch == '\n') {
            lexer->linha++;
            return 1;
        }
    }

    return 0;
}

int chegarNoPonto(Lexer* lexer) {
    char ch;
    while((ch = fgetc(lexer->fp)) != -1) {
        if (ch == '.') return 1;
        else if (ch == '\n') {
            lexer->linha++;
        }
        else if (ch == COMENTARIO && !pularLinha(lexer)) {
            ch = -1;
            break;
        }
        else if (ch != COMENTARIO && ch != ' ' && ch != '\t') {
            printf("Caractere invalido \'%c\' na linha %d.\n", ch, lexer->linha);
            return 0;
        }
    }

    return 0;
}

int verificarBloco(Lexer* lexer, char* bloco) {
    Palavra palavra = lerPalavra(lexer);
    
    if (strcmp(palavra.texto, bloco) != 0) {
        printf("ERRO: estrutura desconhecida %s (não achado o .%s) na linha %d\n", palavra.texto, bloco, lexer->linha);
        return 0;
    }
    
    if (palavra.ultimo == COMENTARIO && pularLinha(lexer)) {
        printf("ERRO: Fim do arquivo\n");
        return 0;
    }
    else if (palavra.ultimo == '\n') {
        lexer->linha++;
    }
    else if (palavra.ultimo == ' ' || palavra.ultimo == '\t') {
        char ch;
        while((ch = fgetc(lexer->fp)) != -1) {
            if (ch == '\n') {
                lexer->linha++;
                break;
            }
            else if (ch == COMENTARIO && !pularLinha(lexer)) {
                ch = -1;
                break;
            }
            else if (ch != COMENTARIO && ch != ' ' && ch != '\t') {
                printf("Caractere inválido \'%c\' na linha %d.\n", ch, lexer->linha);
                return 0;
            }
        }

        if (ch == -1) {
            printf("ERRO: Fim do arquivo, está incompleto.\n");
            return 0;
        }
    }
    else if (palavra.ultimo != COMENTARIO) {
        printf("ERRO: caractere inválido (\'%c\') depois da estrutura %s %d\n", palavra.ultimo, bloco, lexer->linha);
        return 0;
    }

    return 1;
}

int pegarData(Lexer* lexer) {
    char ch;
    int index;
    Data* nova = NULL;

    while((ch = fgetc(lexer->fp)) != -1) {
        if (ch == COMENTARIO && !pularLinha(lexer)) {
            printf("ERRO: fim do arquivo.\n");
            return 0;
        }
        else if (VerificaLetra(ch)) {
            index = 1;
            nova = (Data*) malloc(sizeof(Data));

            if (nova == NULL) {
                printf("Erro ao alocar na memória\n");
                return 0;
            }

            nova->prox = lexer->data;
            nova->token[0] = ch;

            // pegar nome de variável
            do {
                ch = fgetc(lexer->fp);
                if (ch == COMENTARIO || ch == '\n') {
                    free(nova);
                    return 1;
                }
                else if (ch != ' ' && ch != '\t') {
                    nova->token[index] = ch;
                    index++;
                }
            } while(ch != -1 && ch != '\t' && ch != ' ' && index < TAMANHO_TOKEN);

            if (ch == -1) {
                printf("ERRO: falha ao alocar na memória\n");
                free(nova);
                return 0;
            }

            nova->token[index] = '\0';
            nova->vazio = 0;

            // pegar tipo
            Palavra palavra = lerPalavra(lexer);

            if (palavra.ultimo != ' ' && palavra.ultimo != '\t') {
                printf("ERRO: numero maximo eh %d caracteres\n", TAMANHO_TOKEN);
                free(nova);
                return 0;
            }

            strncpy(nova->tipo, palavra.texto, palavra.tamanho);

            // pegar valor
            palavra = lerPalavra(lexer);

            if (palavra.ultimo != ' ' && palavra.ultimo != '\t' && palavra.ultimo != '\n' && palavra.ultimo != COMENTARIO) {
                printf("ERRO: numero maximo eh %d caracteres\n", TAMANHO_TOKEN); // provavelmente nunca cai nisso
                free(nova);
                return 0;
            }
            
            char* endptr;
            errno = 0;
            uint16_t valor = strtol(palavra.texto, &endptr, 0);

            // valor pode ser um hexa ou indefinido (?)
            if (*endptr != '\0' && palavra.tamanho == 1 && palavra.texto[0] == '?') {
                nova->vazio = 1;
            }
            else if (*endptr != '\0') {
                printf("ERRO: numero desconhecido na linha %d\n", lexer->linha);
                free(nova);
                return 0;
            }

            if (errno == ERANGE || valor > UINT8_MAX ) {
                printf("ERRO: numero fora do intervalo possivel na linha %d\n", lexer->linha);
                free(nova);
                return 0;
            }
            else if (nova->vazio == 0) {
                nova->valor = (uint8_t) valor;
            }

            if (palavra.ultimo == COMENTARIO && !pularLinha(lexer)) {
                printf("ERRO: fim do arquivo.\n");
                return 1;
            }
            else if (palavra.ultimo == '\n') {
                lexer->linha++;
            }
            else if (ch != COMENTARIO) {
                do {
                    ch = fgetc(lexer->fp);
                    if (ch != ' ' && ch != '\t' && ch != COMENTARIO && ch != '\n') {
                        printf("ERRO: caracter invalido na linha %d\n", lexer->linha);
                        free(nova);
                        return 0;
                    }
                } while(ch != -1 && ch != COMENTARIO && ch != '\n');
    
                if (ch == -1) {
                    printf("ERRO: fim do arquivo.\n");
                    free(nova);
                    return 0;
                }

                if (ch == COMENTARIO && !pularLinha(lexer)) {
                    printf("ERRO: fim do arquivo.\n");
                    free(nova);
                    return 0;
                }
                else if (ch == '\n') {
                    lexer->linha++;
                }
            }
            
            lexer->data = nova;
        }
        else if (ch == '\n') lexer->linha++;
        else if (ch == '.') return 1;
        else if (ch != '\t' && ch != ' ') {
            printf("ERRO: caracter invalido na linha %d\n", lexer->linha);
            return 0;
        }
    }

    return 0;
}

Palavra lerPalavra(Lexer* lexer) {
    Palavra palavra;
    palavra.tamanho = 0;
    palavra.ultimo = '\0';

    char ch;

    while((ch = fgetc(lexer->fp)) != -1) {
        if (ch == '\n' || ch == '\t' || ch == ' ' || ch == COMENTARIO || palavra.tamanho == TAMANHO_TOKEN) {
            palavra.ultimo = ch;
            palavra.texto[palavra.tamanho] = '\0';
            return palavra;
        }
        
        palavra.texto[palavra.tamanho] = ch;
        palavra.tamanho++;
    }

    palavra.texto[0] = '\0';

    return palavra;
}

mnemonico PegarMnemonico(char* token) {
    if (strcmp(token, "NOP") == 0) return NOP;
    else if (strcmp(token, "STA") == 0) return STA;
    else if (strcmp(token, "LDA") == 0) return LDA;
    else if (strcmp(token, "ADD") == 0) return ADD;
    else if (strcmp(token, "AND") == 0) return AND;
    else if (strcmp(token, "OR") == 0) return OR;
    else if (strcmp(token, "NOT") == 0) return NOT;
    else if (strcmp(token, "JMP") == 0) return JMP;
    else if (strcmp(token, "JN") == 0) return JN;
    else if (strcmp(token, "JZ") == 0) return JZ;
    else if (strcmp(token, "HLT") == 0) return HLT;
    else return DESCONHECIDO;
}
int NaoPossuiValor(mnemonico m) {
    return m == NOP || m == NOT || m == HLT || m == DESCONHECIDO;
}

int pegarOrigem(Lexer* lexer) {
    Palavra palavra = lerPalavra(lexer);

    if (strcmp(palavra.texto, "org") != 0) {
        printf("ERRO: estrutura errada depois de .code %s (não achado o .org) na linha %d\n", palavra.texto, lexer->linha);
        return 0;
    }
    
    if (palavra.ultimo != ' ' && palavra.ultimo != '\t') {
        printf("ERRO: valor de origem nao achado\n");
        return 0;
    }

    palavra = lerPalavra(lexer);

    char* endptr;
    errno = 0;
    uint16_t valor = strtol(palavra.texto, &endptr, 0);

    // valor pode ser um hexa ou indefinido (?)
    if (*endptr != '\0') {
        printf("ERRO: numero desconhecido na linha %d\n", lexer->linha);
        return 0;
    }

    if (errno == ERANGE || valor > UINT8_MAX ) {
        printf("ERRO: numero fora do intervalo possivel na linha %d\n", lexer->linha);
        return 0;
    }
    else {
        lexer->org = (uint8_t) valor;
    }

    if (palavra.ultimo == '\n') lexer->linha++;
    else if (palavra.ultimo == COMENTARIO && !pularLinha(lexer)) {
        printf("ERRO: Fim do arquivo\n");
        return 0;
    }
    else if (palavra.ultimo != COMENTARIO && palavra.ultimo != ' ' && palavra.ultimo != '\t') {
        printf("ERRO: caractere invalido \'%c\' na linha %d\n", palavra.ultimo, lexer->linha);
        return 0;
    }

    return 1;
}

int pegarInstrucoes(Lexer* lexer) {
    char ch;
    int index;
    Instrucao* nova = NULL;

    while((ch = fgetc(lexer->fp)) != -1) {
        if (ch == COMENTARIO && !pularLinha(lexer)) {
            printf("ERRO: Fim do arquivo\n");
            return 0;
        }
        else if (VerificaLetra(ch)) {
            nova = (Instrucao*) malloc(sizeof(Instrucao));

            if (nova == NULL) {
                printf("Erro ao alocar na memória\n");
                return 0;
            }

            nova->prox = NULL;

            Instrucao* aux = lexer->code;

            while(aux != NULL && aux->prox != NULL) {
                aux = aux->prox;
            }

            char mnemonico[TAMANHO_MAXIMO_MNEMONICO + 1];
            mnemonico[0] = ch;

            index = 1;
            // pegar mnemonico
            do {
                ch = fgetc(lexer->fp);
                if (VerificaLetra(ch)) {
                    mnemonico[index] = ch;
                    index++;
                }
                else if (ch != -1 && ch != ' ' && ch != '\t') {
                    free(nova);
                    printf("ERRO: Mnemonico invalido na linha %d\n", lexer->linha);
                    return 0;
                }
            } while(ch != -1 && ch != '\t' && ch != ' ' && index < TAMANHO_MAXIMO_MNEMONICO + 1);

            mnemonico[index] = '\0';

            nova->mnemonico = PegarMnemonico(mnemonico);

            if (nova->mnemonico == DESCONHECIDO) {
                printf("ERRO: Mnemonico desconhecido na linha %d!\n", lexer->linha);
                free(nova);
                return 0;
            }

            if (!NaoPossuiValor(nova->mnemonico)) {
                // pegar valor
                Palavra palavra = lerPalavra(lexer);

                if (palavra.tamanho == 0) {
                    printf("ERRO: o mnemonico %s precisa de um complemento\n", mnemonico);
                    free(nova);
                    return 0;
                }

                strncpy(nova->token, palavra.texto, palavra.tamanho);

                if (palavra.ultimo == COMENTARIO && !pularLinha(lexer)) {
                    printf("ERRO: Fim do arquivo\n");
                    return 1;
                }
                else if (palavra.ultimo == '\n') {
                    lexer->linha++;
                }
                else if (palavra.ultimo == ' ' || palavra.ultimo == '\t') {
                    do {
                        ch = fgetc(lexer->fp);
                        if (ch != ' ' && ch != '\t' && ch != COMENTARIO && ch != '\n') {
                            printf("ERRO: %c muitos argumentos na linha %d\n", ch, lexer->linha);
                            free(nova);
                            return 0;
                        }
                    } while(ch != -1 && ch != COMENTARIO && ch != '\n');
        
                    if (ch == -1) {
                        printf("ERRO: Fim do arquivo\n");
                        free(nova);
                        return 0;
                    }

                    if (ch == COMENTARIO && !pularLinha(lexer)) {
                        printf("ERRO: Fim do arquivo\n");
                        free(nova);
                        return 0;
                    }
                    else if (ch == '\n') {
                        lexer->linha++;
                    }
                }
                else if (palavra.ultimo != COMENTARIO) {
                    printf("ERRO: sei la\n");
                    free(nova);
                    return 0;
                }
            }
            else {
                nova->token[0] = '\0';
            }
            
            if (aux == NULL) {
                lexer->code = nova;
            }
            else {
                aux->prox = nova;
            }
        }
        else if (ch == '\n') lexer->linha++;
        else if (ch != '\t' && ch != ' ') return 0;
    }

    return 1;
}

void printLexer(Lexer* lexer) {
    printf("Linha: %d\n", lexer->linha);

    printf("Data:\n");
    Data* auxData = lexer->data;
    while (auxData != NULL) {
        if (auxData->vazio) {
            printf("- %s %s %s\n", auxData->token, auxData->tipo, "Sem valor");
        }
        else {
            printf("- %s %s %2x\n", auxData->token, auxData->tipo, auxData->valor);
        }
        auxData = auxData->prox;
    }

    printf("Code (%x):\n", lexer->org);
    Instrucao* auxInstrucao = lexer->code;
    while (auxInstrucao != NULL) {
        printf("- %x %s\n", auxInstrucao->mnemonico, auxInstrucao->token);
        auxInstrucao = auxInstrucao->prox;
    }
}

void freeLexer(Lexer* lexer) {
    if (lexer == NULL) return;

    while (lexer->data != NULL) {
        Data* aux = lexer->data->prox;
        free(lexer->data);
        lexer->data = aux;
    }

    while (lexer->code != NULL) {
        Instrucao* aux = lexer->code->prox;
        free(lexer->code);
        lexer->code = aux;
    }

    if (lexer->fp)
        fclose(lexer->fp);

    free(lexer);
}
