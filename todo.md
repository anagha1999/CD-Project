# For assemble code generation

+ Conversion from TAC to ASM to do 
 + in, 
 + or, and
 + Print, import


+ Conversion from TAC to ASM done
 + =, +, -, <, <=, >, >=, ==, !=
 + If False
 + List Indexing
 + goto
 + Labels
 + List Declaration
 + Handle Recursion (need to make changes to yacc file)
 + BeginF, EndF, Param, Call

+ Doubts:
 + How to use $sp, $fp to handle function declarations, calls
 + Should we have a fixed set of registers?
 + How to handle print/import statements? Do we use syscall? Should the value be loaded into a special register?
