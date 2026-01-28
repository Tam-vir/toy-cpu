python ./assembler/assembler.py ./program/math/program.asm ./program/math/program.hex && \
g++ main.cpp ./cpu/cpu.cpp ./vga/vga.cpp -o main \
    -lSDL2 -lSDL2main && \
./main ./program/math/program.hex ./memory/data.txt
