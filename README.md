# What is this?

The stock Action Replay firmware for the Sega Saturn doesn't play nice with a Satiator cartridge: you can't boot into the Satiator menu.
This repo contains a patch to the AR firmware to make it behave.
It's based on the AR 2.02C firmware.

# How do I install it?

- Copy `ar_patched.bin` from `out/` to your Satiator SD card
- Remove the AR cart from your Saturn
- Boot into the Satiator menu (version 53 or higher)
- Press C to exit to the main menu
- Select Action Replay tools
- Insert your AR cart. Do this straight, not at an angle, and you won't have a problem.
- Back up your current AR firmware (if you haven't already)
- Flash the new `.bin` file to the cartridge

# How do I use it?

If a Satiator is present, it will boot to the Satiator menu instead of to the AR menu.

If you hold A on startup, it will boot to the AR menu instead.

If there's no Satiator installed, it will behave like a stock AR cart.

# Building

Simply run `make`.

The Makefile assumes that a SuperH GCC is available as `sh-none-elf-gcc`.
Ensure this is on your `PATH` and if necessary adjust the `CROSS_COMPILE` variable to suit your compiler.

Python 3 is required to apply the built patch to the binary.

At the time of writing, the release versions are built with GCC version 9.2.0.

For building on Windows some changes may be needed to the Makefile.
For example, `dd` is not typically available - this could be replaced with more Python.
Patches welcome.

# Hacking / how does it work?

The original AR firmware starts executing at offset 0xf00 (at the end of the IP).
The first thing it does is copy itself into low work RAM, and then jumps into the loaded copy.

The patch building process does the following things:

1. Compile startup hook code into an unused spot in the ROM - defined as `cart_main` in `arpatch.ld`.
    The hook code does Satiator detection, booting, etc.; see `init.c`.
2. Save a copy of the original cart's startup code at 0xf00 (as `ar_original_trampoline.bin`).
3. Build a new trampoline to go at 0xf00 - defined as `cart_header` in `arpatch.ld`.
    This trampoline copies the hook to low work RAM (`work_low`) and jumps into it it.

The hook code decides whether to do a Satiator boot or AR boot.
For an AR boot, it copies everything to low work RAM, just like the original trampoline, and then jumps in.
The reason for stashing the original trampoline is that the main AR code will crash if it's not copied into low work RAM along with everything else,
so we need to restore it before jumping into the AR code.
See `boot_ar()` in `rom.c`.
