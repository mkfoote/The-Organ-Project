# The Organ Project

This project scans a pair of keyboards as well as the pedal from an Allen MDC 20 organ and output notes via MIDI USB to an attached computer. An Arduino Due is used in this project becasue it has a large number of I/O pins, as well as native USB capabilities. The organ's original wiring is retained.

## Wiring

#### Swell
Referring to the pin numbers on Allen's schematic for this organ, the Swell is scanned by sequentially bringing pins 52, 63, 69, 54, 64 and 62 LOW (Arduino pins 22 - 27). 
Pins 16, 20, 18, 22, 24, 26, 28, 30, 32, 34 and 36 (Arduino pins 36 - 46) are then scanned to determine which keys are closed. LOW = switch closed. Output on Ch. 1

#### Great
Referring to the pin numbers on Allen's schematic for this organ, the Great is scanned by sequentially bringing pins 48, 53, 57, 55, 59 and 61 LOW (Arduino pins 28 - 33). 
Pins 16, 20, 18, 22, 24, 26, 28, 30, 32, 34 and 36 (Arduino pins 36 - 46) are then scanned to determine which keys are closed. LOW = switch closed. Output on Ch. 2

#### Pedal
Referring to the pin numbers on Allen's schematic for this organ, the Pedal is scanned by sequentially bringing pins 67, 65, 58, 60, 68 and 66 LOW (Arduino pins 14 - 19). 
Pins 16, 20, 18, 22, 24, 26, and 28 (Arduino pins 47 - 53) are then scanned to determine which keys are closed. LOW = switch closed. Output on Ch. 3

Note! the wires running to pins 16, 20, 18, 22, 24, 26, and 28 on the pedal are to be detached from the orresponding pins on the Great and Swell.

#### Pistons
The Allen original piston inputs for 1, 2, 3, 4, 5 and GC are Arduino pins 0 - 5. Output on Ch. 5

A new row of piston inputs for 1 - 10, GC, and set are matrixed. Scanned by sequentially bringing arduino pins A5 - A7 (59 - 61) LOW. Arduino pins A8 - A11 (62 - 65) are then scanned to determine which buttons are pressed.
