#define FALSE   0
#define TRUE    1
#define EOL '\n'
#define EOS '\0'

                     /* size definitions */

#define ID_STRING_LENGTH      32   /* max length of a line */
#define MAX_LINE_LENGTH       132  /* max length of a line */
#define TOKEN_SIZE            32   /* max length of a token */
#define NUM_WORDS             11   /* number of reserved words */


typedef enum 
{
   BeginSym, DoSym, EndSym, IfSym, IntegerSym, ProgramSym, ReadSym, 
   RealSym, ThenSym, VarSym, WhileSym, WriteSym, EofSym, ErrorSym, Id, 
   IntNum, FloatNum, Plus, Minus, Times, Slash, Less, Equal, Greater,
   LessEq, NotEqual, GreaterEq, Assign, SemiColon, Comma, Colon, 
   Period, LParen, RParen
} Token;


typedef char IdString[ID_STRING_LENGTH+1];

/*  SYMBOL TABLE */

#define TABSIZE 211
#define ALPHA   123

struct IdNode
{
    char* Name;
    Token Code;
    struct IdNode *Next;
};



typedef struct IdNode *NodePtr;

extern NodePtr SymTab[TABSIZE];

extern NodePtr LookUp(IdString);
extern NodePtr Enter(IdString, Token);

/*  The GLOBAL VARIABLES CONTAINING LEXEMES */
extern int LastSymbol;  /* must be initialized to 0 */
extern int LineLength;  /*Error Recover */
extern IdString IntLexeme;      /* the lexeme for an integer */
extern IdString FloatLexeme;    /* the lexeme for a real */
extern NodePtr IdVal;           /* the lexeme for an identifier */
extern IdString TokenBuffer;
extern int LinePtr;


