## About
cmmk_ctrl is a simple Qt based RGB control application for CoolerMaster's MasterKey keyboards.

![Alt text](/screenshots/main.png?raw=true "Main window")<Paste>

## Disclaimer
This project is neither endorsed by, nor supported by, nor known to CoolerMaster.  As usual with
these sorts of projects, there is no warranty or liability should your device catch fire or eat
your pet.  Should that happen though, do feel free to file a bug report.


## Device Support
cmmk_ctrl supports all the devices that are supported by [libcmmk](https://github.com/chmod222/libcmmk).

A current limitation is the missing last row of configurable LEDs on the MK750, the rest of the
device should work as expected though.

## Features
* Control over the predefined effects with the exception of Multilayer mode (supported in
  the library, but still lacks a somewhat intuitive UI)
* Manual mode
* Saving the profile configuration back to the device

## Build Instructions
The build system is a bit messy because of the libcmmk depedency, cmmk_ctrl uses a standard makefile
which compiles the dependency and then the main application.  The PREFIX variable convention is
respected.

Example:

    # Use absolute paths so cmmk_ctrl can locate the image assets no matter where it's run
    make PREFIX="$PWD/dist"
    make PREFIX="$PWD/dist" install
