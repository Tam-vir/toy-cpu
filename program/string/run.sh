python ./assembler/assembler.py ./program/string/program.asm ./program/string/program.hex && \
g++ main.cpp ./cpu/cpu.cpp ./vga/vga.cpp -o main \
    $(sdl2-config --cflags) \
    $(sdl2-config --libs) -lSDL2 -lSDL2main && \
./main ./program/string/program.hex ./memory/data.txt
