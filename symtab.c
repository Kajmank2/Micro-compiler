#include <stdio.h>
#include <stdlib.h>
#include "micro.h"
#include <string.h>

void InitSymTab()
{ 
   int i;
 
   for(i = 0; i < TABSIZE; i++)
      SymTab[i] = NULL;
}

/* a simple hash function used here */
int Hash(IdString S)
{
    int i, ind;
    ind = 0;
    for (i = 0; i < strlen(S); i++)
        ind = ALPHA * ind + S[i];
        
    return abs(ind) % TABSIZE;
}



/* Is S in the symbol table? If S is in the table, the pointer */
/* to the node containing S is returned. Otherwise, NIL is returned */
NodePtr LookUp(IdString S)
{
    NodePtr Cur;
    Cur = SymTab[Hash(S)];
    while (Cur != NULL)
    {
        if (strcmp(Cur->Name, S) == 0)
            return Cur;
        Cur = Cur->Next;
    }
    return NULL;
}


/*  Enter S into the symbol table */
NodePtr Enter(IdString S, Token Code)
{
    NodePtr Item;
    int ind;
    
    ind = Hash(S);
    Item = (struct IdNode*)malloc(sizeof(struct IdNode));
    Item->Name = (char*)malloc(strlen(S) + 1);
    strcpy(Item->Name, S);
    Item->Code = Code;
    Item->Next = SymTab[ind];
    SymTab[ind] = Item;
    
    return Item;
}


/* for debugging */
void ListSymTab()
{
    int i;
    NodePtr Head;
    
    printf("\n\n--------- Symbol Table ----------\n");
    for (i = 0; i < TABSIZE; i++)
    {
        if ((Head = SymTab[i]) != NULL)
            printf("\n%4d : ", i);
        while (Head != NULL)
        {
            printf("\t %s", Head->Name);
            Head = Head->Next;
        }
    }
    printf("\n");
}


