# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Single-file Arduino sketch (`SerialServoIdProgrammer.ino`) that reassigns the bus ID of Feetech/Waveshare serial servos (ST3020, SMS/STS family). The sketch scans IDs 0–999 on the servo half-duplex bus, prints the first one that responds, and reprograms it to a new ID typed into the USB serial monitor.

## Build / Run

No build system in-repo — open the `.ino` in the Arduino IDE and upload.

- **Target board:** ESP32 (the pin map `S_RXD = 18`, `S_TXD = 19` and `Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD)` are the Waveshare ESP32 Servo Driver defaults; `Serial1.begin` with custom pins is ESP32-only).
- **Library dependency:** `SCServo` (Feetech/Waveshare). Must be installed manually in the Arduino IDE — it is not on the Library Manager registry. The sketch uses the `SMS_STS` class.
- **Serial Monitor settings (required for the programming step to work):** 115200 baud, line ending = **Carriage Return** (CR) only. The input parser at `SerialServoIdProgrammer.ino:52` terminates the ID string on `\r`; using NL or CRLF will break it.
- **Servo bus baud:** 1,000,000 (hard-coded). SMS/STS servos ship at this rate; ST3020 in SCS mode does too. Do not change without matching the servo's configured baud.

## How the flow works

`loop()` has two states, gated by `oldID`:

1. **Scan** (`oldID == -1`): pings IDs 0..999 sequentially. First `Ping()` that returns non-`-1` wins — `oldID` is set and the loop returns early to wait for input. The current code stores `i` (the loop counter) into `oldID` rather than the value returned by `Ping`; in normal operation these are equal, but be aware of this if you touch the scan.
2. **Reprogram** (`oldID != -1`): buffers serial input into `input[]` until `\r`, parses with `atoi`, then runs the EPROM unlock → `writeByte(oldID, SMS_STS_ID, newID)` → lock sequence. After success both IDs are reset to `-1` so the next loop iteration rescans and should find the servo at its new ID.

The EPROM unlock/lock pair is mandatory for SMS/STS servos — the ID register is in the locked EEPROM area, so removing either call will silently fail to persist.

## Editing notes

- Only one servo should be on the bus during programming. The scanner stops at the first responder, so multiple servos would all get reprogrammed to the same ID in sequence (and you'd lose the ability to distinguish them).
- The input buffer is 16 bytes (`input[16]`) with a 15-char cap — fine for IDs but don't repurpose it for longer commands without resizing.
