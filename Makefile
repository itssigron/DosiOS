all:	 clean DiskImages/hareldolev-os.img clean

clean:
	rm -rf  $(shell find Kernel/ -type f -name '*.o')
	rm -rf  $(shell find Kernel/ -type f -name '*.bin')
	rm -rf  $(shell find Bootload/ -type f -name '*.bin')

DiskImages/hareldolev-os.img: image
	./run.sh

image: Bootload/stage0.bin Bootload/Bootloader.bin Kernel/kernel.bin
	mkdir -p DiskImages
	dd if=/dev/zero of=DiskImages/hareldolev-os.img bs=1024 count=3354
	dd if=Bootload/stage0.bin of=DiskImages/hareldolev-os.img conv=notrunc
	dd if=Bootload/Bootloader.bin of=DiskImages/hareldolev-os.img bs=512 seek=1 conv=notrunc
	dd if=Kernel/kernel.bin of=DiskImages/hareldolev-os.img bs=512 seek=290 conv=notrunc
	

C_SOURCES = $(shell find Kernel/ -type f -name '*.c')
S_SOURCES = $(shell find Kernel/ -type f -name '*.asm')

OBJC = $(C_SOURCES:.c=.o)
OBJS = $(S_SOURCES:.asm=.o)

Kernel/kernel.bin : Kernel/kernel_start.o ${OBJS} ${OBJC}
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -nostdlib Kernel/kernel_start.o $^ -lgcc -g

%.o : %.c
	i686-elf-gcc -g -c $< -o $@ -I./Kernel -std=gnu99 -ffreestanding -Wall -Wextra 

%.o : %.asm
	nasm $< -f elf32 -o $@

%.obj : %.asm
	nasm $< -f elf32 -o $@

%.bin : %.asm
	nasm -O0 -f bin -o $@ $<

%.obj : %.c
	i686-elf-gcc -g -c $< -o $@ -ffreestanding -std=gnu99 -Wall -Wextra 

