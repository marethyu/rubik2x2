CC = em++

all: rubik_sdl_only.cpp
	$(CC) -O2 rubik_sdl_only.cpp -o index.html -s USE_SDL=2 --shell-file minimal.html
