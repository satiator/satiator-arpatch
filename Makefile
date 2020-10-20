OBJECTS := trampoline.o init.o ar_original_trampoline.o satiator.o rom.o pad.o

OBJFILES := $(addprefix out/, $(OBJECTS))

CROSS_COMPILE ?= sh-none-elf-

CFLAGS := -fno-PIC -no-pie -Wall -ggdb -O2 -m2 -nostdlib -ffunction-sections -fdata-sections -ffreestanding

VERSION ?= $(shell git describe --always --dirty --match aotsrintsoierats) $(shell date +%y%m%d%H%M%S)
CFLAGS += -DVERSION='"$(VERSION)"'

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

AR_SRC_BIN := ARP_202C.BIN

$(shell mkdir -p out)

out/ar_patched.bin: out/arpatch.srec patch_bin.py
	./patch_bin.py $< $(AR_SRC_BIN) $@

out/%.srec: out/%.elf
	objcopy -O srec $< $@

out/arpatch.elf: $(OBJFILES)
	$(LD) -Tarpatch.ld -Map=out/arpatch.map --gc-sections -o $@ $^

out/ar_original_trampoline.o: $(AR_SRC_BIN)
	dd if=$< of=out/ar_original_trampoline.bin bs=1 skip=3840 count=68
	$(LD) -r -b binary -o $@ out/ar_original_trampoline.bin

out/%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

out/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf out/

.PHONY: default clean
