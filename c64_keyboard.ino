#include <Keyboard.h>

#define NUM_ROWS (8)
const int row_pins[NUM_ROWS] = {
  14,
  15,
  16,
  17,
  18,
  19,
  2,
  0
};

#define NUM_COLS (8)
const int col_pins[NUM_COLS] = {
  13,
  12,
  11,
  10,
  9,
  7,
  5,
  1
};

static bool debug_enabled = false;
#define DEBUG_PRINT(val) \
  if (debug_enabled) Serial.print(val)
#define DEBUG_PRINTLN(val) \
  if (debug_enabled) Serial.println(val)

// During normal operation we DON'T want a serial port enabled.
// Ideally, it should look just like a keyboard with no other devices.
// Having other stuff active can cause compat issues.
// When debugging, we don't want the keyboard active at that point since it'll
// end up spamming keys into our debug session.
// If you REALLY want key events and serial debug at the same time, define
// KEYBOARD_ENABLED to (true) and comment out the normal define.
// #define KEYBOARD_ENABLED (true)
#define KEYBOARD_ENABLED (!debug_enabled)

// The pin that the restore key is connected to
#define RESTORE_PIN (3)

#define DEBOUNCE_MS (50)
#define PRESSED (LOW)
#define RELEASED (HIGH)

typedef class KeyMapper {
private:
  // Key code to send
  int _code;

  // Key code to send if restore is held
  int _res_code;

  // Time in ms of the last state change for debounce
  unsigned long _action_time;

  // The last state of the key
  int _state;
public:
  void Init(int code, int res_code = -1) {
    _code = code;
    _res_code = res_code;
    _state = HIGH;
    _action_time = millis();
  }

  void Press(int code) {
    if (KEYBOARD_ENABLED) {
      Keyboard.press(code);
    }
  }

  void Release(int code) {
    if (KEYBOARD_ENABLED) {
      Keyboard.release(code);
    }
  }

  void HandleKey(int state) {
    if (state != _state) {
      unsigned long now = millis();
      if ((now - _action_time) > DEBOUNCE_MS) {
        _state = state;
        _action_time = now;
        if (state == PRESSED) {
          DEBUG_PRINT("Key pressed ");
          if (_res_code != -1 && digitalRead(RESTORE_PIN) == PRESSED) {
            DEBUG_PRINTLN(_res_code);
            Press(_code);
          } else {
            DEBUG_PRINTLN(_code);
            Press(_res_code);
          }
        } else if (state == RELEASED) {
          DEBUG_PRINT("Key released ");
          DEBUG_PRINT(_code);
          Release(_code);
          if (_res_code != -1) {
            DEBUG_PRINT(" and ");
            DEBUG_PRINT(_res_code);
            Release(_res_code);
          }
          DEBUG_PRINTLN("");
        } else {
          DEBUG_PRINT("ERROR! Key state is ");
          DEBUG_PRINTLN(state);
        }
      }
    }
  }
} KeyMapper;

KeyMapper key_map[NUM_ROWS][NUM_COLS];

void initKeyMap() {
  key_map[0][0].Init(8, 80);
  key_map[1][0].Init(9);
}

void setup() {
  // Setup and read the restore key.
  // If it is down during boot we enter debug mode.
  pinMode(RESTORE_PIN, INPUT_PULLUP);
  // If you configure a pin as INPUT_PULLUP and then immediatly read it, it
  // will read LOW. I consider this a bug in pinMode as I expect it to be a
  // synchronous function, but whatever. Adding a delay here fixes it.
  delay(100);
  if (digitalRead(RESTORE_PIN) == LOW) {
    Serial.begin(115200);
    debug_enabled = true;
  }

  // Make all row pins digital inputs with pullup
  for (int i = 0; i < NUM_ROWS; i++) {
    pinMode(row_pins[i], INPUT_PULLUP);
  }

  // Make all column pins digital outputs and set them high.
  for (int i = 0; i < NUM_COLS; i++) {
    pinMode(col_pins[i], OUTPUT);
    digitalWrite(col_pins[i], HIGH);
  }

  initKeyMap();
}

void loop() {
  for (int i = 0; i < NUM_COLS; i++) {
    digitalWrite(col_pins[i], LOW);
    for (int j = 0; j < NUM_ROWS; j++) {
      key_map[j][i].HandleKey(digitalRead(row_pins[j]));
    }
    digitalWrite(col_pins[i], HIGH);
  }
}
