# The Organ Project

*The Organ Project* turns a pair of keyboards and a pedalboard from an Allen MDC 20 organ into a USB MIDI controller for use with Virtual Pipe Organ software or other MIDI applications. It uses an Arduino Due for its large I/O capabilities and native USB support. The project retains the organ's original wiring, scanning the keys and pedals and converting them into MIDI signals for a connected computer.

## Overview

This project allows your Allen MDC 20 organ to interface with MIDI-compatible software, including Virtual Pipe Organ (VPO) systems, by scanning the organ's keyboards and pedals. It outputs MIDI data via USB, making it compatible with modern music software.

## Features

- **MIDI output over USB:** Sends note data to MIDI software or devices.
- **Fast:** Scans the full organ in ~2ms, enabling a theoretical 500Hz scan rate.
- **Arduino Due-based:** Uses the Due’s I/O pins and native USB support.
- **Open Source:** Free for customization and improvement.

## Virtual Pipe Organ

A Virtual Pipe Organ (VPO) simulates the sound of traditional pipe organs through software. This project enables your classic organ to control a VPO setup, turning it into a fully functional digital pipe organ. Using VPO software like Hauptwerk or GrandOrgue, you can play authentic pipe organ sounds directly from your physical organ.

## License

This project is licensed under the GNU General Public License - see the LICENSE file for details.

## Future Goals

The project is continuously evolving. Future goals include:
- [ ] Support for more types of keyboard matrixes.
- [ ] Expanding to support additional organ models.
- [ ] Enhanced customization: Allowing more flexible configuration of MIDI mappings and organ features.
- [ ] Additional MIDI controls: Implementing support for stops, pistons, and expression pedals.
- [ ] Better documentation: Clearer guides and diagrams for easier setup and adaptation.
