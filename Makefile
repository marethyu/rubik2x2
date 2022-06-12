CC = em++

all: rubik_sdl_only.cpp
	$(CC) -O2 rubik_sdl_only.cpp -o index.html -s USE_SDL=2 --shell-file minimal.html

test:
	$(CC) -O2 rubik_sdl_only.cpp -o index.html -s USE_SDL=2

exe:
	g++ rubik_sdl_only.cpp -o rubik_sdl_only -std=c++14 -lSDL2
