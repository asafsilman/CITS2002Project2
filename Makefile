myshell: parser.o execute.o	globals.o myshell.h
	cc -std=c99 -Wall -Werror -pedantic -o myshell myshell.c parser.o execute.o globals.o myshell.h

parser.o: parser.c myshell.h
	cc -std=c99 -Wall -Werror -pedantic -c -o parser.o parser.c

execute.o: execute.c myshell.h
	cc -std=c99 -Wall -Werror -pedantic -c -o execute.o execute.c

globals.o: globals.c myshell.h
	cc -std=c99 -Wall -Werror -pedantic -c -o globals.o globals.c

clean:
	rm -f myshell parser.o execute.o globals.o