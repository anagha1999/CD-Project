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


#line 387 "y.tab.c" /* yacc.c:339  */

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
    T_Number = 259,
    T_Cln = 260,
    T_NL = 261,
    T_IN = 262,
    T_NEQ = 263,
    T_EQ = 264,
    T_GT = 265,
    T_LT = 266,
    T_EGT = 267,
    T_ELT = 268,
    T_Or = 269,
    T_And = 270,
    ID = 271,
    ND = 272,
    DD = 273,
    T_String = 274,
    Trip_Quote = 275,
    T_Import = 276,
    T_MN = 277,
    T_PL = 278,
    T_DV = 279,
    T_ML = 280,
    T_OP = 281,
    T_CP = 282,
    T_OB = 283,
    T_CB = 284,
    T_Def = 285,
    T_Comma = 286,
    T_Range = 287,
    T_List = 288,
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
#define T_Number 259
#define T_Cln 260
#define T_NL 261
#define T_IN 262
#define T_NEQ 263
#define T_EQ 264
#define T_GT 265
#define T_LT 266
#define T_EGT 267
#define T_ELT 268
#define T_Or 269
#define T_And 270
#define ID 271
#define ND 272
#define DD 273
#define T_String 274
#define Trip_Quote 275
#define T_Import 276
#define T_MN 277
#define T_PL 278
#define T_DV 279
#define T_ML 280
#define T_OP 281
#define T_CP 282
#define T_OB 283
#define T_CB 284
#define T_Def 285
#define T_Comma 286
#define T_Range 287
#define T_List 288
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
#line 324 "code.y" /* yacc.c:355  */
 char *text; int depth; struct AST *node; 

#line 526 "y.tab.c" /* yacc.c:355  */
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

#line 557 "y.tab.c" /* yacc.c:358  */

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
#define YYLAST   220

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  81
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  152

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
       0,   342,   342,   342,   346,   348,   351,   355,   356,   358,
     363,   364,   364,   365,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   377,   378,   379,   380,   381,   382,   383,
     386,   387,   390,   391,   392,   393,   394,   395,   396,   398,
     400,   401,   402,   403,   405,   406,   408,   411,   412,   413,
     415,   421,   422,   423,   425,   427,   428,   429,   431,   432,
     433,   436,   437,   440,   441,   443,   444,   445,   447,   448,
     448,   449,   450,   451,   451,   452,   454,   455,   455,   456,
     456,   457
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_EndOfFile", "T_Number", "T_Cln",
  "T_NL", "T_IN", "T_NEQ", "T_EQ", "T_GT", "T_LT", "T_EGT", "T_ELT",
  "T_Or", "T_And", "ID", "ND", "DD", "T_String", "Trip_Quote", "T_Import",
  "T_MN", "T_PL", "T_DV", "T_ML", "T_OP", "T_CP", "T_OB", "T_CB", "T_Def",
  "T_Comma", "T_Range", "T_List", "T_ID", "T_EQL", "T_LEN", "T_True",
  "T_False", "T_Not", "T_Pass", "T_Break", "T_Return", "T_Print", "T_If",
  "T_Elif", "T_Else", "T_For", "T_While", "$accept", "StartDebugger",
  "$@1", "constant", "term", "list_index", "StartParse", "$@2",
  "basic_stmt", "arith_exp", "range_stmt", "bool_exp", "bool_term",
  "bool_factor", "import_stmt", "pass_stmt", "break_stmt", "return_stmt",
  "assign_stmt", "print_stmt", "finalStatements", "cmpd_stmt", "if_stmt",
  "elif_stmts", "else_stmt", "for_stmt", "while_stmt", "start_suite",
  "$@3", "suite", "end_suite", "$@4", "args_list", "func_def", "$@5",
  "func_call", YY_NULLPTR
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

#define YYPACT_NINF -106

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-106)))

