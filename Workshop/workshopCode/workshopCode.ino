//Bekötés
#define STB 2  // Strobe digital pin
#define CLK 3  // clock digital pin
#define DIO 4  // data digital pin

#include "kijelzo.h"


uint8_t digitTo7Segment(uint8_t digit) {
  switch (digit) {
    case 0:
      return 0b00111111;
    case 1:
      return 0b00000110;
    case 2:
      return 0b01011011;
    case 3:
      return 0b01001111;
    case 4:
      return 0b01100110;
    case 5:
      return 0b01101101;
    case 6:
      return 0b01111101;
    case 7:
      return 0b00000111;
    case 8:
      return 0b01111111;
    case 9:
      return 0b01101111;
    default:
      return 0;
  }
}

//globális változók
int counter = 0;

void displayCounter() {
  int value = counter;

  uint8_t thousands = counter / 1000;
  value = value - (thousands * 1000);

  uint8_t hundreds = value / 100;
  value = value - (hundreds * 100);

  uint8_t tens = value / 10;
  uint8_t ones = value % 10;

  displayDig(0, digitTo7Segment(ones));
  displayDig(1, digitTo7Segment(tens));
  displayDig(2, digitTo7Segment(hundreds));
  displayDig(3, digitTo7Segment(thousands));
}

void setup() {
  pinMode(STB, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT);
  digitalWrite(STB, HIGH);
  digitalWrite(CLK, HIGH);
  digitalWrite(DIO, HIGH);

  reset();
  displayCounter();
}

void loop() {
  uint8_t buttons = getButtons();
  if (buttons == 1) {
    ++counter;
    displayCounter();
  } 
  else if (buttons == 2) {
    --counter;
    if (counter < 0)
      counter = 0;
    displayCounter();
  }
  delay(250);
}
