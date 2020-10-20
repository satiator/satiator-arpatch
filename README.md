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
