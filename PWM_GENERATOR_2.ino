/*
 * Program: cca 50Hz a PWM 
 * Popis: Konfiguruje Timer1 pro generovani 50Hz PWM na pinu 9.
 * A1 potenciometr střídy
 * A0 potenciometr frekvence
 * Pin4 přepínač - pro skokovou změnu PWM (invertuje PWM)
 * Pin12 tlačitko - 10x zpomalí
 * Pin11 tlačítko - 10x zrychlí
 */

// Pokud odkomentujete nasledujici radek, aktivujete ladici rezim (debug mode)
//#define DEBUG_MODE 

const int pinStrida = A1;    // Vstup pro potenciometr střídy
const int pinPerioda = A0;   // Vstup pro potenciometr periody
const int tlacitko = 4;      // Vstup pro přepínač
const int PinPWM = 9;
const int prepinac1 = 12;
const int prepinac2 = 11;
unsigned long maxperioda;
unsigned long minperioda;

// PWM frekvence 50 Hz vyzaduje specifickou konfiguraci Timeru.

void setup() {
  pinMode(tlacitko, INPUT); 
  pinMode(prepinac1, INPUT_PULLUP);
  pinMode(prepinac2, INPUT_PULLUP);

  // --- Konfigurace Timeru 1 pro 50 Hz PWM na Pinu 9 (Output Compare Pin A) ---

  pinMode(PinPWM, OUTPUT); // Ujistime se, ze pin 9 je nastaven jako vystup
 
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
  
   Serial.begin(9600); 
   
}

void loop() {
  bool stavTlacitka=digitalRead(tlacitko);
  // Cteni hodnoty z potenciometru (rozsah 0 - 1023)
  int senzorHodnotaPer = analogRead(pinPerioda);
  maxperioda=6000;
  minperioda=3000;
  if (digitalRead(prepinac1)==LOW){
    // 10x pomalejší
    maxperioda=60000;
    minperioda=30000;
  }
  if (digitalRead(prepinac2)==LOW){
    // 10x rychlejší
    maxperioda=600;
    minperioda=300;
  } 
  
  long periodaHodnota = map(senzorHodnotaPer, 0, 1023, minperioda, maxperioda);
  ICR1 = periodaHodnota;
  // Cteni hodnoty z potenciometru (rozsah 0 - 1023)
  int senzorHodnota = analogRead(pinStrida);
  long pwmHodnota = map(senzorHodnota, 0, 1023, 0, ICR1);
  
  if (stavTlacitka==0){
    OCR1A = pwmHodnota;
  } else {
    OCR1A = periodaHodnota-pwmHodnota;    
  }
  
 
  #ifdef DEBUG_MODE 
  // Optional: tisk hodnot pro ladeni
   Serial.print("ICR1: ");
   Serial.print(ICR1);
   Serial.print("  |  OCR1A (Duty Cycle Register): ");
   Serial.println(OCR1A);
   delay(100);
  #endif
   delay(10); // Kratka prodleva pro stabilizaci cteni
}
