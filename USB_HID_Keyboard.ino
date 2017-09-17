#include <Keyboard.h>
/* 
* -------------------------------------------------------------------------
* Interface Atari ST Keyboard to USB HID Keyboard
* -------------------------------------------------------------------------
* Initial idea and some original code provided by user 'joska' of 
* http://www.atari-forum.com - license unknown
* -------------------------------------------------------------------------
* Copyright Kevin Peat 2017
* kevin@kevinpeat.com
* My changes and additions are licensed public domain
* -------------------------------------------------------------------------
* Developed for use with an Arduino Leonardo as it is able to act directly 
* as a USB keyboard controller so doesn't require the Arduino firmware to 
* be modified as some of the other Arduinos (eg. Uno) would do
* -------------------------------------------------------------------------
*/
//#define DEBUG

// ST keyboard reset pin
const int ST_KB_RESET = 4;

// Atari modifier key codes
const uint8_t ST_LEFT_CTRL = 0x1D;
const uint8_t ST_LEFT_SHIFT = 0x2A;
const uint8_t ST_LEFT_ALT = 0x38;
const uint8_t ST_RIGHT_SHIFT = 0x36;
const uint8_t ST_CAPS_LOCK = 0x3A;

// Arduino Leonardo modifier key codes
const uint8_t ARD_LEFT_CTRL = 0x80;
const uint8_t ARD_LEFT_SHIFT = 0x81;
const uint8_t ARD_LEFT_ALT = 0x82;
const uint8_t ARD_RIGHT_SHIFT = 0x85;
const uint8_t ARD_CAPS_LOCK = 0xC1;

// Arduino Leonardo special key codes
const uint8_t ARD_UP_ARROW = 0xDA;
const uint8_t ARD_DOWN_ARROW = 0xD9;
const uint8_t ARD_LEFT_ARROW = 0xD8;
const uint8_t ARD_RIGHT_ARROW = 0xD7;
const uint8_t ARD_BACKSPACE = 0xB2;
const uint8_t ARD_TAB = 0xB3;
const uint8_t ARD_RETURN = 0xB0;
const uint8_t ARD_ESC = 0xB1;
const uint8_t ARD_INSERT = 0xD1;
const uint8_t ARD_DELETE = 0xD4;
const uint8_t ARD_HOME = 0xD2;
const uint8_t ARD_F1 = 0xC2;
const uint8_t ARD_F2 = 0xC3;
const uint8_t ARD_F3 = 0xC4;
const uint8_t ARD_F4 = 0xC5;
const uint8_t ARD_F5 = 0xC6;
const uint8_t ARD_F6 = 0xC7;
const uint8_t ARD_F7 = 0xC8;
const uint8_t ARD_F8 = 0xC9;
const uint8_t ARD_F9 = 0xCA;
const uint8_t ARD_F10 = 0xCB;
const uint8_t ARD_F11 = 0xCC;
const uint8_t ARD_F12 = 0xCD;

// Keyboard auto-repeat
static uint8_t last_make;    // Last make char
static unsigned long last_make_time;  // Last make time (milliseconds)
int auto_repeat_delay = 500; // Keyboard auto-repeat delay (milliseconds)
int auto_repeat_rate = 25;   // Keyboard auto-repeat rate (milliseconds)

