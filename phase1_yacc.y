%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	
	extern int yylineno;
	extern int depth;
	extern int top();
	extern int pop();
	int currScope = 1, prevScope = 1;
	int check =0;
	int *arrayScope = NULL;
	//single entry into symbol table
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
		currScope = scope;
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
		
		symbolTables[sIndex].Parent = scopeBasedTableSearch(currScope); 
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
		
	/*void modifyRecordID(const char *type, const char *name, int lineNo, int scope)
	{
		int i =0;
		int index = scopeBasedTableSearch(scope);
		//printf("No Of Elems : %d\n", symbolTables[index].noOfElems);
		if(index==0)
		{
			for(i=0; i<symbolTables[index].noOfElems; i++)
			{
				
				if(strcmp(symbolTables[index].Elements[i].type, type)==0 && (strcmp(symbolTables[index].Elements[i].name, name)==0))
				{
					symbolTables[index].Elements[i].lastUseLine = lineNo;
					return;
				}	
			}
			printf("Identifier '%s' at line %d Not Declared\n", name, yylineno);
			exit(1);
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
	}*/
	
	void insertRecord(const char* type, const char *name, int lineNo, int scope)
	{ 
		int FScope = power(scope, arrayScope[scope]); //first scope
		int index = scopeBasedTableSearch(FScope);
		int recordIndex = searchRecordInScope(type, name, index);
		//printf("rIndex : %d, Name : %s\n", recordIndex, name);
		if(recordIndex==-1)
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
	

	
	void printSTable()
	{
		int i = 0, j = 0;
		
		printf("----------------------------SYMBOL TABLE----------------------------");
		printf("\nScope\tName\tType\t\tDeclaration\tLast Used Line\n");
		for(i=0; i<=sIndex; i++)
		{
			for(j=0; j<symbolTables[i].noOfElems; j++)
			{
				printf("(%d)\t%s\t%s\t%d\t\t%d\n", symbolTables[i].scope,symbolTables[i].Elements[j].name, symbolTables[i].Elements[j].type, symbolTables[i].Elements[j].decLineNo,  symbolTables[i].Elements[j].lastUseLine);
			}
		}
	}
	
%}

%union { char *text; int depth; };
%locations
   	  
%token T_EndOfFile T_Return T_Number T_True T_False T_ID T_Print T_Cln T_NL T_EQL T_IN T_NEQ T_EQ T_GT T_LT T_EGT T_ELT T_Or T_And T_Not ID ND DD T_String Trip_Quote T_If T_Elif T_For T_While T_Else T_LEN T_Import T_Break T_Pass T_MN T_PL T_DV T_ML T_OP T_CP T_OB T_CB T_Def T_Comma T_Range T_List

%right T_EQL                                       
%left T_PL T_MN
%left T_ML T_DV

%%
StartDebugger : {init();} StartParse T_EndOfFile {printf("\nValid Python Syntax\n");  printSTable(); check =1;} ;
constant : T_Number {insertRecord("Constant", $<text>1, @1.first_line, currScope);}| T_String {insertRecord("Constant", $<text>1, @1.first_line, currScope);};
term : T_ID| constant;


StartParse : finalStatements T_NL StartParse | finalStatements;

basic_stmt : pass_stmt | break_stmt | import_stmt | assign_stmt | arith_exp | bool_exp | print_stmt | return_stmt | range_stmt ; 
arith_exp :  term | arith_exp  T_PL  arith_exp |
		    arith_exp  T_MN  arith_exp |
		    arith_exp  T_ML  arith_exp |
     	  arith_exp  T_DV  arith_exp | 
		    T_OP arith_exp T_CP ;
		    
range_stmt: T_Range T_OP T_Number T_CP{insertRecord("Identifier", $<text>1, @1.first_line, currScope);}|T_Range T_OP T_Number T_Comma T_Number 
T_CP{insertRecord("Identifier", $<text>1, @1.first_line, currScope);}
list_stmt:T_ID T_IN args_list{insertRecord("Identifier", $<text>1, @1.first_line, currScope);};
len_stmt:T_ID T_IN T_LEN T_OP args_list T_CP {insertRecord("Identifier", $<text>1, @1.first_line, currScope);};
bool_exp : bool_term T_Or bool_term | arith_exp T_LT arith_exp | arith_exp T_GT arith_exp | arith_exp T_ELT arith_exp | arith_exp T_EGT arith_exp | T_ID T_IN range_stmt|list_stmt |len_stmt|bool_term; 
bool_term : bool_factor | T_And bool_factor | arith_exp T_EQ arith_exp | T_True | T_False ;
bool_factor : T_Not bool_factor | T_OP bool_exp T_CP;

import_stmt : T_Import T_ID {insertRecord("library", $<text>2, @2.first_line, currScope);};
pass_stmt : T_Pass ;
break_stmt : T_Break ;
return_stmt : T_Return ;

assign_stmt : T_ID T_EQL arith_exp {insertRecord("Identifier", $<text>1, @1.first_line, currScope);}  | 
	      T_ID T_EQL bool_exp {insertRecord("Identifier", $<text>1, @1.first_line, currScope);}   | 
	      T_ID  T_EQL func_call {insertRecord("Identifier", $<text>1, @1.first_line, currScope);} | 
	      T_ID T_EQL T_OB T_CB {insertRecord("ListTypeID", $<text>1, @1.first_line, currScope);} ;
	      
print_stmt : T_Print T_OP T_String T_CP |
			 T_Print T_OP T_ID T_CP ;

finalStatements : basic_stmt | cmpd_stmt | func_def ;

cmpd_stmt : if_stmt | while_loop| for_loop ;


if_stmt : T_If bool_exp T_Cln start_suite  | T_If bool_exp T_Cln start_suite elif_stmts ;
elif_stmts : else_stmt | T_Elif bool_exp T_Cln start_suite elif_stmts ;
else_stmt : T_Else T_Cln start_suite ;
for_loop: T_For bool_exp T_Cln start_suite;
while_loop : T_While bool_exp T_Cln start_suite; 

start_suite : basic_stmt | T_NL ID {initNewTable($<depth>2); updateCScope($<depth>2);} finalStatements suite;
suite : T_NL ND finalStatements suite | T_NL end_suite;
end_suite : DD {updateCScope($<depth>1);} finalStatements |;

args_list : T_ID T_Comma args_list | T_ID | ;
func_def : T_Def T_ID {insertRecord("Func_Name", $<text>2, @2.first_line, currScope);} T_OP args_list T_CP T_Cln start_suite;
func_call : T_ID T_OP args_list T_CP ;
 
%%

void yyerror(const char *msg)
{
	printf("\nSyntax Error at Line %d, Column : %d\n",  yylineno, yylloc.last_column);
	//if(!check)
		//printSTable();
}

int main()
{
	//printf("Enter the Expression\n");
	yyparse();
	return 0;
}