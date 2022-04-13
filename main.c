#include "micro.h"
#include <stdio.h>
#include "parsererr.h"

int main() {
  int i;
  Token cur_token;

  LineCount = 0;  /* number of newlines seen so far */
  ErrorCount =0;  /* number of errors seen so far */
  LastSymbol = 0; /* Symbol table initialisation */
  LoadKeywords(); /* Load reserved words */
  GetNewLine();   /* Read the first line */

  Lookahead = GetNextToken();  /* parser initialization */
  //InitSymbolSets(); /* error recovery initialisation */
  ProgramProc();   /* start the parse */

  if ( ErrorCount == 0 )
        printf("Successful Compilation.\n");
    else
      printf(" \n %d errors detected.\n", ErrorCount);
      printf("\n ----------ERORR TABLE ----------");
      TableErrorView();

  ListSymTab();
}


