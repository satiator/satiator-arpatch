OBJECTS := trampoline.o init.o ar_original_trampoline.o satiator.o pad.o

OBJFILES := $(addprefix out/, $(OBJECTS))

CROSS_COMPILE ?= sh-none-elf-

CFLAGS := -fno-PIC -no-pie -Wall -ggdb -O2 -m2 -nostdlib -ffunction-sections -fdata-sections -ffreestanding

VERSION ?= $(shell git describe --exact-match --tags --dirty || (echo -n git-; git describe --always --dirty))
VERSION_STR ?= $(VERSION) $(shell date +%y%m%d%H%M%S)
CFLAGS += -DVERSION='"$(VERSION_STR)"'

OUT_FILE=ar_patched-$(VERSION)

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy

AR_SRC_BIN := ARP_202C.BIN

$(shell mkdir -p out)

out/$(OUT_FILE).bin: out/arpatch.srec patch_bin.py
	# Saves go in the second half of the cart, so we only use the first half
	./patch_bin.py $< $(AR_SRC_BIN) $@ 0x20000

out/%.srec: out/%.elf
	$(OBJCOPY) -O srec $< $@

out/arpatch.elf: arpatch.ld $(OBJFILES)
	$(LD) -Map=out/arpatch.map --gc-sections -o $@ -T$^

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