// Key scancodes
// Use ST scancode as index to find the corresponding USB scancode.
// Make-codes are single byte, with some exceptions.
// These are escaped with a 0xe0 byte, when this appear in this table a
// simple switch is used to look up the correct scancode.
// All break codes are the same as the scancodes, but are prefixed with 0xf0.
// The escaped keys are first escaped, then 0xf0, then scancode.
uint8_t scanCodes[] =
{
  0x00, // (Nothing)
  ARD_ESC, // Esc
  0x31, // 1
  0x32, // 2
  0x33, // 3
  0x34, // 4
  0x35, // 5
  0x36, // 6
  0x37, // 7
  0x38, // 8
  0x39, // 9
  0x30, // 0
  0x2D, // -
  0x3D, // == (Mapped to =)
  ARD_BACKSPACE, // Backspace
  ARD_TAB, // Tab
  0x71, // q
  0x77, // w
  0x65, // e
  0x72, // r
  0x74, // t
  0x79, // y
  0x75, // u
  0x69, // i
  0x6F, // o
  0x70, // p
  0x5B, // [
  0x5D, // ]
  ARD_RETURN, // Enter
  ARD_LEFT_CTRL, // Control
  0x61, // a
  0x73, // s
  0x64, // d
  0x66, // f
  0x67, // g
  0x68, // h
  0x6A, // j
  0x6B, // k
  0x6C, // l
  0x3B, // ;
  0x27, // ' (Mapped to '")
  0xE0, // #
  ARD_LEFT_SHIFT, // Lshift
  0x7E, // #~ (Mapped to ~)
  0x7A, // z
  0x78, // x
  0x63, // c
  0x76, // v
  0x62, // b
  0x6E, // n
  0x6D, // m
  0x2C, // ,
  0x2E, // .
  0x2F, // /
  ARD_RIGHT_SHIFT, // Rshift
  0x37, // (Not used)
  ARD_LEFT_ALT, // Alternate
  0x20, // Space
  ARD_CAPS_LOCK, // CapsLock
  ARD_F1, // F1
  ARD_F2, // F2
  ARD_F3, // F3
  ARD_F4, // F4
  ARD_F5, // F5
  ARD_F6, // F6
  ARD_F7, // F7
  ARD_F8, // F8
  ARD_F9, // F9
  ARD_F10, // F10
  0x45, // (Not used)
  0x46, // (Not used)
  0xE0, // Clr/Home
  0xE0, // Up Arrow
  0x49, // (Not used)
  0x2D, // N-
  0xE0, // Left Arrow
  0x4c, // (Not used)
  0xE0, // Right Arrow
  0x2B, // N+
  0x4f, // (Not used)
  0xE0, // Down Arrow
  0x51, // (Not used)
  0xE0, // Insert
  0xE0, // Delete
  0x54, // (Not used)
  0x55, // (Not used)
  0x56, // (Not used)
  0x57, // (Not used)
  0x58, // (Not used)
  0x59, // (Not used)
  0x5a, // (Not used)
  0x5b, // (Not used)
  0x5c, // (Not used)
  0x5d, // (Not used)
  0x5e, // (Not used)
  0x5f, // (Not used)
  0x5C, // ISO Key
  0xB2, // Undo (Mapped to Backspace)
  ARD_F12, // Help (Mapped to F12 which is the Hatari menu key)
  0x28, // N(
  0x29, // N)
  0xE0, // N/
  0x2A, // N*
  0x37, // N7
  0x38, // N8
  0x39, // N9
  0x34, // N4
  0x35, // N5
  0x36, // N6
  0x31, // N1
  0x32, // N2
  0x33, // N3
  0x30, // N0
  0x2E, // N.
  0xE0  // NEnter
};

void setup(void)
{
  // Initialize keyboard:
  Keyboard.begin();
  
  // Open serial port from Atari keyboard
  Serial1.begin(7812);

#ifdef DEBUG
  // Open serial port to PC
  Serial.begin(9600);
#endif
  
  // Reset ST keyboard
  delay(200);
  reset_st_keyboard();
  delay(200);

  // Empty serial buffer before starting
  while(Serial1.available() > 0) Serial1.read();
}

void loop()
{
  // Process incoming Atari keypresses
  if (Serial1.available() > 0) process_keypress(Serial1.read());

  // Handle keyboard auto-repeat
  auto_repeat();
}

// Reset ST Keyboard
void reset_st_keyboard(void)
{
  Serial1.print(0x80);
  Serial1.print(1);
  pinMode(ST_KB_RESET, OUTPUT);
  digitalWrite(ST_KB_RESET, HIGH);
  delay(20);
  digitalWrite(ST_KB_RESET, LOW);
  delay(20);
  digitalWrite(ST_KB_RESET, HIGH);
}

// Process each keypress
void process_keypress(uint8_t key)
{
  // Keypress
  if (((key & 0x7f) > 0) && ((key & 0x7f) < 0x73))
  {
    // Break codes (other than modifiers) do not need to be sent 
    // to the PC as the Leonardo keyboard interface handles that
    if (key & 0x80) // Break
    {
      last_make = 0;
      last_make_time = 0;
      process_modifier(key);
    }
    else // Make
    {
      last_make = key;
      last_make_time = millis();
      convert_scancode(key);
    }
  }
}

