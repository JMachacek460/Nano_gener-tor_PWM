/*
 * Program: cca 50Hz a PWM 
 * Popis: Konfiguruje Timer1 pro generovani 50Hz PWM na pinu 9.
 * A1 potenciometr střídy
 * A0 potenciometr frekvence
 * Pin4 tlačitko - pro skokovou změnu PWM (invertuje PWM)
 */

// Pokud odkomentujete nasledujici radek, aktivujete ladici rezim (debug mode)
#define DEBUG_MODE 

const int potPin = A1;      // Vstup pro potenciometr střídy
const int potPinPer = A0;      // Vstup pro potenciometr periody
const int tlacitko = 4;      // Vstup pro přepínač

// PWM frekvence 50 Hz vyzaduje specifickou konfiguraci Timeru.

void setup() {
  pinMode(tlacitko, INPUT); // Nastavíme pin jako vstup

  // --- Konfigurace Timeru 1 pro 50 Hz PWM na Pinu 9 (Output Compare Pin A) ---

  pinMode(9, OUTPUT); // Ujistime se, ze pin 9 je nastaven jako vystup
 
  // 1. Nastaveni rezimu: Fast PWM, TOP hodnota je ulozena v ICR1
  // WGM13:10 = 1110 (Fast PWM mode 14)
  TCCR1A = (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12);

  // 2. Nastaveni frekvence (TOP value = ICR1)
  // Pro 16MHz krystal a prescaler 64:
  // ICR1 = (16,000,000 / (64 * 50 Hz)) - 1 = 5000 - 1 = 4999.
  // Pouzijeme hodnotu blizsi 5000 pro presne 50Hz
  ICR1 = 4999; 

  // 3. Nastaveni rezimu non-inverting compare output na OC1A (Pin 9)
  // COM1A1 = 1 (Clear OC1A on Compare Match, set at BOTTOM)
  TCCR1A |= (1 << COM1A1);

  // 4. Nastaveni Predelice (Prescaler) na 64 a spusteni casovace
  // CS11 = 1 a CS10 = 1
  TCCR1B |= (1 << CS11) | (1 << CS10);
  
   Serial.begin(9600); // Odkomentujte pro ladeni
   
}

void loop() {
  bool stavTlacitka=digitalRead(tlacitko);
  // Cteni hodnoty z potenciometru (rozsah 0 - 1023)
  int senzorHodnotaPer = analogRead(potPinPer);
  long Hodnota = map(senzorHodnotaPer, 0, 1023, 3000, 6000);

  // Cteni hodnoty z potenciometru (rozsah 0 - 1023)
  int senzorHodnota = analogRead(potPin);
  long pwmHodnota = map(senzorHodnota, 0, 1023, 0, ICR1);
  
  if (stavTlacitka==0){
    ICR1 = Hodnota;
    OCR1A = pwmHodnota;
  } else {
    ICR1 = Hodnota;
    OCR1A = Hodnota-pwmHodnota;    
  }
  
 
  #ifdef DEBUG_MODE 
  // Optional: tisk hodnot pro ladeni
   Serial.print("ICR1: ");
   Serial.print(ICR1);
   Serial.print("  |  OCR1A (Duty Cycle Register): ");
   Serial.println(OCR1A);
  #endif
   delay(50); // Kratka prodleva pro stabilizaci cteni
}
