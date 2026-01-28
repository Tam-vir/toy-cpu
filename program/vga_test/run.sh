python ./assembler/assembler.py ./program/vga_test/program.asm ./program/vga_test/program.hex && \
g++ main.cpp ./cpu/cpu.cpp ./vga/vga.cpp -o main \
    -lSDL2 -lSDL2main && \
./main ./program/vga_test/program.hex ./memory/data.txt
