main: src/main.cpp
	g++ src/main.cpp src/cadev.cpp -Iinclude -Llib -Wall -lmingw32 -lSDL2main -lSDL2 -o main.exe