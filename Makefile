OBJECTS := trampoline.o init.o

OBJFILES := $(addprefix out/, $(OBJECTS))

CROSS_COMPILE ?= sh-none-elf-
CFLAGS := -fno-PIC -no-pie -Wall -ggdb -O2 -m2 -nostdlib

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

$(shell mkdir -p out)

out/ar_patched.bin: out/arpatch.srec patch_bin.py
	./patch_bin.py $< ARP_202C.BIN $@

out/%.srec: out/%.elf
	objcopy -O srec $< $@

out/arpatch.elf: $(OBJFILES)
	$(LD) -Tarpatch.ld -Map=out/arpatch.map -o $@ $^

out/%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

out/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf out/

.PHONY: default clean
