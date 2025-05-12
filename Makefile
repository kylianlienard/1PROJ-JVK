CC = gcc
CFLAGS = -I./SDL_2.30.4/include
LDFLAGS = -L./SDL_2.30.4/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
SRC = ./1.0/main.c
OUT = main.exe

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)