#define YYTABLE_NINF -14

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -106,    18,    65,  -106,  -106,    65,  -106,    -1,    -2,   137,
       2,    17,     0,  -106,  -106,     8,  -106,  -106,  -106,    41,
     137,   137,   137,  -106,  -106,  -106,    78,  -106,   195,  -106,
    -106,    -5,  -106,  -106,  -106,  -106,  -106,  -106,  -106,    77,
    -106,  -106,  -106,  -106,  -106,  -106,  -106,    57,  -106,   188,
      63,  -106,    90,    92,     3,   137,  -106,    79,   195,    96,
     109,   111,  -106,    83,     4,     4,     4,     4,     4,     4,
       4,     4,     4,   137,   137,   108,  -106,  -106,    98,    49,
      94,   102,    25,   195,  -106,  -106,    99,   151,   151,   151,
    -106,     4,    97,    97,    97,    97,    97,    54,    54,  -106,
    -106,   142,  -106,  -106,    65,   100,  -106,   129,  -106,  -106,
     100,  -106,   119,  -106,    43,  -106,  -106,    50,  -106,   121,
     110,   112,   115,  -106,   137,   153,  -106,  -106,   100,   155,
    -106,  -106,   106,   156,   151,  -106,   151,   162,   151,  -106,
    -106,    75,  -106,    43,   106,  -106,  -106,  -106,   162,   106,
    -106,  -106
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,     4,     0,     5,     0,     0,     0,
       0,     0,     6,    42,    43,     0,    47,    48,    49,     0,
       0,     0,     0,     7,    23,     8,     0,    55,    18,    22,
      19,    39,    40,    16,    14,    15,    21,    17,    20,     0,
      56,    58,    60,    59,    57,    10,    46,     6,    28,     0,
       0,    79,     0,     0,     0,     0,    44,     0,     0,     0,
       0,     0,     3,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    11,    29,    45,     0,     0,
       0,     0,     6,    50,    51,    52,     0,     0,     0,     0,
      38,     0,    41,    35,    33,    37,    36,    25,    24,    27,
      26,     0,    32,    34,     0,    78,    30,     0,     9,    53,
      78,    54,     0,    68,    61,    66,    67,     0,    12,    77,
       0,     0,     0,    69,     0,     0,    62,    63,    78,     0,
      31,    81,     0,     0,     0,    76,     0,     0,     0,    65,
      80,    75,    70,     0,     0,    73,    72,    64,     0,     0,
      71,    74
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -106,  -106,  -106,  -106,   154,  -106,    -4,  -106,   -68,    -9,
    -106,    -6,    56,   163,  -106,  -106,  -106,  -106,  -106,  -106,
    -105,  -106,  -106,    26,  -106,  -106,  -106,   -84,  -106,    31,
    -106,  -106,  -104,  -106,  -106,  -106
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    23,    24,    25,    26,   104,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,   126,   127,    42,    43,   114,   132,   142,
     146,   149,   120,    44,    78,    85
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      49,    45,     4,    50,   115,   116,   122,     4,     4,    73,
      74,    58,    58,    58,    59,    60,    61,     6,     3,   113,
     113,   113,     6,     6,   135,     8,     8,   137,    53,     9,
      91,    81,    47,    46,    55,    54,    51,    82,    47,   148,
      13,    14,    15,    52,   151,    83,    58,    15,    84,    50,
     139,   110,   140,    53,   143,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   101,   113,    57,   113,     4,
     113,     5,    69,    70,    71,    72,   106,    76,    71,    72,
     107,    62,   117,    75,     6,    53,     7,     8,   124,   125,
      77,     9,   144,   145,    79,    10,    80,    11,    86,    12,
     118,    87,    13,    14,    15,    16,    17,    18,    19,    20,
       4,   -13,    21,    22,    88,    58,    89,    90,   133,    69,
      70,    71,    72,   108,   105,     6,   111,     7,     8,   102,
     103,   109,     9,   121,   119,   123,    10,   129,    11,   130,
      12,     4,   131,    13,    14,    15,    16,    17,    18,    19,
      20,    64,   128,    21,    22,     4,     6,   112,   134,     8,
     136,   138,    48,     9,    69,    70,    71,    72,   141,   147,
       6,    47,     7,     8,    13,    14,    15,     9,    56,   150,
       0,     0,     0,    11,     0,    12,     0,     0,    13,    14,
      15,    16,    17,    18,    19,    63,     0,    64,    65,    66,
      67,    68,    63,     0,    64,    65,    66,    67,    68,     0,
      69,    70,    71,    72,     0,    76,     0,    69,    70,    71,
      72
};