// Convert from ST scancode to PC scancode
void convert_scancode(uint8_t key)
{
  
  uint8_t break_code = key & 0x80;
  uint8_t pc_code = scanCodes[key & 0x7f];
  uint8_t escaped = (pc_code == 0xe0 ? true:false);
  
#ifdef DEBUG
    Serial.print("Atari scancode: ");
    Serial.println(key, DEC);
    Serial.print("PC scancode: ");
    Serial.println(pc_code, DEC);
    Serial.print("Break code: ");
    Serial.println(break_code, DEC);
    Serial.print("Escaped: ");
    Serial.println(escaped, DEC);
#endif
  
  // Handle modifier key presses
  if (process_modifier(key)) return;

  // Special handling required for escaped keypresses
  if (escaped)
  {
    switch (key & 0x7f)
    {
      case 0x48: // Up arrow
        send_escaped_key(ARD_UP_ARROW);
        break;
      case 0x4b: // Left arrow
        send_escaped_key(ARD_LEFT_ARROW);
        break;
      case 0x4d: // Right arrow
        send_escaped_key(ARD_RIGHT_ARROW);
        break;
      case 0x50: // Down arrow
        send_escaped_key(ARD_DOWN_ARROW);
        break;
      case 0x52: // Insert
        send_escaped_key(ARD_INSERT);
        break;
      case 0x53: // Delete
        send_escaped_key(ARD_DELETE);
        break;
      case 0x47: // Clr/Home
        send_escaped_key(ARD_HOME);
        break;
      case 0x65: // Num /
        send_escaped_key(0x2F);
        break;
      case 0x72: // Num Enter
        send_escaped_key(ARD_RETURN);
        break;
      case 0x2b: // Tilde
        send_escaped_key(0x23);
        break;
      case 0x62: // Help
        send_escaped_key(ARD_F1);
        break;
    }
  }
  else
  {
    Keyboard.write(pc_code);
  }
}

// Send code for escaped Atari keypresses
void send_escaped_key(uint8_t key)
{
  Keyboard.press(key);
  delay(20);
  Keyboard.release(key); 
}

// Process modifier keypresses
boolean process_modifier(uint8_t key)
{
  // Modifier key press  
  switch (key)
    {
      case ST_LEFT_CTRL:
        Keyboard.press(ARD_LEFT_CTRL);
        return true;
      case ST_LEFT_SHIFT:
        Keyboard.press(ARD_LEFT_SHIFT);
        return true;
      case ST_LEFT_ALT:
        Keyboard.press(ARD_LEFT_ALT);
        return true;
      case ST_RIGHT_SHIFT:
        Keyboard.press(ARD_RIGHT_SHIFT);
        return true;        
      case ST_CAPS_LOCK:
        Keyboard.press(ARD_CAPS_LOCK);
        return true;
    }

  // Modifier key release
  switch (key & 0x7f)
    {
      case ST_LEFT_CTRL:
        Keyboard.release(ARD_LEFT_CTRL);
        return true;
      case ST_LEFT_SHIFT:
        Keyboard.release(ARD_LEFT_SHIFT);
        return true;
      case ST_LEFT_ALT:
        Keyboard.release(ARD_LEFT_ALT);
        return true;
      case ST_RIGHT_SHIFT:
        Keyboard.release(ARD_RIGHT_SHIFT);
        return true;        
      case ST_CAPS_LOCK:
        Keyboard.release(ARD_CAPS_LOCK);
        return true;
    }
  
  return false;  
}

// Keyboard auto repeat
void auto_repeat(void)
{
  static unsigned long last_repeat;
  static byte key_repeating;  // True if key being repeated
  
  // Don't want to repeat modifiers  
  switch (last_make)
  {
    case ST_LEFT_CTRL:
    case ST_LEFT_SHIFT:
    case ST_RIGHT_SHIFT:
    case ST_LEFT_ALT:
    case ST_CAPS_LOCK:
    case 0x00: // No key held down
      key_repeating = false;
      return;
  }

  // Delay to first repeat  
  if (!key_repeating && (millis() - last_make_time > auto_repeat_delay))
  {
    key_repeating = true;
    last_repeat = millis();
    return;
  }

  // Start repeating
  if (key_repeating && (millis() - last_repeat > auto_repeat_rate))
  {
    last_repeat = millis();
    convert_scancode(last_make);
  }  
}

