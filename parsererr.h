/* C version of the Micro parser with error recovery */


/* declared in parser.c */

extern Token Lookahead;
extern int ErrorCount;

Token GetNextToken();
void ProgramProc();
void InitSymbolSets();
void TableErrorView();

/* declared in scanner.c */

extern int LineCount;
