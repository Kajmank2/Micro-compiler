CC = cc
OBJFILE = main.o scanner.o symtab.o parser.o

scanner: $(OBJFILE)
	$(CC) -o scanner $(OBJFILE)

$(OBJFILE): micro.h

clean:
	rm *.o
