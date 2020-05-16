# VIDEO

## Intro - shar


## Lexical Analysis  - aish
 + token generation (number, string, OB, OP, NL..)
 + removing comments 
 + handling indentation


## Syntax Analysis/Parsing - anagha
 + grammar (show picture of grammar file, talk about conflicts and minimization)
 + symbol table (diff variable types, last line/declared line, scope, hash table scheme)

## Semantic Analysis/AST Generation - shar
 + bottom up, preorder traversal
 + priority/association (922 code line)
 + make node functions
 + AST.py in detail
 + briefly explain AST2.py (ELIF has 3 children)

## Intermediate Code Generation (Three Address Code) - shar
 + quads structure (detail)
 + explain if-elif-else in detail
 + while loop (similar to if-elif-else)
 + for loop: (talk about how we assign the iterator variable initial and limit value)
 	+ for i in range (n1,n2) (detail)
	+ for i in z (where z = list,string...)
	
## Code Optimization: - Aish
 + deadcode elimination
 + CSE

## Error handling/recovery: - Aish
 + identifier not declared
 + list not indexable
 + show in yacc

## Assembly Code Generation: - anagha
 + linear scan allocation 
 + handling functions 
