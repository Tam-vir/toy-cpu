python ./assembler/assembler.py ./program/vga_line/program.asm ./program/vga_line/program.hex && \
g++ main.cpp ./cpu/cpu.cpp ./vga/vga.cpp -o main \
    -lSDL2 -lSDL2main && \
./main ./program/vga_line/program.hex ./memory/data.txt
