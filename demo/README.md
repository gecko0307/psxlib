# PSXLib Demo

A simple PSXLib use case: loading a sprite sheet image and animating it.

To build the demo, you need the following tools:
- [Psy-Q SDK 4.6](http://www.psxdev.net/downloads.html)
- [CPE2PSX](https://www.romhacking.net/utilities/869/) executable converter
- Lameguy64's [MKPSXISO](https://github.com/Lameguy64/mkpsxiso) to build the CD image

Under Windows, given all the tools are in PATH, and Psy-Q is installed in `C:\psyq`, ready-to-run disk image (`build/game.bin` + `build/game.cue`) can be created by running `build.bat` script.

To run the demo, you need a PlayStation emulator. Recommended Open Source emulators are [RetroArch](https://www.retroarch.com/) with [PCSX ReARMed core](https://github.com/notaz/pcsx_rearmed) and [DuckStation](https://github.com/stenzek/duckstation). With DuckStation, it is advisable to run with Information log level to see TTY output in the console.
