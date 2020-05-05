/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "code.y" /* yacc.c:339  */

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdarg.h>



	//--------------------------------BASIC VARIABLE DECLARATIONS----------------------------------------
	extern int yylineno;
	extern int depth;
	extern int top();
	extern int pop();
	int currentScope = 1, previousScope = 1;
	
	int *arrayScope = NULL;

	//quad stuff
	#define MAXQUADS 1000
	typedef struct quad
	{
		char *R;
		char *A1;
		char *A2;
		char *Op;
		int I;
	} quad;
	
	quad* all_quads = NULL;	
	char* tString =NULL, *lString = NULL;
	void printQuads();

	int ICG_opt(); //optimization code
	int ICG_opt2(); //common sub expr eval
	void createCSV(); //Write final ICG to a CSV for assembly code generation


	//-------------------------------------STRUCTURE DEFINITIONS----------------------------------------------
	
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

	//--------------------------------------------------------------------------------------------------------
	STable *symbolTables = NULL;
	int sIndex = -1;
	char *argsList = NULL;
	//--------------------------------------------------------------------------------------------------------
	
	//------------------------------------SYMBOL TABLE FUNCTIONS----------------------------------------------
	void insertRecord(const char* type, const char *name, int lineNo, int scope);

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
		argsList = (char *)malloc(100);
		strcpy(argsList, "");
		symbolTables = (STable*)calloc(100, sizeof(STable));
		all_quads = (quad*)calloc(MAXQUADS, sizeof(quad));
		tString = (char*)calloc(10, sizeof(char));
		lString = (char*)calloc(10, sizeof(char));
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
			printf("\nIdentifier '%s' at line %d Not Declared\n", name, yylineno);

			//Error Recovery
			insertRecord(type, name, lineNo, scope);

			yyerror("Invalid Python Syntax");
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
					printf("\nIdentifier '%s' at line %d Not Indexable or has not been declared as a list\n", name, yylineno);
					insertRecord("ListTypeID", name, lineNo, currentScope);
					yyerror("Invalid Python Syntax");
					return;
					//exit(1);

				}

			}
			printf("\nIdentifier '%s' at line %d Not Declared as an Indexable Type or has not been declared as a list\n", name, yylineno);
			insertRecord("ListTypeID", name, lineNo, currentScope);
			yyerror("Invalid Python Syntax");
			return;
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
				printf("Identifier '%s' at line %d Not Indexable or has not been declared as a list\n", name, yylineno);
				insertRecord("ListTypeID", name, lineNo, currentScope);
				yyerror("Invalid Python Syntax");
				return;
				//exit(1);

			}
		}
		
		return checkList(name, lineNo, symbolTables[symbolTables[index].Parent].scope);

	}

	 void addToList(char *newVal, int flag)
  	{
	  	if(flag==0)
	  	{
			  strcat(argsList, ", ");
			  strcat(argsList, newVal);
			}
			else
			{
				strcat(argsList, newVal);
			}
	    //printf("\n\t%s\n", newVal);
	  }
	  
	void clearArgsList()
	{
	    strcpy(argsList, "");
	}
	
	void printSTable()
	{
		int i = 0, j = 0;
		
		printf("\n\n*************************SYMBOL TABLES**************************");
		printf("\nScope\tName\tType\t\tLine Decl\tLast Used Line\n");
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
		ICG_opt(); 
		printf("\nAfter dead code elimination");

		printQuads();
		printf("\n");
		printf("\nAfter common sub expression elimination");
		ICG_opt2(); //CSE elimination
		printQuads();
		printf("\n");
		createCSV();
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
	//--------------------------------------------------------------------------------------------------------
	//-------------------------------------AST FUNCTIONS/STRUCTS---------------------------------------------
	//--------------------------------------------------------------------------------------------------------
	int tempCount =0; //for ICG - global var that holds '0', '1' etc:-, 
				      //this val. is appended to each 'tempNo' of each node to make it T0, T1 etc:-
	typedef struct AST
	{ 
	int tempNo; //for ICG
	char* nodeType; //for ICG
	char lexeme[100];
	int NumChild;
	struct AST* left;
	struct AST **child;
	}AST_node;

	struct AST* make_for_node(char* root, char* type, AST_node* child1, AST_node* child2, AST_node* child3, AST_node* child4);
	//struct AST * make_node(char*, char*, AST_node*, AST_node*);
	struct AST* make_leaf(char* root, char* type);
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



	struct AST* make_node(char* root, char* type, AST_node* child1, AST_node* child2)
	{
		//printf("Creating new node\n");
		struct AST * node = (struct AST*)malloc(sizeof(struct AST));
		node->child = (struct AST**)malloc(2*sizeof(struct AST *));
		node->NumChild = 2;
		strcpy(node->lexeme,root);
		//printf("Copied lexeme\n");
		//printf("%s\n",node->lexeme);
		//printf("Children = %s  %s\n", child1->lexeme, child2->lexeme);
		node->child[0] = child1;
		node->child[1] = child2;

		//for ICG:
		node->nodeType = (char*)malloc(strlen(type)+1);
		strcpy(node->nodeType,type);
		node->tempNo = tempCount++;
		return node;
	}

	struct AST* make_for_node(char* root, char* type, AST_node* child1, AST_node* child2, AST_node* child3, AST_node* child4)
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

		//for ICG
		node->nodeType = (char*)malloc(strlen(type)+1);
		strcpy(node->nodeType,type);
		node->tempNo = tempCount++;

		return node;
	}


	struct AST* make_leaf(char* root, char* type)
	{
		//printf("Creating new leaf ");
		struct AST * node = (struct AST*)malloc(sizeof(struct AST));
		strcpy(node->lexeme,root);
		//printf("%s\n",node->lexeme);
		node->NumChild = 0;
		node->child = NULL;

		//for ICG
		node->nodeType = (char*)malloc(strlen(type)+1);
		strcpy(node->nodeType,type);
		node->tempNo = tempCount++;

		return node;
	}

	//--------------------------------------------------------------------------------------------------------
	//-------------------------------------ICG FUNCTIONS/STRUCTS---------------------------------------------
	//--------------------------------------------------------------------------------------------------------
	
	int qIndex = 0; //used to index the quads array
	int lIndex = 0; //used to index the labels
	void make_quad(char *Op, char *A1, char *A2, char *R)
	{
		
		all_quads[qIndex].R = (char*)malloc(strlen(R)+1);
		all_quads[qIndex].Op = (char*)malloc(strlen(Op)+1);
		all_quads[qIndex].A1 = (char*)malloc(strlen(A1)+1);
		all_quads[qIndex].A2 = (char*)malloc(strlen(A2)+1);
		
		strcpy(all_quads[qIndex].R, R);
		strcpy(all_quads[qIndex].A1, A1);
		strcpy(all_quads[qIndex].A2, A2);
		strcpy(all_quads[qIndex].Op, Op);
		all_quads[qIndex].I = qIndex;
 
		qIndex++;
		
		return;
	}

	void printQuads()
	{
		printf("\n********************************QUADS*********************************\n");
		int i = 0;
		printf("Line No\t\tOp\t\tArg1\t\tArg2\t\tRes\n");
		for(i=0; i<qIndex; i++)
		{
			if(all_quads[i].I > -1)
				printf("%d\t\t%s\t\t%s\t\t%s\t\t%s\n", all_quads[i].I, all_quads[i].Op, all_quads[i].A1, all_quads[i].A2, all_quads[i].R);
				//create_icg_csv(all_quads[i].Op,all_quads[i].A1, all_quads[i].A2, all_quads[i].R );

		}
		printf("**************************************************************************\n");
	}

	void Xitoa(int num, char *str)
	{
		if(str == NULL)
		{
			 printf("Allocate Memory\n");
		   return;
		}
		sprintf(str, "%d", num);
	}

	char *makeStr(int no, int flag)
	{
		char A[10];
		Xitoa(no, A);
		
		if(flag==1) //if it's a variable
		{
				strcpy(tString, "t");
				strcat(tString, A);
				insertRecord("ICGTempVar", tString, -1, 0);
				return tString;
		}
		else //if it's a label
		{
				strcpy(lString, "L");
				strcat(lString, A);
				insertRecord("ICGTempLabel", lString, -1, 0);
				return lString;
		}
	}

	int checkIfBinOperator(char *Op)
	{
		if((!strcmp(Op, "+")) || (!strcmp(Op, "binary-"))|| (!strcmp(Op, "*")) || (!strcmp(Op, "/")) || (!strcmp(Op, ">=")) || (!strcmp(Op, "<=")) || (!strcmp(Op, "<")) || (!strcmp(Op, ">")) || 
			 (!strcmp(Op, "in")) || (!strcmp(Op, "==")) || (!strcmp(Op, "and")) || (!strcmp(Op, "or")))
			{
				return 1;
			}
			
			else 
			{
				return 0;
			}
	}

	void ICG_main(AST_node* node)
	{
		//printf("\n\nIn ICG_main\n");
		if(node == NULL || (!strcmp(node->nodeType, "")))
		{
			return;
		}

		if(!strcmp(node->nodeType, "BeginBlock") || !strcmp(node->nodeType, "EndBlock") || !strcmp(node->nodeType, "NewLine") || !strcmp(node->nodeType, "Next"))
		{
			ICG_main(node->child[0]);
			ICG_main(node->child[1]);		
			return;	
		}

		if((!strcmp(node->nodeType, "Identifier")) || (!strcmp(node->nodeType, "Constant")))
		{
			//printf("\n%s", node->nodeType);
			printf("t%d = %s\n", node->tempNo, node->lexeme);
			make_quad("=", node->lexeme,  "-", makeStr(node->tempNo, 1));
			return;
		}

		if(!(strcmp(node->nodeType, "Print")))
		{
			ICG_main(node->child[0]);
			printf("Print t%d\n", node->child[0]->tempNo);
			make_quad("Print", makeStr(node->child[0]->tempNo, 1), "-", "-");
		}

		if(!strcmp(node->nodeType, "import"))
		{
			printf("import %s\n", node->child[0]->lexeme);
			make_quad("import", node->child[0]->lexeme, "-", "-");
			return;
		}
			
		
		if(checkIfBinOperator(node->nodeType)==1)
		{
			ICG_main(node->child[0]);
			ICG_main(node->child[1]);
			char *res = (char*)malloc(10);
			char *arg1 = (char*)malloc(10);
			char *arg2 = (char*)malloc(10);
			
			strcpy(res, makeStr(node->tempNo, 1));
			strcpy(arg1, makeStr(node->child[0]->tempNo, 1));
			strcpy(arg2, makeStr(node->child[1]->tempNo, 1));

			printf("t%d = t%d %s t%d\n", node->tempNo, node->child[0]->tempNo, node->lexeme, node->child[1]->tempNo);
			printf("\nTEST: node->lexeme =%s", node->lexeme);
			make_quad(node->lexeme, arg1, arg2, res);
			free(arg1);
			free(arg2);
			free(res);
			return;
		}

		if(!strcmp(node->nodeType, "unary-"))
		{
				ICG_main(node->child[1]);
				char *X1 = (char*)malloc(10);
				char *X2 = (char*)malloc(10);
				strcpy(X1, makeStr(node->tempNo, 1));
				strcpy(X2, makeStr(node->child[1]->tempNo, 1));
				printf("t%d = %s t%d\n", node->tempNo, node->nodeType, node->child[1]->tempNo);
				make_quad(node->nodeType , X2, "-", X1);	
			}
			

		

		if(!strcmp(node->nodeType, "="))
		{
			ICG_main(node->child[1]);
			printf("%s = t%d\n", node->child[0]->lexeme, node->child[1]->tempNo);
			make_quad(node->nodeType, makeStr(node->child[1]->tempNo, 1), "-",  node->child[0]->lexeme);
			return;
		}

		if(!strcmp(node->nodeType, "ListIndex"))
		{
			printf("t%d = %s[%s]\n", node->tempNo, node->child[0]->lexeme, node->child[1]->lexeme);
			make_quad("=[]", node->child[0]->lexeme, node->child[1]->lexeme, makeStr(node->tempNo, 1));
			return;
		}

		if(!strcmp(node->nodeType, "ListDecl"))
		{
			//ICG_main(node->child[1]);
			printf("%s = %s\n", node->child[0]->lexeme, node->child[1]->lexeme);
			make_quad(node->nodeType, node->child[1]->lexeme, "-",  node->child[0]->lexeme);
			return;

		}



	
		if((!strcmp(node->nodeType, "IF")))
		{			
			int temp = lIndex;
			ICG_main(node->child[0]);
			printf("If False t%d goto L%d\n", node->child[0]->tempNo, lIndex);
			make_quad("If False",  makeStr(node->child[0]->tempNo, 1), "-", makeStr(temp, 0));
			lIndex++;
			ICG_main(node->child[1]);
			lIndex--;
			printf("L%d: ", temp);
			lIndex++;
			make_quad("Label", "-", "-", makeStr(temp, 0));
			return;
		}

				
		if((!strcmp(node->nodeType, "IF-ELIF-ELSE")))
		{			

			
			ICG_main(node->child[0]);
			ICG_main(node->child[1]);
			
		}
			
		if((!strcmp(node->nodeType, "ELIF")))
		{

			int temp = lIndex;
			ICG_main(node->child[0]);
			printf("If False t%d goto L%d\n", node->child[0]->tempNo, lIndex);
			make_quad("If False", makeStr(node->child[0]->tempNo, 1), "-", makeStr(temp, 0));				
			ICG_main(node->child[1]);
			printf("goto L%d\n", temp+1);
			make_quad("goto", "-", "-", makeStr(temp+1, 0));
			printf("L%d: ", temp);
			make_quad("Label", "-", "-", makeStr(temp, 0));
			ICG_main(node->child[2]);
			printf("L%d: ", temp+1);
			make_quad("Label", "-", "-", makeStr(temp+1, 0));
			lIndex+=2;

		}
		if(!strcmp(node->nodeType, "ELSE"))
		{
			ICG_main(node->child[0]);
			return;
		}

		if(!strcmp(node->nodeType, "WHILE"))
		{
			int temp = lIndex;
			ICG_main(node->child[0]);
			printf("L%d: If False t%d goto L%d\n", lIndex, node->child[0]->tempNo, lIndex+1);
			make_quad( "Label", "-", "-",makeStr(temp, 0));		
			make_quad("If False", makeStr(node->child[0]->tempNo, 1), "-",makeStr(temp+1, 0) );								
			lIndex+=2;			
			ICG_main(node->child[1]);
			printf("goto L%d\n", temp);
			make_quad("goto", "-", "-", makeStr(temp, 0));
			printf("L%d: ", temp+1);
			make_quad("Label", "-", "-", makeStr(temp+1, 0)); 
			lIndex = lIndex+2;
			return;
		}

		if(!strcmp(node->nodeType, "FOR"))
		{
			int temp = lIndex;

			//printf("%s\n",node->child[0]->nodeType); 
			char* token = strtok(node->child[0]->nodeType, ","); 
  			char rangeStart[10];
  			char rangeEnd[10];
		    int flag =0;
		    while (token != NULL) { 
		    	//printf("token =%s\n",token);
		    	if(flag==0)
		    	{
		    		strcpy(rangeStart,token);
		    		flag=1;
		    	}
		        token = strtok(NULL, ","); 
		        if(token!=NULL)
		        	strcpy(rangeEnd,token);
		    } 
		    //printf("\nrangeStart =%s rangeEnd =%s",rangeStart,rangeEnd);

		    char *loop_var=strtok(node->child[0]->lexeme, " ");
			make_quad("=", rangeStart, "-",  loop_var); //initializing i=0
			printf("%s = %s\n", loop_var, rangeStart);

			make_quad( "Label", "-", "-",makeStr(temp, 0));	//everything in the current loop is under a label
			printf("L%d: ", lIndex);

			make_quad("<", loop_var, rangeEnd, makeStr(node->child[0]->tempNo,1)); //t=i<n
			printf("t%d= %s<%s\n", node->child[0]->tempNo, loop_var, rangeEnd );
			
			make_quad("If False", makeStr(node->child[0]->tempNo, 1), "-",makeStr(temp+1, 0) );
			//if condition is false, goto exit label				
			printf("If False t%d goto L%d\n", node->child[0]->tempNo, lIndex+1);

			lIndex+=2;			
			ICG_main(node->child[1]);

			//increment loop variable
			printf("t%d = %s + 1\n", node->tempNo, loop_var);
			make_quad("+", loop_var, "1", makeStr(node->tempNo,1));
			printf("%s = t%d\n", loop_var, node->tempNo);
			make_quad("=", makeStr(node->tempNo,1), "-",  loop_var);
			
			printf("goto L%d\n", temp); //end of loop
			make_quad("goto", "-", "-", makeStr(temp, 0));

			printf("L%d: ", temp+1);
			make_quad("Label", "-", "-", makeStr(temp+1, 0)); 

			lIndex = lIndex+2;
			return;


		}

		if(!strcmp(node->nodeType, "Func_Name"))
		{
			printf("Begin Function %s\n", node->child[0]->lexeme);
			make_quad("BeginF", node->child[0]->lexeme, "-", "-");
			ICG_main(node->child[2]);
			printf("End Function %s\n", node->child[0]->lexeme);
			make_quad("EndF", node->child[0]->lexeme, "-", "-");
			return;
		}

		if(!strcmp(node->nodeType, "Func_Call"))
		{
			if(!strcmp(node->child[1]->nodeType, "Void"))
			{
				printf("(t%d)Call Function %s\n", node->tempNo, node->child[0]->lexeme);
				make_quad("Call", node->child[0]->lexeme, "-", makeStr(node->tempNo, 1));
			}
			else
			{
				char A[10];
				char* token = strtok(node->child[1]->lexeme, ","); 
  				int i = 0;
				while (token != NULL) 
				{
						i++; 
				    printf("Push Param %s\n", token);
				    make_quad("Param", token, "-", "-"); 
				    token = strtok(NULL, ","); 
				}
				
				printf("(t%d)Call Function %s, %d\n", node->tempNo, node->child[0]->lexeme, i);
				sprintf(A, "%d", i);
				make_quad("Call", node->child[0]->lexeme, A, makeStr(node->tempNo, 1));
				printf("Pop Params for Function %s, %d\n", node->child[0]->lexeme, i);
								
				return;
			}
		}		



	}

	void replaceAll( char CSE[100][10], char* replaceStr, int CSE_len, int start_index)
	{
		for(int i=0; i<CSE_len; i++)
		{
			for(int j=start_index; j<qIndex; j++)
			{
				if( strcmp(all_quads[j].A1, CSE[i])==0)
				{
					strcpy(all_quads[j].A1,replaceStr);
				}

				if( strcmp(all_quads[j].A2, CSE[i])==0)
				{
					strcpy(all_quads[j].A2,replaceStr);
				}

			}
		}
	}

	int ICG_opt()
	{
		
		int i = 0, j = 0, flag = 1, dont_remove=0;
		while(flag==1)
		{
			
			flag=0;
			for(i=0; i<qIndex; i++)
			{
				dont_remove=0;
				if(!((strcmp(all_quads[i].R, "-")==0) | (strcmp(all_quads[i].Op, "Call")==0) | (strcmp(all_quads[i].Op, "Label")==0) | (strcmp(all_quads[i].Op, "goto")==0) | (strcmp(all_quads[i].Op, "If False")==0)))
				{
					for(j=0; j<qIndex; j++)
					{
							if(((strcmp(all_quads[i].R, all_quads[j].A1)==0) && (all_quads[j].I!=-1)) | ((strcmp(all_quads[i].R, all_quads[j].A2)==0) && (all_quads[j].I!=-1)))
							{
								//if a particular 'T0' is used as arg1/arg2 in another quad entry
								dont_remove=1; //keep the quad entry, don't remove it
							}
					}
				
					if((dont_remove==0) & (all_quads[i].I != -1))
					{
						//you're eliminating the quad, by not actually removing it just making its index = -1 so that it can never be printed via the print function
						all_quads[i].I = -1;
						flag=1;
					}
				}
			}
		}
		return flag;
	}

	int ICG_opt2()
	{
		
		int i = 0, j = 0;
			
			for(i=0; i<qIndex; i++)
			{
				quad temp_quad = all_quads[i];
				char CSEs[100][10];
				int CSE_index=0;
				
				for(j=i+1; j<qIndex; j++)
				{
					if( (strcmp(temp_quad.Op,all_quads[j].Op )==0) && (strcmp(temp_quad.A1,all_quads[j].A1 )==0) && (strcmp(temp_quad.A2,all_quads[j].A2 )==0) && (strcmp(temp_quad.Op,"Print")!=0) )
					{
						//check if the 2 quads have the same Op, A1 and A2
						//Print stmts should be excluded to retain pgm logic
						//printf("temp_quad.R=%s\n", temp_quad.R);

						strcpy(CSEs[CSE_index],all_quads[j].R); 
						//keep a copy of the CSE to replace in the rest of the table

						all_quads[j].I=-1; //eliminate the CSE from all_quads (the entire row)

						//printf("CSE: %s\n", CSEs[CSE_index]);
						CSE_index++;


					}
					
					if(CSE_index>0)
					{
						
						//printf("CSE_index=%d\n", CSE_index);

						replaceAll(CSEs,temp_quad.R,CSE_index,j); //occuring as A1 or A2
						CSE_index=0;
					}
				}

			}
		
	} 


	
	



	void createCSV()
	{
		FILE *fp;
		char *filename;
		filename = "TAC.csv";
		fp=fopen(filename,"w+");
		printf("\nWriting to file\n");
		int i = 0;
		fprintf(fp, "Line No,Op,Arg1,Arg2,Res\n");
		for(i=0; i<qIndex; i++)
		{
			if(all_quads[i].I > -1)
				fprintf(fp, "%d,%s,%s,%s,%s\n", all_quads[i].I, all_quads[i].Op, all_quads[i].A1, all_quads[i].A2, all_quads[i].R);
				//create_icg_csv(all_quads[i].Op,all_quads[i].A1, all_quads[i].A2, all_quads[i].R );

		}
		fclose(fp);

		printf("\n %sfile created",filename);
	}

	




