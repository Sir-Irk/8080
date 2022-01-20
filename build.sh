
WARNING_SUP="-Wno-unused-function -Wno-unused-variable -Wno-missing-braces"
LIBS="-lglfw -lGLU -lGL -lm"
FLAGS="-O0 -Wall -fno-math-errno -march=native"
clang platform.c $FLAGS -o game.exe $LIBS $WARNING_SUP 
