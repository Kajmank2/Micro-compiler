/* C version of the Micro parser with error recovery */


/* declared in parsererr.c */

extern Token Lookahead;
extern int ErrorCount;

Token GetNextToken();
void ProgramProc();
void InitSymbolSets();


/* declared in scanner.c */

extern int LineCount;