#line 971 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_EndOfFile = 258,
    T_Cln = 259,
    T_NL = 260,
    T_IN = 261,
    T_NEQ = 262,
    T_EQ = 263,
    T_GT = 264,
    T_LT = 265,
    T_EGT = 266,
    T_ELT = 267,
    T_Or = 268,
    T_And = 269,
    ID = 270,
    ND = 271,
    DD = 272,
    T_String = 273,
    Trip_Quote = 274,
    T_Import = 275,
    T_MN = 276,
    T_PL = 277,
    T_DV = 278,
    T_ML = 279,
    T_OP = 280,
    T_CP = 281,
    T_OB = 282,
    T_CB = 283,
    T_Def = 284,
    T_Comma = 285,
    T_Range = 286,
    T_List = 287,
    T_Number = 288,
    T_ID = 289,
    T_EQL = 290,
    T_LEN = 291,
    T_True = 292,
    T_False = 293,
    T_Not = 294,
    T_Pass = 295,
    T_Break = 296,
    T_Return = 297,
    T_Print = 298,
    T_If = 299,
    T_Elif = 300,
    T_Else = 301,
    T_For = 302,
    T_While = 303
  };
#endif
/* Tokens.  */
#define T_EndOfFile 258
#define T_Cln 259
#define T_NL 260
#define T_IN 261
#define T_NEQ 262
#define T_EQ 263
#define T_GT 264
#define T_LT 265
#define T_EGT 266
#define T_ELT 267
#define T_Or 268
#define T_And 269
#define ID 270
#define ND 271
#define DD 272
#define T_String 273
#define Trip_Quote 274
#define T_Import 275
#define T_MN 276
#define T_PL 277
#define T_DV 278
#define T_ML 279
#define T_OP 280
#define T_CP 281
#define T_OB 282
#define T_CB 283
#define T_Def 284
#define T_Comma 285
#define T_Range 286
#define T_List 287
#define T_Number 288
#define T_ID 289
#define T_EQL 290
#define T_LEN 291
#define T_True 292
#define T_False 293
#define T_Not 294
#define T_Pass 295
#define T_Break 296
#define T_Return 297
#define T_Print 298
#define T_If 299
#define T_Elif 300
#define T_Else 301
#define T_For 302
#define T_While 303

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 909 "code.y" /* yacc.c:355  */
 char *text; int depth; struct AST *node; 

