# gcc src/main.c -o dist/main.exe
# gcc src/gui.c -o gui.exe -mwindows
# gcc src/gui.c -o gui.exe

gcc src/main.c src/gui.c -o dist/main.exe