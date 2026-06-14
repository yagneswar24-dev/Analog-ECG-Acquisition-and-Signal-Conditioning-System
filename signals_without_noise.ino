#include <avr/io.h>
#include <avr/interrupt.h>

// Full-Resolution P-QRS-T complex (0 to 255)
const uint8_t pqrst_wave[] = {
  128, 128, 128, 128, 128,               // Baseline
  130, 135, 140, 145, 140, 135, 130,     // P-Wave 
  128, 128, 128, 128, 128,               // PR Segment
  120, 105, 95,                          // Q-Wave 
  140, 200, 255, 200, 120,               // R-Wave 
  75, 85, 100,                           // S-Wave 
  128, 128, 128, 128, 128,               // ST Segment
  130, 135, 142, 150, 158, 162,          // T-Wave 
  158, 150, 142, 135, 130, 
  128, 128, 128, 128, 128                // Return to Baseline
};
const uint16_t wave_length = sizeof(pqrst_wave);

// Tracking variables
volatile uint16_t index_A = 0, rest_A = 0;
volatile uint8_t  tick_A = 0;
volatile uint16_t index_B = 0, rest_B = 0;
volatile uint8_t  tick_B = 0;

void setup_timers() {
  DDRD |= (1 << DDD3); // Channel A output (Pin 3)
  DDRB |= (1 << DDB3); // Channel B output (Pin 11)
  
  TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << CS20); 
  
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
  OCR1A = 499; 
  TIMSK1 = (1 << OCIE1A); 
  
  sei(); 
}

// ISR triggered 500 times a second
ISR(TIMER1_COMPA_vect) {
  // --- SIGNAL A (NORMAL, PIN 3) ---
  if (++tick_A >= 2) { if (index_A < wave_length) index_A++; tick_A = 0; }
  
  if (index_A < wave_length) {
    OCR2B = pqrst_wave[index_A]; // Standard Output
  } else { 
    OCR2B = 128; // Hold baseline
    if (++rest_A >= 320) { index_A = 0; rest_A = 0; } 
  }

  // --- SIGNAL B (INVERTED, PIN 11) ---
  if (++tick_B >= 2) { if (index_B < wave_length) index_B++; tick_B = 0; }
  
  if (index_B < wave_length) {
    // The Magic Trick: Subtract the value from 255 to flip it completely upside down!
    OCR2A = 255 - pqrst_wave[index_B]; 
  } else { 
    OCR2A = 128; // Hold baseline
    if (++rest_B >= 320) { index_B = 0; rest_B = 0; } 
  }
}

void setup() {
  Serial.begin(115200); 
  setup_timers();
}

void loop() {
  int rawA = analogRead(A0);
  int rawB = analogRead(A1);
  // int rawC = analogRead(A2);

  float voltsA = (rawA * 5.0) / 1023.0;
  float voltsB = (rawB * 5.0) / 1023.0;
  // float voltsC = (rawC * 5.0) / 1023.0;

  Serial.print("Signal_A_Normal(V):");
  Serial.print(voltsA);
  Serial.print(",");
  Serial.print("Signal_B_Inverted(V):");
  Serial.println(voltsB);
  // Serial.print("Signal_C_Inverted(V):");
  // Serial.println(voltsC);

  // Smooth scrolling speed
  delay(25); 
}