#line 1110 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 1141 "y.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   228

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  98
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  180

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   303

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   929,   929,   929,   935,   937,   942,   947,   949,   950,
     952,   953,   953,   954,   956,   957,   958,   959,   960,   961,
     962,   963,   965,   966,   967,   968,   969,   970,   971,   977,
     978,   979,   980,   981,   982,   983,   985,   987,   988,   989,
     990,   992,   993,   995,   998,   999,  1000,  1002,  1007,  1012,
    1017,  1025,  1027,  1028,  1029,  1031,  1032,  1033,  1036,  1037,
    1041,  1042,  1044,  1046,  1056,  1061,  1063,  1064,  1065,  1066,
    1067,  1067,  1069,  1070,  1072,  1072,  1073,  1075,  1075,  1077,
    1079,  1079,  1080,  1082,  1082,  1085,  1087,  1098,  1098,  1098,
    1100,  1100,  1101,  1101,  1102,  1102,  1103,  1105,  1105
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_EndOfFile", "T_Cln", "T_NL", "T_IN",
  "T_NEQ", "T_EQ", "T_GT", "T_LT", "T_EGT", "T_ELT", "T_Or", "T_And", "ID",
  "ND", "DD", "T_String", "Trip_Quote", "T_Import", "T_MN", "T_PL", "T_DV",
  "T_ML", "T_OP", "T_CP", "T_OB", "T_CB", "T_Def", "T_Comma", "T_Range",
  "T_List", "T_Number", "T_ID", "T_EQL", "T_LEN", "T_True", "T_False",
  "T_Not", "T_Pass", "T_Break", "T_Return", "T_Print", "T_If", "T_Elif",
  "T_Else", "T_For", "T_While", "$accept", "StartDebugger", "$@1",
  "constant", "list_index", "term", "StartParse", "$@2", "basic_stmt",
  "arith_exp", "bool_exp", "bool_term", "bool_factor", "import_stmt",
  "pass_stmt", "break_stmt", "return_stmt", "assign_stmt", "print_stmt",
  "finalStatements", "cmpd_stmt", "if_stmt", "elif_stmts", "else_stmt",
  "for_stmt", "while_stmt", "range_stmt", "start_suite", "$@3", "suite",
  "end_suite", "$@4", "args", "$@5", "args_list", "$@6", "func_def", "$@7",
  "list_stmt", "call_list", "$@8", "call_args", "$@9", "$@10", "$@11",
  "func_call", "$@12", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303
};
# endif

