/* C version of the parser for the Micro language */
 
/*  Micro grammar

   <program>             -> PROGRAM <name> <optional declaration> BEGIN <optional statement> END.
   <name>                -> <ident> ;
   <optional declaration>-> <declaration list> 
   <optional declaration>-> NULL
   <declaration list>   -> <declaration list> <declaration>
   <declaration list>   -> <declaration>
   <declaration>        -> VAR <id list> : <type> ;
   <id list>            -> <id list> , <ident> 
   <id list>            -> <ident> 
   <type>               -> INTEGER 
   <type>               -> REAL
   <optional statement> -> <statement list> 
   <optional statement> -> NULL
   <statement list>     -> <statement list> <statement>
   <statement list>     -> <statement>
   <statement>          -> BEGIN <statement list> END ;
   <statement>          -> <ident> := <expression> ;
   <statement>          -> READ ( <id list> ) ;
   <statement>          -> WRITE ( <expression list> ) ;
   <statement>          -> IF ( <bool> ) THEN <statement> 
   <statement>          -> WHILE ( <bool> ) DO <statement> 
   <expression list>    -> <expression list>, <expression>
   <expression list>    -> <expression>
   <expression>         -> <expression> + <term> 
   <expression>         -> <expression> - <term> 
   <expression>         -> <term> 
   <term>           -> <term> * <factor> 
   <term>           -> <term> / <factor> 
   <term>           -> <factor> 
   <factor>         -> ( <expression> )
   <factor>         -> <ident>
   <factor>         -> IntNum
   <factor>         -> FloatNum
   <ident>          -> ID 
   <bool>           -> <expression> <relation op> <expression>
   <relop>          -> < 
   <relop>          -> = 
   <relop>          -> > 
   <relop>          -> <= 
   <relop>          -> <>
   <relop>          -> >= 
 */
/* 
<program>               =>           PROGRAM <name> <optional declaration> BEGIN <optional statement> END.
<name>                  =>           <ident> ;
<optional declaration>  =>           <declaration list> | NULL
<declaration list>      =>           <declaration><declaration list tail>
<declaration list tail> =>           NULL | <declaration><declaration list tail>
<declaration>           =>           VAR <id list> : <type> ;
<id list>               =>           <ident><id list tail>
<id list tail>          =>           NULL | ,<ident><id list tail>
<type>                  =>           INTEGER | REAL
<optional statement>    =>           <statement list> | NULL
<statement list>        =>           <statement> <statement list tail>
<statement list tail>   =>           NULL | <statement><statement list tail>
<statement>             =>           BEGIN <statement list> END ;
                                    | <ident> := <expression>;
                                    | READ ( <id list> );
                                    | WRITE ( <expression list> );
                                    | IF ( <bool> ) THEN <statement>
                                    | WHILE ( <bool> ) DO <statement>
<expression list>       =>           <expression><expression list tail>
<expression list tail>  =>           NULL | ,<expression><expression list tail>
<expression>            =>           <term> <expression tail>
<expression tail>       =>           NULL | + <term><expression tail> | − <term><expression tail>
<term>                  =>           <factor><term tail>
<term tail>             =>           NULL | ∗ <factor><term tail> | / <factor><term tail>
<factor>                =>           ( <expression> ) | <ident> | INTNUM | FLOATNUM
<ident>                 =>           ID
<bool>                  =>           <expression> <relation op> <expression>
<relation op>           =>           < | = | > | <= | <> | >=
*/
#include <stdio.h>
#include <stdlib.h>
#include "micro.h"
#include "parsererr.h"

static char* TokenArray[] = {
    "Begin", "Do", "End", "If", "Integer", "Program", "Read", 
   "real", "then", "Var", "while", "Write", "Eof", "Error", "id", 
   "int", "float", "+", "-", "*", "/", "<", "=", ">",
   "<=", "<>", ">=", ":=", ";", ",", ":", 
   ".", "(", ")"
    };

Token  Lookahead;
int ErrorCount;
char *arr[500]; // Array which save All errors
int arrLines[32]; //arr helper Linecount display
int DisplayErrCount=0; // arr helper




/* change int to type whose size is big enough to hold all the Tokens, e.g., long long */
typedef unsigned int SymbolSet;

/* these macros will make life easier */
#define EL(a)      (1 << ((unsigned)a))    /* Element: turns elements to sets, i.e., numbers into bits */
#define IN(a,S)    (((S) & EL(a)) != 0)    /* TRUE if a in set S */

/* First and Follow sets */
SymbolSet ProgramNameStarters,
    StmtStarters, ExprListStarters, ExprStarters,
    FactorStarters, IdListStarters;
    

SymbolSet ProgramNameFollowers, ExprListFollowers,
    ExprFollowers, FactorFollowers, IdListFollowers;


/* Function prototypes */
void IdList();
void Expression();
void ExpressionList();
void ExprListTail();


