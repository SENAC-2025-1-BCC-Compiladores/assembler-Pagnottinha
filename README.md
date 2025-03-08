[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/FLG6_3H5)
Gustavo Pagnotta Faria

## Descrição
Este projeto implementa um montador (assembler) para uma linguagem assembly simples. Ele processa um arquivo de entrada contendo código assembly, analisa sua estrutura, converte as instruções para código de máquina e gera um arquivo binário de saída.

## Estrutura do Projeto
- `Makefile`: Arquivo para compilação automática do projeto.
- `assembler.c`: Implementação do montador, que processa as instruções e gera o código de máquina.
- `lexer.c`: Implementação do analisador léxico, que interpreta o arquivo de entrada e separa as palavras-chave e símbolos.
- `assembler.h`, `lexer.h`: Headers correspondentes para a definição de funções e estruturas de dados.

## Formato Assembly
O arquivo deverá possuir 3 estruturas na sequência:
- `.data`: onde será declarado as variáveis. O formato deve ser `(NOME_VARIAVEL) DB (VALOR)` e separados por uma quebra de linha (enter).
    - `NOME_VARIAVEL`: Deve começar com uma letra e tem tamanho máximo de 10 caracteres.
    - `VALOR`: Pode ser um decimal (0 a 255) ou um hexadecimal (0x0 a 0xFF).
- `.code`: onde ficará o código com as instruções.
    - As instruções devem ficar após da estrutura `.org`. Em que deverá seguir o padrão em cada linha `MNEMÔNICO (VALOR SE TIVER)`. O valor pode ser uma posição da memória ou uma variável. Os mnemônicos possíveis para utilizar são os mesmos do neander.
- `.org`: onde começará o código no arquivo binário.
*Exemplo:* `soma.asm`

## Compilação
Para compilar o projeto, utilize o `Makefile` fornecido. Execute:
```sh
make
```
Isso irá gerar um executável chamado `assembler`.

## Uso
Para executar o montador, use o seguinte comando:
```sh
./assembler nome_arquivo.asm
```
Substitua `nome_arquivo.asm` pelo arquivo contendo o código assembly.

## Saída
O programa gera um arquivo binário `Assembler.mem`, contendo o código de máquina correspondente ao programa assembly fornecido.

## Observações
- Caso ocorram erros na análise léxica ou sintática, mensagens serão exibidas no terminal indicando a linha e a natureza do erro. 
- Não há tratamento para o uso de um mnemônico como variável, mas isso não afeta o funcionamento do montador.
- Não houve um tratamento para que se o código passar do tamanho máximo da memória.