#define YYPACT_NINF -135

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-135)))

#define YYTABLE_NINF -98

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -135,    22,    73,  -135,    73,  -135,   -22,    53,   165,    42,
    -135,    -3,  -135,  -135,   -16,  -135,  -135,  -135,    59,   165,
      54,   165,  -135,  -135,  -135,    87,  -135,   204,  -135,    14,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,    92,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,    78,  -135,   185,    66,  -135,
      67,    -8,   165,  -135,    53,   204,   114,   102,   122,  -135,
      94,    19,    19,    19,    19,    19,    19,    19,    19,    19,
     165,   165,   133,  -135,  -135,   105,   126,    21,    18,   204,
    -135,  -135,  -135,   113,   104,    36,   104,  -135,    19,   127,
     127,   127,   127,   127,    12,    12,  -135,  -135,   111,  -135,
    -135,    73,   125,  -135,  -135,  -135,  -135,  -135,   128,   135,
    -135,   146,  -135,    58,   138,   160,   163,  -135,    51,  -135,
    -135,   142,   139,   139,   139,  -135,    62,  -135,   165,   168,
    -135,  -135,   149,   104,   104,   157,   184,    53,  -135,  -135,
    -135,   166,   137,   196,   104,   -15,  -135,  -135,   155,  -135,
     104,  -135,  -135,   200,   104,  -135,  -135,   186,  -135,  -135,
     139,    65,  -135,    58,     8,   157,  -135,   137,  -135,  -135,
    -135,  -135,   187,  -135,   200,   137,   175,  -135,  -135,  -135
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,     0,     5,     0,     0,     0,     0,
       4,     7,    39,    40,     0,    44,    45,    46,     0,     0,
       0,     0,     8,     9,    22,     0,    52,    18,    19,    36,
      37,    16,    14,    15,    21,    17,    20,     0,    53,    55,
      57,    56,    54,    10,    43,     7,    27,     0,     0,    83,
       0,     0,     0,    41,     0,     0,     0,     0,     0,     3,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    11,    28,    42,     0,     0,     0,     7,    47,
      48,    50,    49,     0,     0,     0,     0,    35,     0,    38,
      32,    30,    34,    33,    24,    23,    26,    25,     0,    29,
      31,     0,    79,     6,    94,    85,    92,    90,     0,     0,
      51,     0,    69,    58,     0,     0,     0,    65,     0,    12,
      77,     0,    89,    89,    89,    86,    96,    70,     0,     0,
      59,    60,     0,     0,     0,    82,     0,     0,    95,    93,
      91,     0,     0,     0,     0,     0,    64,    63,     0,    78,
       0,    87,    98,     0,     0,    62,    66,     0,    80,    84,
      89,    76,    71,     0,     0,    82,    88,     0,    74,    73,
      61,    67,     0,    81,     0,     0,     0,    72,    75,    68
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -135,  -135,  -135,  -135,  -135,    -6,     0,  -135,    -2,    -5,
      -1,    82,   203,  -135,  -135,  -135,  -135,  -135,  -135,  -134,
    -135,  -135,    55,  -135,  -135,  -135,  -135,   -65,  -135,    47,
    -135,  -135,  -135,  -135,    57,  -135,  -135,  -135,  -135,  -118,
    -135,    97,  -135,  -135,  -135,  -135,  -135
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    22,    23,    24,    25,   101,   112,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,   130,   131,    40,    41,   116,   113,   142,   162,
     169,   175,   121,   135,   149,   165,    42,    75,    81,   138,
     160,   108,   124,   123,   122,    82,   109
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      26,    46,    26,    47,    43,   139,   140,    48,   153,    52,
       5,   156,    44,     7,    55,   157,    55,     8,    56,    77,
      58,   117,     3,    14,    50,    10,    78,    70,    71,    12,
      13,    14,    51,   174,   171,    68,    69,     5,   172,   104,
       7,   178,   166,   -97,    88,    50,    79,    55,    83,   105,
      80,    48,    10,    45,   106,   107,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    98,   114,   146,   147,
     115,     5,    66,    67,    68,    69,    49,    73,     4,   155,
     104,   167,   168,   118,    54,   159,    10,    45,    57,   163,
      59,     5,    74,     6,     7,   106,   107,    72,     8,    26,
      76,   119,     9,   128,   129,    50,    10,    11,    85,   111,
      12,    13,    14,    15,    16,    17,    18,    19,    84,    61,
      20,    21,     5,    55,     6,     7,    86,   143,    87,     8,
     102,   151,    66,    67,    68,    69,   -13,    10,    11,   110,
      26,    12,    13,    14,    15,    16,    17,    18,    66,    67,
      68,    69,    99,   100,   103,     5,   125,     6,     7,   120,
     126,   127,     8,   132,   133,    26,     9,   134,   136,   137,
      10,    11,   144,    26,    12,    13,    14,    15,    16,    17,
      18,    19,   145,     5,    20,    21,     7,   148,   150,   158,
       8,    60,   152,    61,    62,    63,    64,    65,    10,    45,
     154,   179,    12,    13,    14,   161,    66,    67,    68,    69,
      60,    73,    61,    62,    63,    64,    65,    53,   170,   164,
     176,   177,   173,   141,     0,    66,    67,    68,    69
};