void TestSymbolSet(SymbolSet S)
// TestSymbolSet
{
    Token    i;    
    printf("--------\n");
    for ( i = BeginSym; i <= ErrorSym; i++ )
        if ( IN(i, S) )
            printf("%s\n", TokenArray[i]);
    printf("--------\n");
}

/* PANIC RECOVERY 
void SyntaxError (Token T)
    {
    printf("##Syntax Error: line %d at token %s \n", LineCount, TokenArray[T]);
    printf("\n ERROR \n");
    exit(1); 
    }
*/
void SyntaxError(Token T)
// SyntaxError
{
    ErrorCount += 1;    /* increment the count and report an error */
    printf("\n error: unexpected symbol '%s'\n",TokenArray[Lookahead]);
} 

void SkipTo(SymbolSet RelevantSymbols)
// SkipTo
{
    while ( !IN(Lookahead, RelevantSymbols) )
        Lookahead = GetNextToken();
} // SkipTo

void InitSymbolSets() //Not Implemented
{
/* The First sets for all nonterminals */
   ProgramNameStarters = EL(Id);

/* The Follow sets for all nonterminals */
    ProgramNameFollowers = EL(SemiColon);


   TestSymbolSet(ProgramNameStarters);
   TestSymbolSet(ProgramNameFollowers);
}

int CheckForStarters(SymbolSet Starters, SymbolSet Followers)
// CheckForStarters
{
    if ( !IN(Lookahead, Starters) )
    {
        SyntaxError(ErrorSym);
        SkipTo(Starters | Followers);
    }
    return IN(Lookahead, Starters);
    
} 


void FindFollowers(SymbolSet Followers)
// FindFollowers
{
    if ( !IN(Lookahead, Followers) )
    {
        SyntaxError(ErrorSym);
        SkipTo(Followers);
    }
}
void TokenErrorPTR() // Marker
{
    for(int i =0; i<LineLength;i=i+1)
    {
        if (i==LinePtr)
            printf("^");
        printf("-");
    }

}

void TableError() // Method which add error to table
{
    int i;
    char **ptr = arr;

    ptr[DisplayErrCount] = TokenArray[Lookahead];
    arrLines[DisplayErrCount] = LineCount;
    DisplayErrCount= DisplayErrCount+1;
}

void TableErrorView() // Method which add error to table
{
    int i;
    int j;
    for ( i = 0,j=0; i < ErrorCount; i++,j++)
        printf("\n Line %d : %s", arrLines[j], arr[i]);
}

