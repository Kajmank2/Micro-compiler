A Micro SCANNER -- C IMPLEMENTATION

Each program must start with phrase:
"program <ident>;"
if fail program return err;

FILES:

      README ...... This file
      micro.h .... The global header file
      scanner.c ... The scanner functions
      parser.c ... The parser functions
      symbol.c .... The symbol table management functions 
      main.c ...... The driver main program to test the scanner
      parser ..... The executable version of parser.c
      makefile .... The makefile for making the executable "scanner"

To execute ``parser'', type

     parser < test.in > test.out
