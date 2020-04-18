#include <Arduino.h>

#define R1_PIN A2
#define R2_PIN A3
#define VIBRO_PIN 2
#define SW1_PIN 10 /* 12v Transistor — LED-stripe highlight */
#define SW2_PIN 5 /* 220v Relay — Middle light */

const byte MIN_R_VAL = 300u;
const byte MAX_R_VAL = 600u;
const byte LOW_BORDER = 2u;
const byte HIGH_BORDER = 5u;

byte left = 0, right = 0;

enum STATE {
  S_NULL,
  S_UNKNOWN,
  S_IDLE,
  S_TRY,
  S_SIG,
};

STATE state = S_UNKNOWN, prev_state = S_NULL;
bool is_left = NULL;
char input_mode;
// char vibro_input_buffer[255] = "";

void print_state () {
  switch (state) {
    case S_UNKNOWN: Serial.println("unknown"); break;
    case S_IDLE: Serial.println("idle"); break;
    case S_TRY: Serial.print("try "); Serial.println(is_left ? 'L' : 'R'); break;
    case S_SIG: Serial.print("sig "); Serial.println(is_left ? 'L' : 'R'); break;
  }
}

template<class... Args>
void vibrate (Args... args) {
  int durations[8] = {args...};
  size_t count = sizeof...(Args);

  for (size_t i = 0; i < count; ++i) {
    digitalWrite(VIBRO_PIN, HIGH);
    delay(durations[i]);
    digitalWrite(VIBRO_PIN, LOW);
    delay(durations[i] == 33 ? 11 : 33);
  }
}

void setup() {
  Serial.begin (19200); //  9600, 115200
  pinMode(A1, OUTPUT), digitalWrite(A1, HIGH); // use as VCC
  pinMode(9, OUTPUT), digitalWrite(9, HIGH); // use as VCC

  pinMode(R1_PIN, INPUT);
  pinMode(R2_PIN, INPUT);
  pinMode(VIBRO_PIN, OUTPUT);
  pinMode(SW1_PIN, OUTPUT);
  pinMode(SW2_PIN, OUTPUT);
}

/**
 * @returns 0 — 12
 */
byte normalize_r_val(int val) {
  // Serial.print("(");
  // Serial.print(val);
  // Serial.print(") ");
  if (val < MIN_R_VAL) return 0;

  return (val - MIN_R_VAL) / (MAX_R_VAL - MIN_R_VAL);
}

void loop() {
  left = normalize_r_val(analogRead(R1_PIN));
  right = normalize_r_val(analogRead(R2_PIN));

  // Serial.print(left); // ; max 1024
  // Serial.print(" / ");
  // Serial.print(right);
  // Serial.println();

  switch (state)
  {
  case S_UNKNOWN:
    if (left <= LOW_BORDER && right <= LOW_BORDER) state = S_IDLE;

    break;


  case S_IDLE:
    if (left >= HIGH_BORDER && right <= LOW_BORDER) state = S_TRY, is_left = true;
    else if (right >= HIGH_BORDER && left <= LOW_BORDER) state = S_TRY, is_left = false;
    
    break;


  case S_TRY:
    // something went wrong (moment missed?)
    if (left < 1 && right < 1) {
      state = S_IDLE;
      Serial.println("// something went wrong (moment missed?)");
      break;
    }

    // wait for inversion of raw signals
    if (is_left) {
      if (left <= LOW_BORDER && right >= LOW_BORDER) state = S_SIG;
    } else {
      if (right <= LOW_BORDER && left >= LOW_BORDER) state = S_SIG;
    }

    break;

  case S_SIG:
    Serial.println("YES!");
    Serial.print("turn "); Serial.println(is_left ? 'L' : 'R');

    print_state();

    state = S_IDLE;

    delay(33);
    digitalWrite(VIBRO_PIN, HIGH);
    delay(33);
    digitalWrite(VIBRO_PIN, LOW);
    delay(33);

    break;
  
  default:
    break;
  }

  if (state != prev_state) {
    Serial.print(left);
    Serial.print(" / ");
    Serial.print(right);
    Serial.print(" // ");
    print_state();
  }

  prev_state = state;

  delay(5);

  if (Serial.available()>0) {
    char input = Serial.read();
    Serial.println(input);
    if (input == '1' || input == '2' || input == 'v') {
      input_mode = input;
    }
    else if ((input_mode == '1' || input_mode == '2') && (input == 'H' || input == 'L')) {
      int PIN = input_mode == '1' ? SW1_PIN : SW2_PIN;
      digitalWrite(PIN, input == 'H' ? HIGH : LOW);
      input_mode = 0;
    }
    else if (input_mode == 'v') {
      input_mode = 0;
      if (input == 'S') vibrate(66);
      else if (input == 'L') vibrate(133);
      else if (input == 'B') vibrate(133, 33);
      else if (input == 'E') vibrate(33, 33, 33);
      // size_t len = strlen(vibro_input_buffer);
      // if (input == 'z' || len == 255) {
      //   handle_vibro_command(vibro_input_buffer);
      //   strcpy(vibro_input_buffer, "");
      // } else {
      //   vibro_input_buffer[len] = input;
      //   vibro_input_buffer[len + 1] = '\0';
      // }
    }
    Serial.println(input);
  }
}