void Match (Token T)
{
    if (Lookahead == T)
       Lookahead = GetNextToken();
    else{ // Exception with Token Buffer for ID 
        ErrorCount=ErrorCount+1;
        TokenErrorPTR();
        TableError();
        if(Lookahead==Id)
            printf(" -> symbol '%s' expected but '%s' found \n",TokenArray[T], TokenBuffer);
        else
            printf(" -> symbol '%s' expected but '%s' found\n",TokenArray[T], TokenArray[Lookahead]);
       }
}
void RelationOp()
/*<relation op>  =>  < | = | > | <= | <> | >= */
{
 if(Lookahead==Less)
{
    Match(Less);
}else if (Lookahead == Equal)
{
    Match(Equal);
}else if (Lookahead == Greater)
{
    Match(Greater);
}else if (Lookahead == LessEq)
{
    Match(LessEq);
}else if (Lookahead == NotEqual)
{
    Match(NotEqual);
}else if (Lookahead == GreaterEq)
{
    Match(GreaterEq);
}
}
void Bool()
/* <bool> -> <expression> <relation op> <expression>*/
{
    Expression();
    RelationOp();
    Expression();
}
void Ident()
/* <ident> -> Id  */
{
    Match(Id);
}
void Factor()
/*( <expression> ) | <ident> | INTNUM | FLOATNUM*/
{
    if (Lookahead ==LParen)
    {
        Match(LParen);
        Expression();
        Match(RParen);
    }else if ( Lookahead == Id){
        Ident();
    } else if (Lookahead == IntNum){
        Match(IntNum);
    }else if (Lookahead == FloatNum){
        Match(FloatNum);
    }
}
void TermTail()
/*NULL | ∗ if <factor><term tail> | / <factor><term tail>*/
{
    if (Lookahead==Times)
    {
        Match(Times);
        Factor();
        TermTail();
    }else if (Lookahead==Slash)
    {
        Match(Slash);
        Factor();
        TermTail();
    }
}
void Term()
/* <term>           -> <term> * <factor> 
   <term>           -> <term> / <factor> 
   <term>           -> <factor> */
{
    Factor();
    TermTail();
}
void ExpressionTail()
{
    if (Lookahead== Plus) /*NULL | + <term><expression tail>*/
    {
        Match(Plus);
        Term();
        ExpressionTail();
    }else if (Lookahead == Minus)/*| − <term><expression tail>*/
    {
        Match(Minus);
        Term();
        ExpressionTail();
    }
}
void Expression()
/* <expression>         -> <expression> + <term> 
   <expression>         -> <expression> - <term> 
   <expression>         -> <term> */
{
Term();
ExpressionTail();
}
void ExprListTail()
/* <expr list> -> <expr> <expr list tail> */
{
if(Lookahead==Comma)
    {
    Match(Comma);
    Expression();
    ExpressionList();
    }
}
void ExpressionList()
/* <expression><expression list tail> */
{
Expression();
ExprListTail();
}
void Statement()
/* <statement>          -> BEGIN <statement list> END ; */
    {
        if(Lookahead==BeginSym){
            Match(BeginSym);
            if(Lookahead==Id) // <statement> -> <ident> := <expression> ;
        {
            Ident();
            Match(Assign);
            Expression();
            Match(SemiColon);
        }else if(Lookahead == ReadSym) //<statement> -> READ ( <id list> ) ;
        {
            Match(ReadSym);
            Match(LParen);
            IdList();
            Match(RParen);
            Match(SemiColon);
        }else if (Lookahead == WriteSym) //<statement> -> WRITE ( <expression list> ) ;
        {
            Match(WriteSym);
            Match(LParen);
            ExpressionList();
            Match(RParen);
            Match(SemiColon);
        }else if (Lookahead == IfSym)   //<statement> -> IF ( <bool> ) THEN <statement> 
        {
            Match(IfSym);
            Match(LParen);
            Bool();
            Match(RParen);
            Match(ThenSym);
            Statement();
        }else if (Lookahead == WhileSym)    //<statement> -> WHILE ( <bool> ) DO <statement> 
        {
            Match(WhileSym);
            Match(LParen);
            Bool();
            Match(RParen);
            Match(DoSym);
            Statement();
        }
        }

    if(Lookahead==Id) // <statement> -> <ident> := <expression> ;
        {
            Ident();
            Match(Assign);
            Expression();
            Match(SemiColon);
        }else if(Lookahead == ReadSym) //<statement> -> READ ( <id list> ) ;
        {
            Match(ReadSym);
            Match(LParen);
            IdList();
            Match(RParen);
            Match(SemiColon);
        }else if (Lookahead == WriteSym) //<statement> -> WRITE ( <expression list> ) ;
        {
            Match(WriteSym);
            Match(LParen);
            ExpressionList();
            Match(RParen);
            Match(SemiColon);
        }else if (Lookahead == IfSym)   //<statement> -> IF ( <bool> ) THEN <statement> 
        {
            Match(IfSym);
            Match(LParen);
            Bool();
            Match(RParen);
            Match(ThenSym);
            Statement();
        }else if (Lookahead == WhileSym)    //<statement> -> WHILE ( <bool> ) DO <statement> 
        {
            Match(WhileSym);
            Match(LParen);
            Bool();
            Match(RParen);
            Match(DoSym);
            Statement();
        }
}
void StatmentListTail()
{
    if(Lookahead==Id || Lookahead==ReadSym ||Lookahead==WriteSym ||Lookahead==IfSym || Lookahead==WhileSym)
    {
        Statement();
        StatmentListTail();
    }
}
void StatementList()
/*<statement> <statement list tail>*/
{
    Statement();
    StatmentListTail();
}
void OptionalStatment()
/*<optional statement> -> <statement list>  | Null*/
{
    if(Lookahead == BeginSym || Lookahead==Id || Lookahead==ReadSym ||Lookahead==WriteSym ||Lookahead==IfSym || Lookahead==WhileSym)
    {
        Match(BeginSym);
        StatementList();
        Match(EndSym);
        Match(SemiColon);
    }
}
void Type()
/*<type>-> INTEGER |  REAL*/
{
    if (Lookahead == RealSym){
       Match(RealSym);
    }
    else if (Lookahead == IntegerSym){
       Match(IntegerSym);
    }
}
void IdListTail()
{
    if (Lookahead == Comma){
        Match(Comma);
        Ident();
        IdListTail();
    }
}
void IdList()
/*<ident><id list tail> */
{
    Ident();
    IdListTail();
}
void Declaration()
/*<declaration>  -> VAR <id list> : <type> ; */
{
    Match(VarSym);
    IdList();
    Match(Colon);
    Type();
    Match(SemiColon);
}
void DeclarationListTail()
/* NULL | <declaration><declaration list tail> */
{
    if(Lookahead == VarSym){
        Declaration();
        DeclarationListTail();
    }
}
void DeclarationList()
/*<declaration list>   -> <declaration list> <declaration>
<declaration list>   -> <declaration> */
{
    if(Lookahead==VarSym)
   {
    Declaration();
    DeclarationListTail();
   }
}
void OptionalDeclaration()
/*<optional declaration>-> <declaration list> | NULL */
{
   if(Lookahead==VarSym)
   {
    DeclarationList();
   }
}
void Name()
/* <name> ->Ident */
{
    Ident();
    Match(SemiColon);
}
void ProgramProc()
/*PROGRAM <name> <optional declaration> BEGIN <optional statement> END. */
{
    Match(ProgramSym);
    Name();
    OptionalDeclaration();
    OptionalStatment();
    Match(EndSym);
    Match(Period);
    Match(EofSym);
}
