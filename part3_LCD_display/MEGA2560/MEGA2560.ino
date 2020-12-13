#include <LiquidCrystal.h>
#include <Wire.h>

// Initalisierung der Bibliothek mit Pins zur Kommunikation
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

char fieldNumber = '0';
byte Feldnummer = 1;
float Temperatur;
float Feuchte;
float Druck;
float Helligkeit;
float Akkuspannung;
float Quality;
float Taupunkt;
float Akkuladung;

#define NEXT_PIN 6
#define PREV_PIN 5

/* Präkompilerdefinition TEST_MODE für Testmodus 
 * entfernen um Testmodusanweisungen nicht zu kompilieren. 
 * Betrifft va. Ausgaben über serielle Schnittstelle 
 */
#define TEST_MODE


void setup() {
  // Setup LCD-Display
  lcd.begin(16, 2);

  // Setup I2C-Bus
  Wire.begin(8);                // Beitreten mit Adresse #8
  Wire.onReceive(receiveEvent); // Interrupt Event registrieren

  #ifdef TEST_MODE
  // Setup serielle Schnittstelle
  Serial.begin(9600);    
  #endif

  // Setup Input-Pins
  pinMode(NEXT_PIN, INPUT_PULLUP);
  pinMode(PREV_PIN, INPUT_PULLUP);
}


void loop() {
  
  if (!digitalRead(NEXT_PIN)) {
    delay(300);
    (Feldnummer < 8) ? Feldnummer++ : Feldnummer = 1;
    
    #ifdef TEST_MODE
    Serial.print("Feldnummer ");
    Serial.println(Feldnummer);
    #endif
  }

  if (!digitalRead(PREV_PIN)) {
    delay(300);
    (Feldnummer > 1) ? Feldnummer-- : Feldnummer = 8;

    #ifdef TEST_MODE
    Serial.print("Feldnummer ");
    Serial.println(Feldnummer);
    #endif
  }

  // LCD-Anzeige in Abh. der aktuellen Feldnummer
  switch (Feldnummer) {

    case 1:
      printLCD("Temperatur", Temperatur, (" \xDF" "C"));
      break;
      
    case 2:
      printLCD("Feuchte", Feuchte, " %");
      break;

    case 3:
      printLCD("Taupunkt", Taupunkt, (" \xDF" "C"));
      break;

    case 4:
      printLCD("Druck", Druck, " mbar");
      break;

    case 5:
      printLCD("Helligkeit", Helligkeit, "");
      break;

    case 6:
      printLCD("Luftqualit\xE1t", Quality, "");
      break;

    case 7:
      printLCD("Akkuladung", Akkuladung, " %");
      break;

    case 8:
      printLCD("Akkuspannung", Akkuspannung, " V");
      break;
  } 
  
 delay(100);
}


void printLCD(String nachricht, float wert, String einheit)
{
  lcd.clear();
      
  lcd.setCursor(0,0);
  lcd.print(nachricht);

  lcd.setCursor(0,1);
  lcd.print(wert);
  lcd.print(einheit);
}


void receiveEvent(int howMany)
{
  String nachricht = "";
  char number ='0';

  // Lesen der vorangestellten ID (Feldnummer)
  number = (char)Wire.read();

  #ifdef TEST_MODE
  Serial.print("number: "); Serial.println(number);
  #endif

  // Nachricht byteweise empfangen
  while(Wire.available())
  {
    nachricht += String((char)Wire.read());
  }
  Serial.println(nachricht);

  // Zuweisen in Abh. der ID (Feldnummer)
  switch(number){
    case '1':
      Temperatur = nachricht.toFloat();

      #ifdef TEST_MODE
      Serial.print("Temp: "); Serial.println(Temperatur);
      #endif
      
      fieldNumber = '0';
      break;
      
    case '2':
      Feuchte = nachricht.toFloat();

      #ifdef TEST_MODE
      Serial.print("Feuchte: "); Serial.println(Feuchte);
      #endif
      
      fieldNumber = '0';
      break;
      
    case '3':
      Druck = nachricht.toFloat();

      #ifdef TEST_MODE
      Serial.print("Druck: "); Serial.println(Druck);
      #endif
      
      fieldNumber = '0';
      break;
      
    case '4':
      Helligkeit = nachricht.toFloat();

      #ifdef TEST_MODE
      Serial.print("Helligkeit: "); Serial.println(Helligkeit);
      #endif
      
      fieldNumber = '0';
      break;
      
    case '5':
      Akkuspannung = nachricht.toFloat();

      #ifdef TEST_MODE
      Serial.print("Akku: "); Serial.println(Akkuspannung,5);
      #endif
      
      fieldNumber = '0';
      break;
      
    case '6':
      Quality = nachricht.toFloat();

      #ifdef TEST_MODE
      Serial.print("Luftqualität: "); Serial.println(Quality,5);
      #endif
      
      fieldNumber = '0';
      break;
      
    case '7':
      Taupunkt = nachricht.toFloat();

      #ifdef TEST_MODE
      Serial.print("Taupunkt: "); Serial.println(Taupunkt,5);
      #endif
      
      fieldNumber = '0';
      break;
      
    case '8':
      Akkuladung = nachricht.toFloat();

      #ifdef TEST_MODE
      Serial.print("Akkuladung: "); Serial.println(Akkuladung,5);
      #endif
      
      fieldNumber = '0';
      break;    
  }
}
