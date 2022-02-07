/* C version of the scanner for the Micro language */
 
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

Each token is represented by a pair of two components <type, lexeme>.
When called, the scanner function GetNextToken returns the token type. 
In the case of integers, reals and identifiers, the corresponding 
lexemes are left in the following three global variables:

     IntLexeme:    the lexeme for the current integer
     FloatLexeme:  the lexeme for the current real
     IdVal:        the pointer to the node containing the current id

There three global variables will be used by the code generator
to perform the actual translation.

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "micro.h"

/* Beginning of scanner declarations */

IdString TokenBuffer;
int TokenPtr;

NodePtr IdVal;           /* the symbol-table entry for an ID */
IdString IntLexeme;   /* the lexeme for an INT literal    */
IdString FloatLexeme; /* the lexeme for an INT literal    */
int LastSymbol;       /* must be initialized to 0 */
NodePtr SymTab[TABSIZE];

char LineBuffer[MAX_LINE_LENGTH + 1];
int LineLength, LinePtr, LineCount;

int ErrorNum;
char EofFlag;


/* Convert characters to lower case characters */
void ToLowerCase()
{
    int i;
    
    for (i = 0; i < LineLength; i++)
        LineBuffer[i] = tolower(LineBuffer[i]);
}



void Listing()
{
    int i;

    printf("\n");
    printf("%d ", LineCount);
    for (i = 0; i < LineLength; i++)
        printf("%c", LineBuffer[i]);
    printf("\n");
}


/* print an error messgage */
void LexicalError (char c)
{
    printf("\n***** Lexical error: line %d at %c\n", LineCount, c);
}


/* get a new input line */
void GetNewLine()
{

    LineCount++;

    LineLength = 0;
    while ((EofFlag = getchar()) != EOL && EofFlag != EOF)
        LineBuffer[LineLength++] = EofFlag;

    LineBuffer[LineLength++] = ' ';

    if (EofFlag != EOF)
        Listing();

    LinePtr = -1;

    ToLowerCase();
}


void GetNextChar(char* c)
{
    if (LinePtr + 1 >= LineLength)
        GetNewLine();
    *c = LineBuffer[++LinePtr];
}


void PutNextChar()
{
    LinePtr -= 1;
}


void ClearLineBuffer()
{
    LinePtr = -1;
    LineLength = 0;
}


void ClearTokenBuffer()
{
    TokenPtr = 0;
    TokenBuffer[0] = EOS;
}


void BufferChar(char c)
{
    TokenBuffer[TokenPtr++] = c;
}


void BufferName(char c)
{
    char Inspect;

    BufferChar(c);
    GetNextChar(&Inspect);
    while (isalnum(Inspect) || Inspect == '_')
    {
        BufferChar(Inspect);
        GetNextChar(&Inspect);
    }
    PutNextChar();
    BufferChar(EOS);
}

Token BufferLiterals(char c)
/* recognise integer and real literals */
{
    char Inspect, Inspect2;

    BufferChar(c);
    GetNextChar(&Inspect);
    while (isdigit(Inspect))
    {
        BufferChar(Inspect);
        GetNextChar(&Inspect);
        }
    if (Inspect == '.')   /* digits followed by a . */
    {
        GetNextChar(&Inspect2);
        if( isdigit(Inspect2) ) /* followed by digits. Its a real */
        {
            BufferChar(Inspect);
            BufferChar(Inspect2);
            GetNextChar(&Inspect);
            while (isdigit(Inspect))
            {
                BufferChar(Inspect);
                GetNextChar(&Inspect);
                }
            PutNextChar();
            BufferChar(EOS);
            return FloatNum;
            }
        else             /* digits. not followed by digits. An init and a . */
        {
            PutNextChar(); /* return inspect and inspect2 to input buffer */
            PutNextChar(); 
            BufferChar(EOS);
            return IntNum;
            }
        }
    else		/* digits not followed by . Its an int */
    {
        PutNextChar(); 
        BufferChar(EOS);
        return IntNum;
        }
    }


Token GetNextToken()
{
    char CurrentChar,
    Inspect;
    Token CurToken;
    NodePtr PosPtr;

    ClearTokenBuffer();

    while (EofFlag != EOF)
    {
        GetNextChar(&CurrentChar);
        if (isspace(CurrentChar))
        {
            /* skip white spaces */
        }
        else if (isalpha(CurrentChar))
        {
            BufferName(CurrentChar);
            PosPtr = LookUp(TokenBuffer);
            if (PosPtr == NULL)
                PosPtr = Enter(TokenBuffer, Id);
            IdVal = PosPtr;
            return IdVal->Code;
        }
        else if (isdigit(CurrentChar))
        {
            CurToken = BufferLiterals(CurrentChar);
            if (CurToken == IntNum)
                strcpy(IntLexeme, TokenBuffer);
            else if (CurToken == FloatNum)
                strcpy(FloatLexeme, TokenBuffer);
            return CurToken;
        }
        else
        {
            switch (CurrentChar)
            {               /* other tokens */
                case '+':
                    return Plus;
                case '-':
                    GetNextChar(&Inspect);
                    if (Inspect == '-')
                    {
                        GetNewLine();/* skip rest of input line */
                        break;
                    }
                    else
                    {
                        PutNextChar();
                        return Minus;
                    }
                case '*':
                    return Times;
                case '/':
                    return Slash;
                case '<':
                    GetNextChar(&Inspect);
                    if (Inspect == '=')
                        return LessEq;
                    else if (Inspect == '>')
                        return NotEqual;
                    else
                    {
                        PutNextChar();
                        return Less;
                    }
                case '=':
                    return Equal;
                case '>':
                    GetNextChar(&Inspect);
                    if (Inspect == '=')
                        return GreaterEq;
                    else
                    {
                        PutNextChar();
                        return Greater;
                    }
                case '(':
                    return LParen;
                case ')':
                    return RParen;
                case ':':
                    GetNextChar(&Inspect);
                    if (Inspect == '=')
                        return Assign;
                    else
                    {
                        PutNextChar();
                        return Colon;
                    }
                case ';':
                    return SemiColon;
                case ',':
                    return Comma;
                case '.':
                    return Period;
                default:
                    /* let the parser deal with the error */
                    LexicalError(CurrentChar);
                    return ErrorSym;
            }
        }
    }
    return EofSym;              /* EofFlag == EOF */
}


void LoadKeywords()
{
    Enter("begin", BeginSym);
    Enter("do", DoSym);
    Enter("end", EndSym);
    Enter("if", IfSym);
    Enter("integer", IntegerSym);
    Enter("program", ProgramSym);
    Enter("read", ReadSym);
    Enter("real", RealSym);
    Enter("then", ThenSym);
    Enter("var", VarSym);
    Enter("while", WhileSym);
    Enter("write", WriteSym);
}