static const yytype_int16 yycheck[] =
{
       2,     7,     4,     8,     4,   123,   124,     8,   142,    25,
      18,    26,    34,    21,    19,    30,    21,    25,    19,    27,
      21,    86,     0,    39,    27,    33,    34,    13,    14,    37,
      38,    39,    35,   167,    26,    23,    24,    18,    30,    18,
      21,   175,   160,    25,    25,    27,    51,    52,    54,    28,
      51,    52,    33,    34,    33,    34,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    31,   133,   134,
      34,    18,    21,    22,    23,    24,    34,    26,     5,   144,
      18,    16,    17,    88,    25,   150,    33,    34,    34,   154,
       3,    18,    26,    20,    21,    33,    34,     5,    25,   101,
      33,   101,    29,    45,    46,    27,    33,    34,     6,     5,
      37,    38,    39,    40,    41,    42,    43,    44,     4,     8,
      47,    48,    18,   128,    20,    21,     4,   128,    34,    25,
      25,   137,    21,    22,    23,    24,     3,    33,    34,    26,
     142,    37,    38,    39,    40,    41,    42,    43,    21,    22,
      23,    24,    70,    71,    28,    18,    28,    20,    21,    34,
      25,    15,    25,    25,     4,   167,    29,     4,    26,    30,
      33,    34,     4,   175,    37,    38,    39,    40,    41,    42,
      43,    44,    33,    18,    47,    48,    21,    30,     4,    34,
      25,     6,    26,     8,     9,    10,    11,    12,    33,    34,
       4,    26,    37,    38,    39,     5,    21,    22,    23,    24,
       6,    26,     8,     9,    10,    11,    12,    14,   163,    33,
      33,   174,   165,   126,    -1,    21,    22,    23,    24
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    50,    51,     0,     5,    18,    20,    21,    25,    29,
      33,    34,    37,    38,    39,    40,    41,    42,    43,    44,
      47,    48,    52,    53,    54,    55,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      73,    74,    85,    55,    34,    34,    54,    58,    59,    34,
      27,    35,    25,    61,    25,    58,    59,    34,    59,     3,
       6,     8,     9,    10,    11,    12,    21,    22,    23,    24,
      13,    14,     5,    26,    26,    86,    33,    27,    34,    58,
      59,    87,    94,    54,     4,     6,     4,    34,    25,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    60,
      60,    56,    25,    28,    18,    28,    33,    34,    90,    95,
      26,     5,    57,    76,    31,    34,    75,    76,    58,    55,
      34,    81,    93,    92,    91,    28,    25,    15,    45,    46,
      71,    72,    25,     4,     4,    82,    26,    30,    88,    88,
      88,    90,    77,    59,     4,    33,    76,    76,    30,    83,
       4,    54,    26,    68,     4,    76,    26,    30,    34,    76,
      89,     5,    78,    76,    33,    84,    88,    16,    17,    79,
      71,    26,    30,    83,    68,    80,    33,    78,    68,    26
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    49,    51,    50,    52,    52,    53,    54,    54,    54,
      55,    56,    55,    55,    57,    57,    57,    57,    57,    57,
      57,    57,    58,    58,    58,    58,    58,    58,    58,    59,
      59,    59,    59,    59,    59,    59,    59,    60,    60,    60,
      60,    61,    61,    62,    63,    64,    65,    66,    66,    66,
      66,    67,    68,    68,    68,    69,    69,    69,    70,    70,
      71,    71,    72,    73,    73,    74,    75,    75,    75,    76,
      77,    76,    78,    78,    80,    79,    79,    82,    81,    81,
      84,    83,    83,    86,    85,    87,    87,    89,    88,    88,
      91,    90,    92,    90,    93,    90,    90,    95,    94
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     1,     1,     4,     1,     1,     1,
       2,     0,     4,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     1,     3,     1,
       1,     2,     3,     2,     1,     1,     1,     3,     3,     3,
       3,     4,     1,     1,     1,     1,     1,     1,     4,     5,
       1,     5,     3,     6,     6,     4,     4,     6,     8,     1,
       0,     5,     4,     2,     0,     3,     0,     0,     3,     0,
       0,     4,     0,     0,     8,     2,     3,     0,     4,     0,
       0,     3,     0,     3,     0,     3,     0,     0,     5
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 929 "code.y" /* yacc.c:1646  */
    {init();}
#line 2453 "y.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 929 "code.y" /* yacc.c:1646  */
    {printf("\nValid Python Syntax\n" ); printf("**************************************************************************\n");
printf("\n\n*********************************AST***********************************"); AST_print((yyvsp[-1].node)); printf("\n**************************************************************************\n");
printf("\n\n***********************************ICG***********************************\n"); ICG_main((yyvsp[-1].node));
printf("**************************************************************************\n");
printQuads(); printSTable(); freeAll();  exit(0);}
#line 2463 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 935 "code.y" /* yacc.c:1646  */
    {insertRecord("Constant", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);
					  (yyval.node) = make_leaf((yyvsp[0].text), "Constant");}
#line 2470 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 937 "code.y" /* yacc.c:1646  */
    {insertRecord("Constant", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);
		 			  (yyval.node) = make_leaf((yyvsp[0].text), "Constant");}
#line 2477 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 942 "code.y" /* yacc.c:1646  */
    {
									  checkList((yyvsp[-3].text), (yylsp[-3]).first_line, currentScope);
									  (yyval.node) = make_node("ListIndex", "ListIndex", make_leaf((yyvsp[-3].text), "ListTypeID"), make_leaf((yyvsp[-1].text), "Constant"));
									  }
#line 2486 "y.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 947 "code.y" /* yacc.c:1646  */
    { modifyRecordID("Identifier", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); 
		(yyval.node) = make_leaf((yyvsp[0].text),"Identifier");}
#line 2493 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 949 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2499 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 950 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2505 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 952 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2511 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 953 "code.y" /* yacc.c:1646  */
    {resetDepth();}
#line 2517 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 953 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("Start","NewLine", (yyvsp[-3].node), (yyvsp[0].node));}
#line 2523 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 954 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-1].node);}
#line 2529 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 956 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2535 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 957 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2541 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 958 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2547 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 959 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2553 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 960 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2559 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 961 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2565 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 962 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2571 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 963 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2577 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 965 "code.y" /* yacc.c:1646  */
    { (yyval.node)=(yyvsp[0].node);}
#line 2583 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 966 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("+","+",(yyvsp[-2].node), (yyvsp[0].node)); }
#line 2589 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 967 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("binary-","binary-",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2595 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 968 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("*","*",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2601 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 969 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("/","/",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2607 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 970 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("temp","unary-", make_leaf("-", ""), (yyvsp[0].node));}
#line 2613 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 971 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-1].node);}
#line 2619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 977 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("or", "or", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2625 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 978 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("<", "<",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2631 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 979 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("and","and", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2637 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 980 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node(">",">",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2643 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 981 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("<=","<=", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2649 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 982 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node(">=",">=",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2655 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 983 "code.y" /* yacc.c:1646  */
    {checkList((yyvsp[0].text), (yylsp[0]).first_line, currentScope); 
         (yyval.node) = make_node("in", "in", (yyvsp[-2].node), make_leaf((yyvsp[0].text), "Constant"));}
#line 2662 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 985 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2668 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 987 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2674 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 988 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("==", "==",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2680 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 989 "code.y" /* yacc.c:1646  */
    {insertRecord("Constant", "True", (yylsp[0]).first_line, currentScope); (yyval.node) = make_leaf("True", "Constant");}
#line 2686 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 990 "code.y" /* yacc.c:1646  */
    {insertRecord("Constant", "False", (yylsp[0]).first_line, currentScope); (yyval.node) = make_leaf("False", "Constant");}
#line 2692 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 992 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("not", "not", make_leaf("", ""), (yyvsp[0].node));}
#line 2698 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 993 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[-1].node);}
#line 2704 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 995 "code.y" /* yacc.c:1646  */
    {insertRecord("PackageName", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); 
							  (yyval.node) = make_node("import", "import", make_leaf((yyvsp[0].text), "PackageName"), make_leaf("", "") );}
#line 2711 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 998 "code.y" /* yacc.c:1646  */
    {(yyval.node)=make_leaf("pass", "pass");}
#line 2717 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 999 "code.y" /* yacc.c:1646  */
    {(yyval.node)=make_leaf("break", "break");}
#line 2723 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 1000 "code.y" /* yacc.c:1646  */
    {(yyval.node)=make_leaf("return", "return");}
#line 2729 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 1003 "code.y" /* yacc.c:1646  */
    {
			  insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope); 
			  (yyval.node)=make_node("=","=",make_leaf((yyvsp[-2].text), "Identifier"),(yyvsp[0].node));
			  }
