%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdarg.h>
	extern int yylineno;
	extern int depth;
	extern int top();
	extern int pop();
	int currentScope = 1, previousScope = 1;
	
	int *arrayScope = NULL;

	typedef struct AST
	{ char lexeme[100];
	int NumChild;
	struct AST **child;
	}AST_node;

	typedef struct record
	{
		char *type;
		char *name;
		int decLineNo;
		int lastUseLine;
	} record;

	typedef struct STable
	{
		int no;
		int noOfElems;
		int scope;
		record *Elements;
		int Parent;
		
	} STable;

	STable *symbolTables = NULL;
	int sIndex = -1;
	
	int power(int base, int exp)
	{
		int i =0, res = 1;
		for(i; i<exp; i++)
		{
			res *= base;
		}
		return res;
	}
	
	void updateCScope(int scope)
	{
		currentScope = scope;
	}
	
	void resetDepth()
	{
		while(top()) pop();
		depth = 10;
	}
	
	int scopeBasedTableSearch(int scope)
	{
		int i = sIndex;
		for(i; i > -1; i--)
		{
			if(symbolTables[i].scope == scope)
			{
				return i;
			}
		}
		return -1;
	}
	void initNewTable(int scope)
	{
		arrayScope[scope]++;
		sIndex++;
		symbolTables[sIndex].no = sIndex;
		symbolTables[sIndex].scope = power(scope, arrayScope[scope]);
		symbolTables[sIndex].noOfElems = 0;		
		symbolTables[sIndex].Elements = (record*)calloc(20, sizeof(record));
		
		symbolTables[sIndex].Parent = scopeBasedTableSearch(currentScope); 
	}
	
	void init()
	{
		symbolTables = (STable*)calloc(100, sizeof(STable));
		arrayScope = (int*)calloc(10, sizeof(int));
		initNewTable(1);
		
	}
	
	
	int searchRecordInScope(const char* type, const char *name, int index)
	{
		int i =0;
		for(i=0; i<symbolTables[index].noOfElems; i++)
		{
			if((strcmp(symbolTables[index].Elements[i].type, type)==0) && (strcmp(symbolTables[index].Elements[i].name, name)==0))
			{
				return i;
			}	
		}
		return -1;
	}
		
	void modifyRecordID(const char *type, const char *name, int lineNo, int scope)
	{
		int check_error = 0;
		int i =0;
		int index = scopeBasedTableSearch(scope);
		//printf("No Of Elems : %d\n", symbolTables[index].noOfElems);
		if(index==0) //WHEN is index actuallly =0? when you reach the outer-most scope 
		{
			for(i=0; i<symbolTables[index].noOfElems; i++)
			{
				
				if(strcmp(symbolTables[index].Elements[i].type, type)==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
				{
					symbolTables[index].Elements[i].lastUseLine = lineNo;
					return;
				}	
			}
			//printf("Identifier '%s' at line %d Not Declared\n", name, yylineno);
			//printSTable();
			check_error =1;
			//exit(1);
		}
		
		for(i=0; i<symbolTables[index].noOfElems; i++)
		{
			//printf("\t%d Name: %s\n", i, symbolTables[index].Elements[i].name);
			if(strcmp(symbolTables[index].Elements[i].type, type)==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
			{
				symbolTables[index].Elements[i].lastUseLine = lineNo;
				return;
			}	
		}
		//printf("Parent : %d\n", symbolTables[index].Parent);
		return modifyRecordID(type, name, lineNo, symbolTables[symbolTables[index].Parent].scope);
	}
	
	void insertRecord(const char* type, const char *name, int lineNo, int scope)
	{ 
		int FScope = power(scope, arrayScope[scope]);
		int index = scopeBasedTableSearch(FScope);
		int recordIndex = searchRecordInScope(type, name, index);
		//printf("rIndex : %d, Name : %s\n", recordIndex, name);
		if(recordIndex==-1) //record doesnt exist in the scope
		{
			
			symbolTables[index].Elements[symbolTables[index].noOfElems].type = (char*)calloc(30, sizeof(char));
			symbolTables[index].Elements[symbolTables[index].noOfElems].name = (char*)calloc(20, sizeof(char));
		
			strcpy(symbolTables[index].Elements[symbolTables[index].noOfElems].type, type);	
			strcpy(symbolTables[index].Elements[symbolTables[index].noOfElems].name, name);
			symbolTables[index].Elements[symbolTables[index].noOfElems].decLineNo = lineNo;
			symbolTables[index].Elements[symbolTables[index].noOfElems].lastUseLine = lineNo;
			symbolTables[index].noOfElems++;

		}
		else
		{
			symbolTables[index].Elements[recordIndex].lastUseLine = lineNo;
		}
	}
	
	void checkList(const char *name, int lineNo, int scope)
	{
		int index = scopeBasedTableSearch(scope);
		int i;
		if(index==0)
		{
			
			for(i=0; i<symbolTables[index].noOfElems; i++)
			{
				
				if(strcmp(symbolTables[index].Elements[i].type, "ListTypeID")==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
				{
					symbolTables[index].Elements[i].lastUseLine = lineNo;
					return;
				}	

				else if(strcmp(symbolTables[index].Elements[i].name, name)==0)
				{
					printf("Identifier '%s' at line %d Not Indexable\n", name, yylineno);
					//exit(1);

				}

			}
			printf("Identifier '%s' at line %d Not Declared as an Indexable Type\n", name, yylineno);
			//exit(1);
		}
		
		for(i=0; i<symbolTables[index].noOfElems; i++)
		{
			if(strcmp(symbolTables[index].Elements[i].type, "ListTypeID")==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
			{
				symbolTables[index].Elements[i].lastUseLine = lineNo;
				return;
			}
			
			else if(strcmp(symbolTables[index].Elements[i].name, name)==0)
			{
				printf("Identifier '%s' at line %d Not Indexable\n", name, yylineno);
				//exit(1);

			}
		}
		
		return checkList(name, lineNo, symbolTables[symbolTables[index].Parent].scope);

	}
	
	void printSTable()
	{
		int i = 0, j = 0;
		
		printf("\n\n----------------------------All Symbol Tables----------------------------");
		printf("\nScope\tName\tType\t\tDeclaration\tLast Used Line\n");
		for(i=0; i<=sIndex; i++)
		{
			for(j=0; j<symbolTables[i].noOfElems; j++)
			{
				printf("(%d, %d)\t%s\t%s\t%d\t\t%d\n", symbolTables[i].Parent, symbolTables[i].scope, symbolTables[i].Elements[j].name, symbolTables[i].Elements[j].type, symbolTables[i].Elements[j].decLineNo,  symbolTables[i].Elements[j].lastUseLine);
			}
		}
	}
	
	void freeAll()
	{
		int i = 0, j = 0;
		for(i=0; i<=sIndex; i++)
		{
			for(j=0; j<symbolTables[i].noOfElems; j++)
			{
				free(symbolTables[i].Elements[j].name);
				free(symbolTables[i].Elements[j].type);	
			}
			free(symbolTables[i].Elements);
		}
		free(symbolTables);
	}

	//Functions for AST
	struct AST* make_for_node(char* root, AST_node* child1, AST_node* child2, AST_node* child3, AST_node* child4);
	struct AST * make_node(char*, AST_node*, AST_node*);
	struct AST* make_leaf(char* root);
	void AST_print(struct AST *t);

	void AST_print(struct AST *t)
	{
		static int ctr=0;
		//printf("\ninside print tree\n");
		int i;
		//printf("Current node = %s\n", t->lexeme);
		if(t->NumChild==0) 
			return;

		struct AST *t2=t;
		printf("\n%s  -->",t2->lexeme);
		for(i=0;i<t2->NumChild;++i) 
		{
			printf("%s ",t2->child[i]->lexeme);
			
		}
		for(i=0;i<t2->NumChild;++i)
		{
			AST_print(t->child[i]);
		}

				
	}



	struct AST* make_node(char* root, AST_node* child1, AST_node* child2)
	{
		printf("Creating new node\n");
		struct AST * node = (struct AST*)malloc(sizeof(struct AST));
		node->child = (struct AST**)malloc(2*sizeof(struct AST *));
		node->NumChild = 2;//
		strcpy(node->lexeme,root);
		//printf("Copied lexeme\n");
		//printf("%s\n",node->lexeme);
		//printf("Children = %s  %s\n", child1->lexeme, child2->lexeme);
		node->child[0] = child1;
		node->child[1] = child2;
		return node;
	}

	struct AST* make_for_node(char* root, AST_node* child1, AST_node* child2, AST_node* child3, AST_node* child4)
	{
		//printf("Creating new node\n");
		struct AST * node = (struct AST*)malloc(sizeof(struct AST));
		node->child = (struct AST**)malloc(4*sizeof(struct AST *));
		node->NumChild = 4;
		strcpy(node->lexeme,root);
		node->child[0] = child1;
		node->child[1] = child2;
		node->child[2] = child3;
		node->child[3] = child4;
		return node;
	}


	struct AST* make_leaf(char* root)
	{
		//printf("Creating new leaf ");
		struct AST * node = (struct AST*)malloc(sizeof(struct AST));
		strcpy(node->lexeme,root);
		//printf("%s\n",node->lexeme);
		node->NumChild = 0;
		node->child = NULL;
		return node;
	}

%}

%locations

%union { char *text; int depth; struct AST *node; };
%type <node> StartParse StartDebugger  args_list start_suite suite end_suite func_call finalStatements arith_exp bool_exp term constant basic_stmt cmpd_stmt func_def list_index import_stmt pass_stmt break_stmt print_stmt if_stmt elif_stmts else_stmt while_stmt return_stmt assign_stmt bool_term bool_factor range_stmt for_stmt

%token T_EndOfFile  T_Number  T_Cln T_NL  T_IN T_NEQ T_EQ T_GT T_LT T_EGT T_ELT T_Or T_And  ID ND DD T_String Trip_Quote  T_Import   T_MN T_PL T_DV T_ML T_OP T_CP T_OB T_CB T_Def T_Comma T_Range T_List
%token <text> T_ID T_EQL T_LEN T_True T_False T_Not T_Pass T_Break T_Return T_Print T_If T_Elif T_Else T_For T_While
%left T_PL T_MN
%left T_ML T_DV
%left T_OP T_CP
%right T_EQL                                       

%nonassoc T_If
%nonassoc T_Elif
%nonassoc T_Else

 
 

%%
StartDebugger : {init();} StartParse T_EndOfFile {printf("\nValid Python Syntax\n"); 
printf("\n\n---------------------------------AST-----------------------------------"); AST_print($2);   
printSTable(); freeAll(); exit(0);} ;

constant : T_Number {insertRecord("Constant", $<text>1, @1.first_line, currentScope);
					  $$ = make_leaf($<text>1);}
		 | T_String {insertRecord("Constant", $<text>1, @1.first_line, currentScope);
		 			  $$ = make_leaf($<text>1);};

term : T_ID {
			$$=make_leaf($1);
			modifyRecordID("Identifier", $<text>1, @1.first_line, currentScope);
			} 
	   | constant { $$=$1;} 
	   | list_index{$$=$1;};

list_index : T_ID T_OB T_Number T_CB {
									  checkList($<text>1, @1.first_line, currentScope);
									  $$ = make_node("ListIndex", make_leaf($<text>1), make_leaf($<text>3));
									  };

StartParse : T_NL StartParse {$$=$2;}
			| finalStatements T_NL {resetDepth();} StartParse {$$ = make_node("Start", $1, $4);} 
			| finalStatements T_NL {$$=$1;};

basic_stmt : pass_stmt {$$=$1;} 
			| break_stmt {$$=$1;} 
			| import_stmt {$$=$1;} 
			| assign_stmt {$$=$1;} 
			| arith_exp {$$=$1;} 
			| bool_exp {$$=$1;} 
			| print_stmt {$$=$1;}
			| return_stmt {$$=$1;}
			| range_stmt ; 

arith_exp :  term {$$=$1;}
			| arith_exp  T_PL  arith_exp {$$ = make_node("+",$1, $3);}
			| arith_exp  T_MN  arith_exp {$$ = make_node("-",$1, $3);}
			| arith_exp  T_ML  arith_exp {$$ = make_node("/",$1, $3);}
 			| arith_exp  T_DV  arith_exp {$$ = make_node("*",$1, $3);}
 			| T_MN term {$$ = make_node("temp", make_leaf("-"), $2);}
 			| T_OP arith_exp T_CP {$$=$2;};

		    
range_stmt:	  T_Range T_OP T_Number T_CP{insertRecord("Identifier", $<text>1, @1.first_line, currentScope);}
			| T_Range T_OP T_Number T_Comma T_Number T_CP{insertRecord("Identifier", $<text>1, @1.first_line, currentScope);}


bool_exp : bool_term T_Or bool_term {$$ = make_node("or", $1, $3);}
         | arith_exp T_LT arith_exp {$$ = make_node("<", $1, $3);}
         | bool_term T_And bool_term {$$ = make_node("and", $1, $3);}
         | arith_exp T_GT arith_exp {$$ = make_node(">",$1, $3);}
         | arith_exp T_ELT arith_exp {$$ = make_node("<=", $1, $3);}
         | arith_exp T_EGT arith_exp {$$ = make_node(">=",$1, $3);}
         | arith_exp T_IN T_ID {checkList($<text>3, @3.first_line, currentScope); 
         $$ = make_node("in", $1, make_leaf("$<text>3"));}
         | bool_term {$$=$1;}; 

bool_term : bool_factor {$$ = $1;}
          | arith_exp T_EQ arith_exp {$$ = make_node("==", $1, $3);}
          | T_True {insertRecord("Constant", "True", @1.first_line, currentScope); $$ = make_leaf("True");}
          | T_False {insertRecord("Constant", "False", @1.first_line, currentScope); $$ = make_leaf("False");}; 
          
bool_factor : T_Not bool_factor {$$ = make_node("!", make_leaf("end"), $2);}
            | T_OP bool_exp T_CP {$$ = $2;}; 

import_stmt : T_Import T_ID {insertRecord("PackageName", $<text>2, @2.first_line, currentScope); 
							  $$ = make_node("import_stmt", make_leaf("import"), make_leaf($<text>2) );};

pass_stmt   : T_Pass {$$=make_leaf("pass");};
break_stmt  : T_Break {$$=make_leaf("break");};
return_stmt : T_Return {$$=make_leaf("return");}; 

assign_stmt : T_ID T_EQL arith_exp 
			{
			  insertRecord("Identifier", $<text>1, @1.first_line, currentScope); 
			  //$1 = make_leaf($<text>1);
			  $$=make_node("=",make_leaf($<text>1),$3);
			  }  
			 |T_ID T_EQL bool_exp {insertRecord("Identifier", $<text>1, @1.first_line, currentScope);}   
			 |T_ID  T_EQL func_call {insertRecord("Identifier", $<text>1, @1.first_line, currentScope);} 
			 |T_ID T_EQL T_OB T_CB {insertRecord("ListTypeID", $<text>1, @1.first_line, currentScope);} ;
	      
print_stmt : T_Print T_OP T_String T_CP {$$=make_node("print_stmt", make_leaf($<text>3), make_leaf("NULL"));};

finalStatements : basic_stmt 
				| cmpd_stmt 
				| func_def ;

cmpd_stmt : if_stmt {$$ = $1;} 
			| while_stmt {$$ = $1;}
			| for_stmt {$$ = $1;};


if_stmt : T_If bool_exp T_Cln start_suite {$$= make_node("IF", $2, $4);}  
		| T_If bool_exp T_Cln start_suite elif_stmts {$$= make_node("IF-ELIF-ELSE", make_node("IF", $2, $4), $5);}  ;


elif_stmts : else_stmt{$$= $1;} 
			| T_Elif bool_exp T_Cln start_suite elif_stmts {$$ = make_for_node("ELIF", $2, $4, $5, make_leaf("NULL"));} ;

else_stmt : T_Else T_Cln start_suite {$$ = make_node("ELSE", $3, make_leaf("NULL"));} ;
for_stmt: T_For bool_exp T_Cln start_suite {$$ =make_node($1, $2, $4);};
while_stmt : T_While bool_exp T_Cln start_suite {$$ =make_node("WHILE", $2, $4);}; 

start_suite : basic_stmt {$$=$1;} 
			| T_NL ID { initNewTable($<depth>2); updateCScope($<depth>2);} finalStatements suite {$$ =make_node("start_suite",  $4, $5);};
suite : T_NL ND finalStatements suite {$$ = make_node("Next", $3, $4);}
		| T_NL end_suite {$$ = $2;};
end_suite : DD {updateCScope($<depth>1);} finalStatements {$$ = make_node("EndBlock", $3, make_leaf("NULL"));}
		|{$$ = make_leaf("EndBlock"); resetDepth();};

args_list : T_ID T_Comma args_list {$$ = $3;}
		| T_ID {$$ = make_leaf($1);} | {$$ = make_leaf("none");};
func_def : T_Def T_ID {insertRecord("Func_Name", $<text>2, @2.first_line, currentScope);} T_OP args_list T_CP T_Cln start_suite {$$ = make_for_node("Func_def", make_leaf($<text>2), $5, $8, make_leaf("NULL"));};
func_call : T_ID T_OP args_list T_CP {$$ = make_node($1, $3, NULL);};
 
%%

void yyerror(const char *msg)
{
	//printSTable();
	printf("\n\n%s", msg);
	printf("\nSyntax Error at Line %d, Column : %d\n",  yylineno, yylloc.last_column);
	exit(0);
}

