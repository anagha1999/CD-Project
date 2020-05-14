# CD-Project
+ Instructions to run:
	+ ```./run.sh```
	+ ```./a.out < test_files/_test.py_``` where _test.py_ is a test file of your choice in test_files/ directory
		+ this will generate optimized intermediate code
	+ python3 asm.py 
		+ this will generate the corresponding assembly code	

+ Following phases are included:
	+ Lexical Analysis
	+ Syntax Analysis/Parsing
	+ Semantic Analysis/AST Generation
	+ Intermediate Code Generation (Three Address Code)
	+ Code Optimization:
		+ Dead Code Elimination
		+ Common Sub-Expression Elimination
	+ Assembly Code Generation

+ Constructs Handled:
	+ If-Elif-Else
	+ While Loop
	+ For loop
		+ ```for i in z``` (where z = list,string...)
		+ ```for i in range``` (n1,n2)
	+ Functions

+ References: 
	+ https://github.com/Shruthi2512/C_compiler_front_end-
	+ https://github.com/2020saurav/py-codegen/tree/master/src
	+ https://github.com/luckysahani/Compiler-Assignment-3-and-4/tree/master/Ass4
			