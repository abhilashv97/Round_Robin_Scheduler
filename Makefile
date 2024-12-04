
PROJECT=main
CPU ?= cortex-m4
BOARD ?= stm32l476rg

# Object files
OBJS = startup.o main.o scheduler.o

# Rules
all:
	# Assemble and compile source files
	# arm-none-eabi-gcc -mthumb -mcpu=$(CPU) -ggdb -Wall -Wextra -c startup.c -o startup.o
	# arm-none-eabi-gcc -mthumb -mcpu=$(CPU) -ggdb -Wall -Wextra -c main.c -o main.o
	# arm-none-eabi-gcc -mthumb -mcpu=$(CPU) -ggdb -Wall -Wextra -c scheduler.c -o scheduler.o

	arm-none-eabi-gcc -mthumb -mcpu=$(CPU) -g3 -O0 -Wall -Wextra -c startup.c -o startup.o
	arm-none-eabi-gcc -mthumb -mcpu=$(CPU) -g3 -O0 -Wall -Wextra -c main.c -o main.o
	arm-none-eabi-gcc -mthumb -mcpu=$(CPU) -g3 -O0 -Wall -Wextra -c scheduler.c -o scheduler.o

	# Link object files into an ELF executable
	arm-none-eabi-ld -Tlinker_file.ld $(OBJS) -o $(PROJECT).elf -Map=$(PROJECT).map

	# Generate disassembly for debugging
	arm-none-eabi-objdump -D -S $(PROJECT).elf > $(PROJECT).elf.lst

	# Flash the binary to the STM32 device using OpenOCD
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32l4x.cfg

gdb:
	# Start GDB for debugging
	gdb-multiarch -q $(PROJECT).elf -ex "target remote localhost:3333"

clean:
	# Clean up generated files
	rm -rf *.out *.elf *.gdb_history *.lst *.debug *.o *.map *.gdb_history