static const yytype_int16 yycheck[] =
{
       9,     5,     4,     9,    88,    89,   110,     4,     4,    14,
      15,    20,    21,    22,    20,    21,    22,    19,     0,    87,
      88,    89,    19,    19,   128,    22,    22,   132,    28,    26,
      26,    28,    34,    34,    26,    35,    34,    34,    34,   144,
      37,    38,    39,    26,   149,    54,    55,    39,    54,    55,
     134,    26,   136,    28,   138,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,   134,    26,   136,     4,
     138,     6,    22,    23,    24,    25,    27,    27,    24,    25,
      31,     3,    91,     6,    19,    28,    21,    22,    45,    46,
      27,    26,    17,    18,     4,    30,     4,    32,    19,    34,
     104,     5,    37,    38,    39,    40,    41,    42,    43,    44,
       4,     3,    47,    48,     5,   124,     5,    34,   124,    22,
      23,    24,    25,    29,    26,    19,    27,    21,    22,    73,
      74,    29,    26,     4,    34,    16,    30,    27,    32,    27,
      34,     4,    27,    37,    38,    39,    40,    41,    42,    43,
      44,     9,    31,    47,    48,     4,    19,     6,     5,    22,
       5,     5,     8,    26,    22,    23,    24,    25,     6,   143,
      19,    34,    21,    22,    37,    38,    39,    26,    15,   148,
      -1,    -1,    -1,    32,    -1,    34,    -1,    -1,    37,    38,
      39,    40,    41,    42,    43,     7,    -1,     9,    10,    11,
      12,    13,     7,    -1,     9,    10,    11,    12,    13,    -1,
      22,    23,    24,    25,    -1,    27,    -1,    22,    23,    24,
      25
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    50,    51,     0,     4,     6,    19,    21,    22,    26,
      30,    32,    34,    37,    38,    39,    40,    41,    42,    43,
      44,    47,    48,    52,    53,    54,    55,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    74,    75,    82,    55,    34,    34,    53,    58,
      60,    34,    26,    28,    35,    26,    62,    26,    58,    60,
      60,    60,     3,     7,     9,    10,    11,    12,    13,    22,
      23,    24,    25,    14,    15,     6,    27,    27,    83,     4,
       4,    28,    34,    58,    60,    84,    19,     5,     5,     5,
      34,    26,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    61,    61,    56,    26,    27,    31,    29,    29,
      26,    27,     6,    57,    76,    76,    76,    58,    55,    34,
      81,     4,    81,    16,    45,    46,    72,    73,    31,    27,
      27,    27,    77,    60,     5,    81,     5,    69,     5,    76,
      76,     6,    78,    76,    17,    18,    79,    72,    69,    80,
      78,    69
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    49,    51,    50,    52,    52,    53,    53,    53,    54,
      55,    56,    55,    55,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    58,    58,    58,    58,    58,    58,    58,
      59,    59,    60,    60,    60,    60,    60,    60,    60,    60,
      61,    61,    61,    61,    62,    62,    63,    64,    65,    66,
      67,    67,    67,    67,    68,    69,    69,    69,    70,    70,
      70,    71,    71,    72,    72,    73,    74,    75,    76,    77,
      76,    78,    78,    80,    79,    79,    81,    81,    81,    83,
      82,    84
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     1,     1,     1,     1,     1,     4,
       2,     0,     4,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     2,     3,
       4,     6,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     3,     1,     1,     2,     3,     2,     1,     1,     1,
       3,     3,     3,     4,     4,     1,     1,     1,     1,     1,
       1,     4,     5,     1,     5,     3,     4,     4,     1,     0,
       5,     4,     2,     0,     3,     0,     3,     1,     0,     0,
       8,     4
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
#line 342 "code.y" /* yacc.c:1646  */
    {init();}
#line 1857 "y.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 342 "code.y" /* yacc.c:1646  */
    {printf("\nValid Python Syntax\n"); 
printf("\n\n---------------------------------AST-----------------------------------"); AST_print((yyvsp[-1].node));   
printSTable(); freeAll(); exit(0);}
#line 1865 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 346 "code.y" /* yacc.c:1646  */
    {insertRecord("Constant", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);
					  (yyval.node) = make_leaf((yyvsp[0].text));}
#line 1872 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 348 "code.y" /* yacc.c:1646  */
    {insertRecord("Constant", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);
		 			  (yyval.node) = make_leaf((yyvsp[0].text));}
#line 1879 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 351 "code.y" /* yacc.c:1646  */
    {
			(yyval.node)=make_leaf((yyvsp[0].text));
			modifyRecordID("Identifier", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);
			}
#line 1888 "y.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 355 "code.y" /* yacc.c:1646  */
    { (yyval.node)=(yyvsp[0].node);}
#line 1894 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 356 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1900 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 358 "code.y" /* yacc.c:1646  */
    {
									  checkList((yyvsp[-3].text), (yylsp[-3]).first_line, currentScope);
									  (yyval.node) = make_node("ListIndex", make_leaf((yyvsp[-3].text)), make_leaf((yyvsp[-1].text)));
									  }
#line 1909 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 363 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1915 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 364 "code.y" /* yacc.c:1646  */
    {resetDepth();}
#line 1921 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 364 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("Start", (yyvsp[-3].node), (yyvsp[0].node));}
#line 1927 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 365 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-1].node);}
#line 1933 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 367 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1939 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 368 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1945 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 369 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1951 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 370 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1957 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 371 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1963 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 372 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1969 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 373 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1975 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 374 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1981 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 377 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 1987 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 378 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("+",(yyvsp[-2].node), (yyvsp[0].node));}
#line 1993 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 379 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("-",(yyvsp[-2].node), (yyvsp[0].node));}
#line 1999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 380 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("/",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2005 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 381 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("*",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2011 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 382 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("temp", make_leaf("-"), (yyvsp[0].node));}
#line 2017 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 383 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[-1].node);}
#line 2023 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 386 "code.y" /* yacc.c:1646  */
    {insertRecord("Identifier", (yyvsp[-3].text), (yylsp[-3]).first_line, currentScope);}
#line 2029 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 387 "code.y" /* yacc.c:1646  */
    {insertRecord("Identifier", (yyvsp[-5].text), (yylsp[-5]).first_line, currentScope);}
#line 2035 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 390 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("or", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2041 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 391 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("<", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2047 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 392 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("and", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2053 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 393 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node(">",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2059 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 394 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("<=", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2065 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 395 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node(">=",(yyvsp[-2].node), (yyvsp[0].node));}
#line 2071 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 396 "code.y" /* yacc.c:1646  */
    {checkList((yyvsp[0].text), (yylsp[0]).first_line, currentScope); 
         (yyval.node) = make_node("in", (yyvsp[-2].node), make_leaf("$<text>3"));}
#line 2078 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 398 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2084 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 400 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2090 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 401 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("==", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2096 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 402 "code.y" /* yacc.c:1646  */
    {insertRecord("Constant", "True", (yylsp[0]).first_line, currentScope); (yyval.node) = make_leaf("True");}
#line 2102 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 403 "code.y" /* yacc.c:1646  */
    {insertRecord("Constant", "False", (yylsp[0]).first_line, currentScope); (yyval.node) = make_leaf("False");}
#line 2108 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 405 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("!", make_leaf("end"), (yyvsp[0].node));}
#line 2114 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 406 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[-1].node);}
#line 2120 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 408 "code.y" /* yacc.c:1646  */
    {insertRecord("PackageName", (yyvsp[0].text), (yylsp[0]).first_line, currentScope); 
							  (yyval.node) = make_node("import_stmt", make_leaf("import"), make_leaf((yyvsp[0].text)) );}
#line 2127 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 411 "code.y" /* yacc.c:1646  */
    {(yyval.node)=make_leaf("pass");}
#line 2133 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 412 "code.y" /* yacc.c:1646  */
    {(yyval.node)=make_leaf("break");}
#line 2139 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 413 "code.y" /* yacc.c:1646  */
    {(yyval.node)=make_leaf("return");}
#line 2145 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 416 "code.y" /* yacc.c:1646  */
    {
			  insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope); 
			  //$1 = make_leaf($<text>1);
			  (yyval.node)=make_node("=",make_leaf((yyvsp[-2].text)),(yyvsp[0].node));
			  }
