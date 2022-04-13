CC = gcc
OBJFile = parsererr
OBJBase = \
	main.o\
	parser.o\
	scanner.o\
	symtab.o

parsererr: $(OBJBase)
	$(CC) -o $(OBJFile) $(OBJBase)

$(OBJBase):micro.h

clean:
	rm $(OBJBase)