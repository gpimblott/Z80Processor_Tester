# Z80Processor_Tester
This is a simple tester PCB for Z80 processors.  I use it for testing processors taken from ZX81 and Spectrums that I am fixing... also to test processors that I buy online.

## Software

The software is written in MPLab X to be used on a PIC 16F690 microcontroller

## Hardware

The PCB is designed in KiCad and all the necessary files are provided to create your own version

# Versions and updates

## v01 : Hardware and Software

Initial version that cycles through the address by returning a NOP instructuction (all zeros).  The leds will cycle through a binary sequence until the ram refresh kicks
in then it will skip about.  This is still enough to test the basic operation of the chip and has enabled me to verify several dead Z80's.

This is complete and I have had boards made at PCBWay using the .zip file included in the hardware/v01 directory.

# v02 : Hardware and Software - WIP

Do NOT use the hardware or software - this is WIP

This is an updated version that adds the following
* filters the DRAM refresh cycles
* Stop / Start buttons
