<h1 align="center"> BLEFI Device</h1>
<p align="center">
  ESP32-based wireless exploration platform for WiFi & BLE systems
</p>

---

## Overview

**BLEFI Device** is an embedded systems project built using the ESP32 and ESP-IDF.

The project is designed as a **modular wireless experimentation platform**, starting with WiFi packet sniffing and expanding toward BLE communication, low-power operation, and real-world IoT applications.

---

## Current Features

- WiFi packet sniffing (promiscuous mode)
- Basic UI framework for device interaction
- Button input handling system

---

## Hardware

- ESP32 (WROOM32 module)
- USB interface for programming and communication
- Button-based input system
- Integrated power management (battery + regulation)

---

## Hardware Design

### PCB Layout
<p align="center">
  <img src="assets/blefi_pcb.png" width="800">
</p>

Custom PCB designed in KiCad integrating ESP32, power management, USB interface, and user input system.

### Schematic
<p align="center">
  <img src="assets/blefi_schematic.png" width="900">
</p>

System schematic showing power architecture, USB interface, MCU connections, and peripheral subsystems.

---

## Firmware Demo

> *(Add short demo video link here)*

https://youtube.com/upload-video-tomorrow

---

## Project Goals

This project serves as a platform to explore:

- Wireless communication (WiFi + BLE)
- Embedded UI design
- Low-power IoT systems
- Data capture and analysis

---

## Planned Features

- Expand WiFi functionality (promiscuous mode)
- BLE scanning and communication
- SD card logging
- Low-power / sleep modes
- Improve UI navigation

## Hardware Status

- Prototype running on ESP32 dev board
- Transitioning to custom PCB design

---