#line 2738 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 1008 "code.y" /* yacc.c:1646  */
    {
			 	insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope);
			 	(yyval.node) = make_node("=","=", make_leaf((yyvsp[-2].text), "Identifier"), (yyvsp[0].node));
			 }
#line 2747 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 1013 "code.y" /* yacc.c:1646  */
    {
			 	insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope);
			 	(yyval.node) = make_node("=", "=",make_leaf((yyvsp[-2].text), "Identifier"), (yyvsp[0].node));
			 }
#line 2756 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 1018 "code.y" /* yacc.c:1646  */
    {
			 	insertRecord("ListTypeID", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope);
			 	(yyval.node) = make_node("=", "ListDecl", make_leaf((yyvsp[-2].text), "ListTypeID"), (yyvsp[0].node));
			 }
#line 2765 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 1025 "code.y" /* yacc.c:1646  */
    {(yyval.node)=make_node("Print", "Print", (yyvsp[-1].node), make_leaf("", ""));}
#line 2771 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 1031 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2777 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 1032 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2783 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 1033 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2789 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 1036 "code.y" /* yacc.c:1646  */
    {(yyval.node)= make_node("IF", "IF", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2795 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 1038 "code.y" /* yacc.c:1646  */
    {(yyval.node)= make_node("IF-ELIF-ELSE", "IF-ELIF-ELSE", make_node("IF","IF", (yyvsp[-3].node), (yyvsp[-1].node)), (yyvsp[0].node));}
#line 2801 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 1041 "code.y" /* yacc.c:1646  */
    {(yyval.node)= (yyvsp[0].node);}
#line 2807 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 1042 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_for_node("ELIF", "ELIF", (yyvsp[-3].node), (yyvsp[-1].node), (yyvsp[0].node), make_leaf("",""));}
#line 2813 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 1044 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("ELSE","ELSE", (yyvsp[0].node), make_leaf("", ""));}
#line 2819 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 1047 "code.y" /* yacc.c:1646  */
    {
				insertRecord("Identifier", (yyvsp[-4].text), (yylsp[-4]).first_line, currentScope); 
				char rangeNodeText[20] ="";
				strcat(rangeNodeText, (yyvsp[-4].text));
				strcat(rangeNodeText, " in range");
				//printf("%\n%s", rangeNodeText);
				(yyval.node) =make_node("FOR", "FOR", make_leaf(rangeNodeText, argsList), (yyvsp[0].node));  
				clearArgsList(); 
			}
#line 2833 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 1056 "code.y" /* yacc.c:1646  */
    {
		 insertRecord("Identifier", (yyvsp[-4].text), (yylsp[-4]).first_line, currentScope); 
		 checkList((yyvsp[-2].text), (yylsp[-2]).first_line, currentScope);
		 (yyval.node) =make_node("FOR", "FOR", make_leaf((yyvsp[-2].text), "Identifier"), (yyvsp[0].node));}
#line 2842 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 1061 "code.y" /* yacc.c:1646  */
    {(yyval.node) =make_node("WHILE","WHILE", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2848 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 1063 "code.y" /* yacc.c:1646  */
    {addToList("0", 1); addToList((yyvsp[-1].text), 0); }
#line 2854 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 1064 "code.y" /* yacc.c:1646  */
    {addToList((yyvsp[-3].text), 1); addToList((yyvsp[-1].text), 0);}
#line 2860 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 1065 "code.y" /* yacc.c:1646  */
    {addToList((yyvsp[-5].text), 1); addToList((yyvsp[-3].text), 0);}
#line 2866 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 1066 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2872 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 1067 "code.y" /* yacc.c:1646  */
    { initNewTable((yyvsp[0].depth)); updateCScope((yyvsp[0].depth));}
#line 2878 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 1067 "code.y" /* yacc.c:1646  */
    {(yyval.node) =make_node("start_suite", "BeginBlock", (yyvsp[-1].node), (yyvsp[0].node));}
#line 2884 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 1069 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("Next", "Next", (yyvsp[-1].node), (yyvsp[0].node));}
#line 2890 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 1070 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2896 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 1072 "code.y" /* yacc.c:1646  */
    {updateCScope((yyvsp[0].depth));}
#line 2902 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 1072 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("", "EndBlock", (yyvsp[0].node), make_leaf("", ""));}
#line 2908 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 1073 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf("", ""); resetDepth();}
#line 2914 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 1075 "code.y" /* yacc.c:1646  */
    {insertRecord("Identifier", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); addToList((yyvsp[0].text), 1);}
#line 2920 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 1076 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf(argsList, "argsList"); clearArgsList();}
#line 2926 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 1077 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf("", "Void");}
#line 2932 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 1079 "code.y" /* yacc.c:1646  */
    {insertRecord("Identifier", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); addToList((yyvsp[0].text), 0);}
#line 2938 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 1080 "code.y" /* yacc.c:1646  */
    {addToList("",0);}
#line 2944 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 1082 "code.y" /* yacc.c:1646  */
    {insertRecord("Func_Name", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);}
#line 2950 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 1083 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_for_node("Func_def", "Func_Name", make_leaf((yyvsp[-6].text), "Func_Name"), (yyvsp[-3].node), (yyvsp[0].node), make_leaf("",""));}
#line 2956 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 1085 "code.y" /* yacc.c:1646  */
    { (yyval.node) = make_leaf("[]", ""); }
#line 2962 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 1087 "code.y" /* yacc.c:1646  */
    {
			 	char* str = (char *)malloc(102*sizeof(char));
			 	strcpy(str,"[");
			 	strcat(str, argsList);
			 	char close[2];
			 	strcpy(close,"]");
			 	strcat(str, close);
			 	(yyval.node) = make_leaf(str, ""); 
			 	clearArgsList(); 
			 	free(str);}
#line 2977 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 1098 "code.y" /* yacc.c:1646  */
    {addToList((yyvsp[0].node)->lexeme, 0);}
#line 2983 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 1100 "code.y" /* yacc.c:1646  */
    {modifyRecordID("Identifier", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); addToList((yyvsp[0].text), 1);}
#line 2989 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 1100 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf(argsList,"argsList"); }
#line 2995 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 1101 "code.y" /* yacc.c:1646  */
    {addToList((yyvsp[0].text), 1);}
#line 3001 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 1101 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf(argsList,"argsList"); }
#line 3007 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 1102 "code.y" /* yacc.c:1646  */
    {addToList((yyvsp[0].text), 1);}
#line 3013 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 1102 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf(argsList,"argsList"); }
#line 3019 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 1103 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf("","Void");}
#line 3025 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 1105 "code.y" /* yacc.c:1646  */
    {modifyRecordID("Func_Name", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);}
#line 3031 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 1105 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("Func_Call", "Func_Call",make_leaf((yyvsp[-4].text), "Func_Name"), (yyvsp[-1].node));}
#line 3037 "y.tab.c" /* yacc.c:1646  */
    break;


#line 3041 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1107 "code.y" /* yacc.c:1906  */


void yyerror(const char *msg)
{
	//printSTable();
	//printf("\n\n%s", msg);
	printf("\nSyntax Error at Line %d, Column : %d\n",  yylineno, yylloc.last_column);
	printf("**************************************************************************\n");
	//printSTable();
	printf("**************************************************************************\n");
	//exit(0);
}

