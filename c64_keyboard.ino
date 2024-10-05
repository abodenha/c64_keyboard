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
            Press(_res_code);
          } else {
            DEBUG_PRINTLN(_code);
            Press(_code);
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
  // TODO: Replace the class with C-style data structure. Init will become
  // InitKey and should access key_map on its own (and do range checking).
  // If you accidentally typo something out of range on the array index, the
  // compiler will quite happily do an out of bounds write and stomp all
  // over things we really don't want stomped on. That can make recovery
  // a pain in the butt.
  // TODO the :/[ and ;/] keys aren't doing what I want. Likely will need
  // to handle them in the key layout on the OS side, but need to figure
  // out what keycap to send.
  key_map[0][0].Init(KEY_BACKSPACE);
  key_map[1][0].Init(KEY_RETURN);
  key_map[2][0].Init(KEY_RIGHT_ARROW, KEY_LEFT_ARROW);
  key_map[3][0].Init(KEY_F7, KEY_F8);
  key_map[4][0].Init(KEY_F1, KEY_F2);
  key_map[5][0].Init(KEY_F3, KEY_F4);
  key_map[6][0].Init(KEY_F5, KEY_F6);
  key_map[7][0].Init(KEY_DOWN_ARROW, KEY_UP_ARROW);
  key_map[0][1].Init('3');
  key_map[1][1].Init('w', KEY_UP_ARROW);
  key_map[2][1].Init('a', KEY_LEFT_ARROW);
  key_map[3][1].Init('4');
  key_map[4][1].Init('z');
  key_map[5][1].Init('s', KEY_DOWN_ARROW);
  key_map[6][1].Init('e');
  key_map[7][1].Init(KEY_LEFT_SHIFT);
  key_map[0][2].Init('5');
  key_map[1][2].Init('r');
  key_map[2][2].Init('d', KEY_RIGHT_ARROW);
  key_map[3][2].Init('6');
  key_map[4][2].Init('c');
  key_map[5][2].Init('f');
  key_map[6][2].Init('t');
  key_map[7][2].Init('x');
  key_map[0][3].Init('7');
  key_map[1][3].Init('y');
  key_map[2][3].Init('g');
  key_map[3][3].Init('8');
  key_map[4][3].Init('b');
  key_map[5][3].Init('h');
  key_map[6][3].Init('u');
  key_map[7][3].Init('v');
  key_map[0][4].Init('9');
  key_map[1][4].Init('i');
  key_map[2][4].Init('j');
  key_map[3][4].Init('0');
  key_map[4][4].Init('m');
  key_map[5][4].Init('k');
  key_map[6][4].Init('o');
  key_map[7][4].Init('n');
  key_map[0][5].Init(KEY_KP_PLUS);
  key_map[1][5].Init('p');
  key_map[2][5].Init('l');
  key_map[3][5].Init(KEY_KP_MINUS);
  key_map[4][5].Init('.');
  key_map[5][5].Init(';');
  key_map[6][5].Init('['); // This is the @ key. Need to decide what to send here.
  key_map[7][5].Init(',');
  key_map[0][6].Init(']'); // This is the pound key. Need to decide what to send here.
  key_map[1][6].Init(KEY_KP_ASTERISK);
  key_map[2][6].Init('\''); // The ;/] key
  key_map[3][6].Init(KEY_HOME, KEY_END);
  key_map[4][6].Init(KEY_RIGHT_SHIFT);
  key_map[5][6].Init('=');
  key_map[6][6].Init('\\'); // This is the upward pointing arrow key.
  key_map[7][6].Init('/');
  key_map[0][7].Init('1');
  key_map[1][7].Init(KEY_ESC); // Backward pointing arrow key in the upper left
  key_map[2][7].Init(KEY_LEFT_CTRL);
  key_map[3][7].Init('2');
  key_map[4][7].Init(' ');
  key_map[5][7].Init(KEY_LEFT_GUI); // The Commodore key.
  key_map[6][7].Init('q');
  key_map[7][7].Init(KEY_LEFT_ALT);
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
  } else {
    debug_enabled = false;
    Keyboard.begin(KeyboardLayout_en_US);
    Serial.end(); // Just to be sure. Bootloader might have initted it.
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
