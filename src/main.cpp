#include <Arduino.h>

/* Truth table for stop and turn signals
*           | LEFT_led      | RIGHT_led
*-----------|---------------|-----------
* ST-BY     | low           | low
* STOP      | high          | high
* LEFT      | blink         | off/low
* RIGHT     | off/low       | blink
* SLEFT     | blink         | high
* SRIGHT    | high          | blink
*/

// Config variables
int st_by_power = 127;
int fast_blink = 100;
int slow_blink = 300; // slow_blink > fast_blink

// Input pins
int LEFT = PA0;
int RIGHT = PA1;
int ST_BY = PA2;
int STOP = PA3;

// Output pins
int LED_left = PA4;
int LED_right = PA5;

// Service variables
int low_light = 0;

// Prototypes
int init_check();
int strob();
int left();
int right();
int sleft();
int sright();
int off();

void setup()
{
  // Init inputs
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(ST_BY, INPUT_PULLUP);
  pinMode(STOP, INPUT_PULLUP);

  // Init LED outputs
  pinMode(LED_left, OUTPUT);
  pinMode(LED_right, OUTPUT);
}

void loop()
{
  init_check();

  // Read inputs ans make a decission

  if (!digitalRead(ST_BY))
  {
    low_light = st_by_power;
  }
  else
  {
    low_light = 0;
  }

  switch ((digitalRead(STOP) * 1) + (digitalRead(LEFT) * 2) + +(digitalRead(RIGHT) * 4))
  {
  case 1:
    strob();
    break;
  case 2:
    left();
    break;
  case 4:
    right();
    break;
  case 3:
    sleft();
    break;
  case 5:
    sright();
    break;
  default:
    off();
    break;
  }
}

int init_check()
{
  for (int i = 0; i > 255; i++)
  {
    analogWrite(LED_left, i);
    analogWrite(LED_left, 256 - i);
    delay(100);
  }
  off();
  return 0;
}

int strob()
{
  // Fast blink
  digitalWrite(LED_left, HIGH);
  digitalWrite(LED_right, HIGH);
  delayMicroseconds(fast_blink);
  digitalWrite(LED_left, LOW);
  digitalWrite(LED_right, LOW);
  delayMicroseconds(fast_blink);

  digitalWrite(LED_left, HIGH);
  digitalWrite(LED_right, HIGH);
  delayMicroseconds(fast_blink);
  digitalWrite(LED_left, LOW);
  digitalWrite(LED_right, LOW);
  delayMicroseconds(fast_blink);

  digitalWrite(LED_left, HIGH);
  digitalWrite(LED_right, HIGH);
  delayMicroseconds(fast_blink);
  digitalWrite(LED_left, LOW);
  digitalWrite(LED_right, LOW);
  delayMicroseconds(fast_blink);

  // Slow blink if break is not released yet
  if (digitalRead(STOP) == 0)
  {
    digitalWrite(LED_left, HIGH);
    digitalWrite(LED_right, HIGH);
    delayMicroseconds(slow_blink);
    digitalWrite(LED_left, LOW);
    digitalWrite(LED_right, LOW);
    delayMicroseconds(slow_blink);

    digitalWrite(LED_left, HIGH);
    digitalWrite(LED_right, HIGH);
    delayMicroseconds(slow_blink);
    digitalWrite(LED_left, LOW);
    digitalWrite(LED_right, LOW);
    delayMicroseconds(slow_blink);

    digitalWrite(LED_left, HIGH);
    digitalWrite(LED_right, HIGH);
    delayMicroseconds(slow_blink);
    digitalWrite(LED_left, LOW);
    digitalWrite(LED_right, LOW);
    delayMicroseconds(slow_blink);
  }
  else
  {
    return 0;
  }

  // Static stop light
  while (!digitalRead(STOP) and digitalRead(LEFT) and digitalRead(RIGHT))
  {
    digitalWrite(LED_left, HIGH);
    digitalWrite(LED_right, HIGH);
  }
  return 0;
}

int left()
{
  digitalWrite(LED_left, HIGH);
  analogWrite(LED_right, low_light);
  return 0;
}

int right()
{
  analogWrite(LED_left, low_light);
  digitalWrite(LED_right, HIGH);
  return 0;
}

int sleft()
{
  digitalWrite(LED_left, LOW);
  digitalWrite(LED_right, HIGH);
  return 0;
}

int sright()
{
  digitalWrite(LED_left, HIGH);
  digitalWrite(LED_right, LOW);
  return 0;
}

int off()
{
  digitalWrite(LED_left, LOW);
  digitalWrite(LED_right, LOW);
  return 0;
}