#line 2155 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 421 "code.y" /* yacc.c:1646  */
    {insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope);}
#line 2161 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 422 "code.y" /* yacc.c:1646  */
    {insertRecord("Identifier", (yyvsp[-2].text), (yylsp[-2]).first_line, currentScope);}
#line 2167 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 423 "code.y" /* yacc.c:1646  */
    {insertRecord("ListTypeID", (yyvsp[-3].text), (yylsp[-3]).first_line, currentScope);}
#line 2173 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 425 "code.y" /* yacc.c:1646  */
    {(yyval.node)=make_node("print_stmt", make_leaf((yyvsp[-1].text)), make_leaf("NULL"));}
#line 2179 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 431 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2185 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 432 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2191 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 433 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2197 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 436 "code.y" /* yacc.c:1646  */
    {(yyval.node)= make_node("IF", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2203 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 437 "code.y" /* yacc.c:1646  */
    {(yyval.node)= make_node("IF-ELIF-ELSE", make_node("IF", (yyvsp[-3].node), (yyvsp[-1].node)), (yyvsp[0].node));}
#line 2209 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 440 "code.y" /* yacc.c:1646  */
    {(yyval.node)= (yyvsp[0].node);}
#line 2215 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 441 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_for_node("ELIF", (yyvsp[-3].node), (yyvsp[-1].node), (yyvsp[0].node), make_leaf("NULL"));}
#line 2221 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 443 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("ELSE", (yyvsp[0].node), make_leaf("NULL"));}
#line 2227 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 444 "code.y" /* yacc.c:1646  */
    {(yyval.node) =make_node((yyvsp[-3].text), (yyvsp[-2].node), (yyvsp[0].node));}
#line 2233 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 445 "code.y" /* yacc.c:1646  */
    {(yyval.node) =make_node("WHILE", (yyvsp[-2].node), (yyvsp[0].node));}
#line 2239 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 447 "code.y" /* yacc.c:1646  */
    {(yyval.node)=(yyvsp[0].node);}
#line 2245 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 448 "code.y" /* yacc.c:1646  */
    { initNewTable((yyvsp[0].depth)); updateCScope((yyvsp[0].depth));}
#line 2251 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 448 "code.y" /* yacc.c:1646  */
    {(yyval.node) =make_node("start_suite",  (yyvsp[-1].node), (yyvsp[0].node));}
#line 2257 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 449 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("Next", (yyvsp[-1].node), (yyvsp[0].node));}
#line 2263 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 450 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2269 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 451 "code.y" /* yacc.c:1646  */
    {updateCScope((yyvsp[0].depth));}
#line 2275 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 451 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node("EndBlock", (yyvsp[0].node), make_leaf("NULL"));}
#line 2281 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 452 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf("EndBlock"); resetDepth();}
#line 2287 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 454 "code.y" /* yacc.c:1646  */
    {(yyval.node) = (yyvsp[0].node);}
#line 2293 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 455 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf((yyvsp[0].text));}
#line 2299 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 455 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_leaf("none");}
#line 2305 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 456 "code.y" /* yacc.c:1646  */
    {insertRecord("Func_Name", (yyvsp[0].text), (yylsp[0]).first_line, currentScope);}
#line 2311 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 456 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_for_node("Func_def", make_leaf((yyvsp[-6].text)), (yyvsp[-3].node), (yyvsp[0].node), make_leaf("NULL"));}
#line 2317 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 457 "code.y" /* yacc.c:1646  */
    {(yyval.node) = make_node((yyvsp[-3].text), (yyvsp[-1].node), NULL);}
#line 2323 "y.tab.c" /* yacc.c:1646  */
    break;


#line 2327 "y.tab.c" /* yacc.c:1646  */
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
#line 459 "code.y" /* yacc.c:1906  */


void yyerror(const char *msg)
{
	//printSTable();
	printf("\n\n%s", msg);
	printf("\nSyntax Error at Line %d, Column : %d\n",  yylineno, yylloc.last_column);
	exit(0);
}

