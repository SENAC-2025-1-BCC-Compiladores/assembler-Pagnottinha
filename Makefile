lexer:
	gcc lexer.c -o lexer.exe -pedantic -std=c99 -W -Wall

assembler:
	gcc assembler.c lexer.c -o assembler.exe -pedantic -std=c99 -W -Wall