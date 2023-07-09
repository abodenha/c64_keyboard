# SPDX-FileCopyrightText: 2023 Freaking Rad Kreative Network
#
# SPDX-License-Identifier: MIT

""" Control code for C64 to USB keyboard adapter"""
import time

import board
import digitalio
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

# For debugging. Set this to False to prevent events from actually being sent over USB.
g_output_keypress = True

# Time in ns used for debouncing a key. The code igores anything else happening on a key
# for this long after a key press.
g_debounce_time = 50 * 1000000

# Current time used for debounce logic.
g_now = time.monotonic_ns()

# The keyboard object!
time.sleep(1)  # Sleep for a bit to avoid a race condition on some systems
keyboard = Keyboard(usb_hid.devices)

row_pins = [
    board.A0,
    board.A1,
    board.A2,
    board.A3,
    board.A4,
    board.A5,
    board.D2,
    board.D0,
]
col_pins = [
    board.D13,
    board.D12,
    board.D11,
    board.D10,
    board.D9,
    board.D7,
    board.D5,
    board.D1,
]

row_key_pins = []
col_key_pins = []

# Make all row pins digital inputs with pullup
for pin in row_pins:
    key_pin = digitalio.DigitalInOut(pin)
    key_pin.direction = digitalio.Direction.INPUT
    key_pin.pull = digitalio.Pull.UP
    row_key_pins.append(key_pin)
# Make all column pins digital outputs and set low
for pin in col_pins:
    key_pin = digitalio.DigitalInOut(pin)
    key_pin.direction = digitalio.Direction.OUTPUT
    key_pin.drive_mode = digitalio.DriveMode.PUSH_PULL
    key_pin.value = True
    col_key_pins.append(key_pin)

# Restore key is special since it just connects direct to ground.
# Handle separately but configure as an input with pullup like the others
restore_pin = digitalio.DigitalInOut(board.D3)
restore_pin.direction = digitalio.Direction.INPUT
restore_pin.pull = digitalio.Pull.UP

class KeyData:
    def __init__(self, code, res_code = None):
        self.code = code
        self.down = False
        self.down_time = 0
        self.res_code = res_code

    def handle_key(self, up):
        if (g_now - self.down_time) > g_debounce_time:
            if self.down:
                if up:  # Released
                    if g_output_keypress:
                        keyboard.release(self.code)
                        if self.res_code is not None:
                            keyboard.release(self.res_code)
                    self.down = False
                    print("Releasing key ", self.code, " at ", g_now)
            else:
                if not up:  # Pressed
                    if g_output_keypress:
                        if self.res_code is not None and not restore_pin.value:
                            keyboard.press(self.res_code)
                        else:
                            keyboard.press(self.code)
                    self.down = True
                    self.down_time = g_now
                    print("Pressing key ", self.code, " at ", g_now)

