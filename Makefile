
######################
# Makefile for Tinix #
######################


# Entry point of Tinix
# It must be as same as 'KernelEntryPointPhyAddr' in load.inc!!!
ENTRYPOINT		= 0x30400
ENTRYPOINTAPP	= 0x1000000
# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET	=   0x400


# Programs, flags, etc.
ASM		= nasm
DASM		= ndisasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32
LDFLAGS		= -s -Ttext $(ENTRYPOINT) -m elf_i386
LDFLAGSAPP	= -s -Ttext $(ENTRYPOINTAPP) -m elf_i386
DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

# This Program
TINIXBOOT	= boot/boot.bin boot/loader.bin
TINIXKERNEL	= kernel.bin
APPS 		= cat.bin time.bin cls.bin
APPS_O 		= app/cat.o app/time.o app/cls.o
APPOBJS 	= lib/string.o kernel/syscall.o kernel/printf.o kernel/vsprintf.o lib/stdlibc.o lib/stdlib.o
OBJS		= kernel/kernel.o kernel/syscall.o kernel/start.o kernel/main.o\
			kernel/clock.o kernel/i8259.o kernel/global.o kernel/protect.o\
			kernel/proc.o kernel/keyboard.o kernel/tty.o service/shell.o kernel/console.o\
			kernel/printf.o kernel/vsprintf.o\
			lib/klib.o lib/klibc.o lib/string.o\
			lib/mem.o lib/stdlibc.o lib/stdlib.o
DASMOUTPUT	= kernel.bin.asm

# All Phony Targets
.PHONY : buildimg everything final image clean realclean disasm all buildimg buildapp

# Default starting position
everything : $(TINIXBOOT) $(TINIXKERNEL)

buildapp : $(APPS)

all : realclean everything buildapp

final : all clean

image : final buildimg

clean :
	rm -f $(OBJS) $(APPOBJS) $(APPS_O)

realclean :
	rm -f $(OBJS) $(TINIXBOOT) $(TINIXKERNEL) $(APPOBJS) $(APPS) $(APPS_O)

disasm :
	$(DASM) $(DASMFLAGS) $(TINIXKERNEL) > $(DASMOUTPUT)

# Write "boot.bin" & "loader.bin" into floppy image "TINIX.IMG"
# We assume that "TINIX.IMG" exists in current folder
buildimg :
	mount TINIX.IMG /mnt/floppy -o loop
	cp -f boot/loader.bin /mnt/floppy/
	cp -f kernel.bin /mnt/floppy/
	cp -f app/*.bin  /mnt/floppy/
	cp -f txt/*.txt  /mnt/floppy/
	umount  /mnt/floppy

boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm boot/include/load.inc boot/include/fat12hdr.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(TINIXKERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(TINIXKERNEL) $(OBJS)

cat.bin : app/cat.o $(APPOBJS) 
	$(LD) $(LDFLAGSAPP) -o app/$@ $< $(APPOBJS) 

cls.bin : app/cls.o $(APPOBJS)
	$(LD) $(LDFLAGSAPP) -o app/$@ $< $(APPOBJS) 

time.bin : app/time.o $(APPOBJS) 
	$(LD) $(LDFLAGSAPP) -o app/$@ $< $(APPOBJS) 

kernel/kernel.o : kernel/kernel.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o: kernel/start.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h include/mem.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o: kernel/main.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h include/mem.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259.o: kernel/i8259.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o: kernel/global.c include/type.h include/const.h include/protect.h include/proc.h \
			include/tty.h include/console.h include/global.h include/proto.h include/mem.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o: kernel/protect.c include/type.h include/const.h include/protect.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o: kernel/clock.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/proc.o: kernel/proc.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/keyboard.o: kernel/keyboard.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/keymap.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o: kernel/tty.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

service/shell.o: service/shell.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/proto.h \
			include/shell.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/console.o: kernel/console.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/tty.h include/console.h include/global.h include/keyboard.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/vsprintf.o: kernel/vsprintf.c include/type.h include/const.h include/string.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/printf.o: kernel/printf.c include/type.h include/const.h
	$(CC) $(CFLAGS) -o $@ $<

app/cat.o: app/cat.c
	$(CC) $(CFLAGS) -o $@ $<

app/cls.o: app/cls.c
	$(CC) $(CFLAGS) -o $@ $<

app/time.o: app/time.c
	$(CC) $(CFLAGS) -o $@ $<

lib/klibc.o: lib/klib.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

lib/stdlibc.o: lib/stdlib.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

lib/stdlib.o : lib/stdlib.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/klib.o : lib/klib.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/mem.o: lib/mem.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/tty.h include/console.h include/global.h include/mem.h
	$(CC) $(CFLAGS) -o $@ $<