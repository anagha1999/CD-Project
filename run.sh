lex code.l
yacc -d code.y
gcc lex.yy.c y.tab.c -ll -w