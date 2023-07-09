# SPDX-FileCopyrightText: 2023 Freaking Rad Kreative Network
#
# SPDX-License-Identifier: MIT
""" boot.py for C64 to USB keyboard adapter"""
import storage
import usb_midi
import usb_cdc
import board
import digitalio

# Holding restore as the device boots will skip disabling serial and drive.
restore_pin = digitalio.DigitalInOut(board.D3)
restore_pin.direction = digitalio.Direction.INPUT
restore_pin.pull = digitalio.Pull.UP

if restore_pin.value:
    storage.disable_usb_drive()  # Disable the CIRCUITPY drive connection to USB
    usb_midi.disable()  # Disable midi
    usb_cdc.disable()  # Disable both serial devices.
