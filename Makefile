all:
	gcc -I SDL/include -L SDL/lib -o src/main main.c -lmingw32 -lSDL2main -lSDL2