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

// --- NEW: 50Hz Common-Mode Noise LUT ---
// At a 500Hz sampling rate, a 50Hz sine wave takes 10 samples per cycle.
// Max amplitude is ~109, while the scaled ECG max amplitude is ~12. (Ratio ~ 9:1)
const int8_t noise_50Hz[] = {0, 68, 109, 109, 68, 0, -68, -109, -109, -68};
volatile uint8_t noise_index = 0;

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
  // Grab the current common-mode noise and advance the 50Hz index
  int16_t current_noise = noise_50Hz[noise_index];
  if (++noise_index >= 10) noise_index = 0;

  // --- SIGNAL A (NORMAL, PIN 3) ---
  int16_t ecg_A_val = 0;
  if (++tick_A >= 2) { if (index_A < wave_length) index_A++; tick_A = 0; }
  
  if (index_A < wave_length) {
    // Extract ECG from baseline 128, scale down by 10 to leave room for noise
    ecg_A_val = (pqrst_wave[index_A] - 128) / 10; 
  } else { 
    if (++rest_A >= 320) { index_A = 0; rest_A = 0; } 
  }

  // Combine baseline + scaled ECG + common noise
  int16_t out_A = 128 + ecg_A_val + current_noise;
  if (out_A > 255) out_A = 255; else if (out_A < 0) out_A = 0; // Prevent overflow
  OCR2B = (uint8_t)out_A;

  // --- SIGNAL B (INVERTED, PIN 11) ---
  int16_t ecg_B_val = 0;
  if (++tick_B >= 2) { if (index_B < wave_length) index_B++; tick_B = 0; }
  
  if (index_B < wave_length) {
    // Invert the ECG around baseline 128, then scale down by 10  
    ecg_B_val = (128 - pqrst_wave[index_B]) / 10; 
  } else { 
    if (++rest_B >= 320) { index_B = 0; rest_B = 0; } 
  }

  // Combine baseline + scaled INVERTED ECG + common noise
  int16_t out_B = 128 + ecg_B_val + current_noise;
  if (out_B > 255) out_B = 255; else if (out_B < 0) out_B = 0; // Prevent overflow
  OCR2A = (uint8_t)out_B;
}

void setup() {
  Serial.begin(115200); 
  setup_timers();
}

void loop() {
  int rawA = analogRead(A0);
  int rawB = analogRead(A1);
  int rawC = analogRead(A2);

  float voltsA = (rawA * 5.0) / 1023.0;
  float voltsB = (rawB * 5.0) / 1023.0;
  float voltsC = (rawC * 5.0) / 1023.0;

  // Fixed a minor bug in your original print statements so the IDE Serial Plotter reads all 3 cleanly
  Serial.print("Signal_A_Normal(V):");
  Serial.print(voltsA);
  Serial.print(",");
  Serial.print("Signal_B_Inverted(V):");
  Serial.print(voltsB);
  Serial.print(",");
  Serial.print("Signal_C_Inverted(V):");
  Serial.println(voltsC);

  // Smooth scrolling speed
  delay(25); 
}