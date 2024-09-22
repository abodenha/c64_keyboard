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

    void HandleKey(int state) {
      if (state != _state) {
        unsigned long now = millis();
        if ((now - _action_time) > DEBOUNCE_MS) {
          _state = state;
          _action_time = now;
          if (state == PRESSED) {
            Serial.print("Key pressed ");
            if (_res_code != -1 && digitalRead(RESTORE_PIN) == PRESSED) {
              Serial.println(_res_code);
            } else {
              Serial.println(_code);
            }
          } else if (state == RELEASED) {
            Serial.print("Key released ");
            Serial.print(_code);
            if (_res_code != -1) {
              Serial.print(" and ");
              Serial.print(_res_code);
            }
            Serial.println("");
          } else {
            Serial.print("ERROR! Key state is ");
            Serial.println(state);
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
  if (digitalRead(RESTORE_PIN) == LOW) {
    Serial.begin(115200);
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