# Note that the keycodes here don't actually correspond to the generated
# characters but rather the code we'd expect from a key in that position.
# The keyboard layout on the OS maps those into the desired key.
# RUN/STOP sends ALT
key_table = [
    [
        KeyData(Keycode.BACKSPACE),                          # 0,0
        KeyData(Keycode.RETURN),                             # 0,1
        KeyData(Keycode.RIGHT_ARROW, Keycode.LEFT_ARROW),    # 0,2
        KeyData(Keycode.F7, Keycode.F8),                     # 0,3
        KeyData(Keycode.F1, Keycode.F2),                     # 0,4
        KeyData(Keycode.F3, Keycode.F4),                     # 0,5
        KeyData(Keycode.F5, Keycode.F6),                     # 0,6
        KeyData(Keycode.DOWN_ARROW, Keycode.UP_ARROW),       # 0,7
    ],
    [
        KeyData(Keycode.THREE),                              # 1,0
        KeyData(Keycode.W, Keycode.UP_ARROW),                # 1,1
        KeyData(Keycode.A, Keycode.LEFT_ARROW),              # 1,2
        KeyData(Keycode.FOUR),                               # 1,3
        KeyData(Keycode.Z),                                  # 1,4
        KeyData(Keycode.S, Keycode.DOWN_ARROW),              # 1,5
        KeyData(Keycode.E),                                  # 1,6
        KeyData(Keycode.LEFT_SHIFT),                         # 1,7
    ],
    [
        KeyData(Keycode.FIVE),                               # 2,0
        KeyData(Keycode.R),                                  # 2,1
        KeyData(Keycode.D, Keycode.RIGHT_ARROW),             # 2,2
        KeyData(Keycode.SIX),                                # 2,3
        KeyData(Keycode.C),                                  # 2,4
        KeyData(Keycode.F),                                  # 2,5
        KeyData(Keycode.T),                                  # 2,6
        KeyData(Keycode.X),                                  # 2,7
    ],
    [
        KeyData(Keycode.SEVEN),                              # 3,0
        KeyData(Keycode.Y),                                  # 3,1
        KeyData(Keycode.G),                                  # 3,2
        KeyData(Keycode.EIGHT),                              # 3,3
        KeyData(Keycode.B),                                  # 3,4
        KeyData(Keycode.H),                                  # 3,5
        KeyData(Keycode.U),                                  # 3,6
        KeyData(Keycode.V),                                  # 3,7
    ],
    [
        KeyData(Keycode.NINE),                               # 4,0
        KeyData(Keycode.I),                                  # 4,1
        KeyData(Keycode.J),                                  # 4,2
        KeyData(Keycode.ZERO),                               # 4,3
        KeyData(Keycode.M),                                  # 4,4
        KeyData(Keycode.K),                                  # 4,5
        KeyData(Keycode.O),                                  # 4,6
        KeyData(Keycode.N),                                  # 4,7
    ],
    [
        KeyData(Keycode.MINUS),                              # 5,0 (The + key)
        KeyData(Keycode.P),                                  # 5,1
        KeyData(Keycode.L),                                  # 5,2
        KeyData(Keycode.EQUALS),                             # 5,3 (The - key)
        KeyData(Keycode.PERIOD),                             # 5,4
        KeyData(Keycode.SEMICOLON),                          # 5,5
        KeyData(Keycode.LEFT_BRACKET),                       # 5,6
        KeyData(Keycode.COMMA),                              # 5,7
    ],
    [
        KeyData(Keycode.RIGHT_BRACKET),                      # 6,0 (POUND key)
        KeyData(Keycode.KEYPAD_ASTERISK),                    # 6,1
        KeyData(Keycode.QUOTE),                              # 6,2 (; key)
        KeyData(Keycode.HOME, Keycode. END),                 # 6,3
        KeyData(Keycode.RIGHT_SHIFT),                        # 6,4
        KeyData(Keycode.KEYPAD_PERIOD),                      # 6,5
        KeyData(Keycode.BACKSLASH),                          # 6,6 (up arrow key)
        KeyData(Keycode.FORWARD_SLASH),                      # 6,7
    ],
    [
        KeyData(Keycode.ONE),                                # 7,0
        KeyData(Keycode.ESCAPE),                             # 7,1
        KeyData(Keycode.CONTROL),                            # 7,2
        KeyData(Keycode.TWO),                                # 7,3
        KeyData(Keycode.SPACE),                              # 7,4
        KeyData(Keycode.WINDOWS),                            # 7,5
        KeyData(Keycode.Q),                                  # 7,6
        KeyData(Keycode.ALT),                                # 7,7
    ],
]

down = False

while True:
    # Grab the time for this iteration.
    # We just call once per loop as we don't need sub ms accuracy
    # Only calling the _ns version of the function to avoid
    # the gradual loss of precision the ms version has.
    g_now = time.monotonic_ns()
    # Scan each column.
    for col in col_key_pins:
        x = col_key_pins.index(col)
        col.value = False  # Pull it low pressing a key will pull the row low.
        # Then each row within each column
        for row in row_key_pins:
            y = row_key_pins.index(row)
            key_table[x][y].handle_key(row.value)
        col.value = True  # Pull it back high to prep for the next step in the scan.

