python ./assembler/assembler.py ./program/vga_chk/program.asm ./program/vga_chk/program.hex && \
g++ main.cpp ./cpu/cpu.cpp ./vga/vga.cpp -o main \
    $(sdl2-config --cflags) \
    $(sdl2-config --libs) -lSDL2 -lSDL2main && \
./main ./program/vga_chk/program.hex ./memory/data.txt
