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

// Input pins
int LEFT = PB7;
int RIGHT = PB6;
int ST_BY = PB5;
int STOP = PB4;

// Output pins
int LED_left = A1;
int LED_right = A2;

// Config variables
int st_by_power = 250;
int fast_blink = 50;
int slow_blink = 100; // slow_blink > fast_blink
uint32_t turn_delay = 2000;

// Service variables
int low_light = 255;
int result = 0x0000;
uint32_t time_flag = 0;

// Prototypes
int measure();
int init_check();
int stop();
int strobe();
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
  measure();
  init_check();

  // Read inputs ans make a decission

  if (!digitalRead(ST_BY))
  {
    low_light = st_by_power; // Slight light
  }
  else
  {
    low_light = 255; // Light off
  }

  switch (result)
  {
  case 0: // OFF
    off();
    break;
  case 1: // STOP & RIGHT
    sright();
    break;
  case 2: // STOP & LEFT
    sleft();
    break;
  case 3: // STOP when turning and STROBE by default
    if (millis() > (time_flag + turn_delay))
    {
      strobe(); // Strobe signal on stop when going straight
    }
    else
    {
      stop(); // Just the stop signal in pauses of turn signal blinking
    }
    break;
  case 4: // OFF
    off();
    break;
  case 5: // RIGHT
    right();
    break;
  case 6: // LEFT
    left();
    break;
  case 7: // OFF
    off();
    break;
  default: // NO input or error occurred
    off();
    break;
  }
}

int measure()
{
  result ^= (-(digitalRead(LEFT)) ^ result) & (1UL << 0);
  result ^= (-(digitalRead(RIGHT)) ^ result) & (1UL << 1);
  result ^= (-(digitalRead(STOP)) ^ result) & (1UL << 2);
  // result ^= (-R ^ result) & (1UL << 3);
  return 0;
}

int init_check()
{
  for (int i = 0; i > 255; i++)
  {
    analogWrite(LED_left, i);
    analogWrite(LED_left, 255 - i);
    delay(300);
  }
  return 0;
}

int stop()
{
  analogWrite(LED_left, 0);
  analogWrite(LED_right, 0);
  return 0;
}

int strobe()
{
  // Fast blink
  analogWrite(LED_left, 0);
  analogWrite(LED_right, 0);
  delay(fast_blink);

  analogWrite(LED_left, 255);
  analogWrite(LED_right, 255);
  delay(fast_blink);

  analogWrite(LED_left, 0);
  analogWrite(LED_right, 0);
  delay(fast_blink);

  analogWrite(LED_left, 255);
  analogWrite(LED_right, 255);
  delay(fast_blink);

  analogWrite(LED_left, 0);
  analogWrite(LED_right, 0);
  delay(fast_blink);

  analogWrite(LED_left, 255);
  analogWrite(LED_right, 255);
  delay(fast_blink);

  // Slow blink if break is not released yet
  measure();
  if (result == 3)
  {
    analogWrite(LED_left, 0);
    analogWrite(LED_right, 0);
    delay(slow_blink);

    analogWrite(LED_left, 255);
    analogWrite(LED_right, 255);
    delay(slow_blink);

    analogWrite(LED_left, 0);
    analogWrite(LED_right, 0);
    delay(slow_blink);

    analogWrite(LED_left, 255);
    analogWrite(LED_right, 255);
    delay(slow_blink);

    analogWrite(LED_left, 0);
    analogWrite(LED_right, 0);
    delay(slow_blink);

    analogWrite(LED_left, 255);
    analogWrite(LED_right, 255);
    delay(slow_blink);
  }
  else
  {
    return 0;
  }

  // Static stop light
  while (result == 3)
  {
    analogWrite(LED_left, 0);
    analogWrite(LED_right, 0);
    delay(100);
    measure();
  }
  return 0;
}

int left()
{
  analogWrite(LED_left, 0);
  analogWrite(LED_right, low_light);
  time_flag = millis();
  return 0;
}

int right()
{
  analogWrite(LED_left, low_light);
  analogWrite(LED_right, 0);
  time_flag = millis();
  return 0;
}

int sleft()
{
  analogWrite(LED_left, 255);
  analogWrite(LED_right, 0);
  time_flag = millis();
  return 0;
}

int sright()
{
  analogWrite(LED_left, 0);
  analogWrite(LED_right, 255);
  time_flag = millis();
  return 0;
}

int off()
{
  analogWrite(LED_left, low_light);
  analogWrite(LED_right, low_light);
  return 0;
}
