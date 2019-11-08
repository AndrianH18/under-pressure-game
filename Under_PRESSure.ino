/* Under PRESSure!
 * 
 * !!!!!!!!!!!!!!!
 * CAUTION: ensure that timing setting on this sketch are matched with the timing setting in the Python GUI script!!!
 * 
 * This is an incredibly simple Arduino-based game with Python GUI running on a computer.  
 * The Arduino board receives players' input and sends the data to over USB (serial) to the computer running
 * the GUI. The computer (or rather, the GUI) simply serves as a "scoreboard".
 * 
 *------------------------------------------------------------------------------------------
 * CURRENT LIMITATION:
 * 1. [GUI] When reading serial input, the GUI freezes until a serial input is received, because
 *    readline() is a blocking function.
 *      - Tried other non-blocking read. Somehow didn't work.
 *      - Can solve using "multithreading" (will do that sometime later...).
 *
 * 2. There's a need to match timing setting in this sketch and the GUI script (i.e.
 *    time_limit, initial_wait, wait).
 *      - Can be solved by sending timing data over serial.
 *      
 * 3. Crude timing (using delay() blocking function instead of timer interrupt) to send data
 *    at desired interval.
 *------------------------------------------------------------------------------------------
 */

// Pinout.
// LED1 and LED2 are for the buttons' built-in LEDs.
// For buttons, must use pins D2 and D3 as they support hardware interrupt.
#define BUT1 2
#define BUT2 3
#define LED1 4
#define LED2 5

// Time settings (in ms).
#define TIME_LIMIT   5000   // Time of each round. 
#define INITIAL_WAIT 4000   // Initial wait before starting game. Buttons irresponsive during this delay.
#define WAIT         6000   // Delay before standby for next round. Buttons irresponsive during this delay.

volatile int press1;
volatile int press2;
unsigned long lastMillis;

void pressed1(void);
void pressed2(void);


void setup()
{
  pinMode(BUT1, INPUT);   // Do NOT use INPUT_PULLUP (button already has RC debounce).
  pinMode(BUT2, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  Serial.begin(115200);
  
  delay(INITIAL_WAIT);
}

void loop()
{
  attachInterrupt(digitalPinToInterrupt(BUT1), pressed1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUT2), pressed2, FALLING);

  press1 = 0;
  press2 = 0;

  // Wait for button press.
  while (press1 == 0 && press2 == 0);

  // Start the round.
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);

  lastMillis = millis();

  // Send number of button presses during the interval.
  // press1 and press2 updated via hardware interrupt.
  while (millis() - lastMillis <= TIME_LIMIT)
  {
    Serial.println(String(press1) + " " + String(press2));
    
    // Reset.
    press1 = 0;
    press2 = 0;
    delay(99);
  }

  // Round ends.
  
  detachInterrupt(digitalPinToInterrupt(BUT1));
  detachInterrupt(digitalPinToInterrupt(BUT2));

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  // Wait for WAIT milliseconds.
  delay(WAIT);
}

// ISR.
void pressed1()
{
  press1++;
}

void pressed2()
{
  press2++;
}
