# PIC32-debug-tool

A PIC32-based PIC32 programmer &amp; more.

## Overview

This projects aims to create an open-source PIC32-based USB adapter, useful for flashing, communicating, and eventually even debugging PIC32 chips.

The entire toolchain is open source
* The MCU definitions and how to setup an environment can be found here: [Toolchain](https://gitlab.com/rhn/pic32-parts-free)
* The compiler is standard upstream GCC for mipsel. You can find how to set it up in the link above
* The USB stack is [M-Stack](https://github.com/signal11/m-stack)

## Currently implemented features

A Makefile is available, where you can change just the MCU define, and the C code has ifdefs for the supported HW. The currently tested targets are a PIC32MX270F256D chip, and a PIC32MX440F256H chip.

The USB code works on both chips above. Currently implemented is a very basic USB-UART converter @ 115200.

Schematics and connections to be added as project progresses.

### FYI

The Makefile currently expects the .S files to be uppercase, while the toolchain has them in lowercase.
