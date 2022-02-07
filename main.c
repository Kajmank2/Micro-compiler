#include "micro.h"

#include <stdio.h>
Token GetNextToken();
void ProgramProc();
extern int LineCount;

int main() {
  int i;
  Token cur_token;
  extern Token Lookahead;

  LineCount = 0;  /* number of newlines seen so far */
  LastSymbol = 0; /* Symbol table initialisation */
  LoadKeywords(); /* Load reserved words */
  GetNewLine();   /* Read the first line */

  Lookahead = GetNextToken();  /* parser initialization */
  ProgramProc();   /* start the parse */

  printf("Successful Compilation\n");

  ListSymTab();
}


