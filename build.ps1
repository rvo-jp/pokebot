# gcc src/main.c -o dist/main.exe
# gcc src/gui.c -o gui.exe -mwindows
# gcc src/gui.c -o gui.exe

# gcc src/pokebot.c src/gui.c -o Pokebot/Pokebot.exe -mwindows
# gcc src/pokebot.c src/gui.c -o Pokebot/Pokebot.exe

# g++ src2/pokebot.cpp src2/bot.cpp src2/main.cpp -o Pokebot2/Pokebot2.exe

g++ src2/pokebot.cpp src2/bot.cpp src2/main.cpp -o Pokebot2/Pokebot2.exe -static -lgdiplus -lole32 -lshlwapi -mwindows
