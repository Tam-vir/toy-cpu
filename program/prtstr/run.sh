python ./assembler/assembler.py ./program/prtstr/program.asm ./program/prtstr/program.hex && \
g++ main.cpp ./cpu/cpu.cpp ./vga/vga.cpp -o main \
    -lSDL2 -lSDL2main && \
./main ./program/prtstr/program.hex ./memory/data.txt
