# PSXLib

PSXLib is a library for homebrew PlayStation 1 development. This project aims to implement fully free and open replacement to proprietary standard library from Psy-Q SDK. It doesn't provide its own MIPS toolchain though, it is meant to be used with Psy-Q CCPSX compiler and corresponding tools.

## Features

Currently PSXLib contains only the most basic functionality:

- GPU initialization and OT management
- Uploading TIM images to VRAM
- Rendering basic 2D primitives - sprites, dots and lines
- Rendering text
- Reading controller input
- TTY output (printf)

PSXLib API doesn't imitate Psy-Q one-to-one. I tried to design it in simpler and more consistent manner. Some functions are inspired by Blade's PSX library.

PSXLib is written in ANSI C with pieces in ASPSX assembler.

## Usage
A simple usage example can be found in the `demo` folder. Keep in mind that this repository doesn't contain third-party utilities needed to build it. You can download a prepackaged version for Windows from Releases page, but you still need to install Psy-Q SDK by yourself.
