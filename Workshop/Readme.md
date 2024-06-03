# Intro

TM1638 - LED Driver + Keyboard scan interface

* Nem SPI, de soros kommunikációt alkalmaz
* 10 segmens x 8
* 8 x 3 bit key scan

## Életrekeltés

![01](img/01.png)

Utasítások definiálása:

```c
#define INSTRUCTION_WRITE_DATA 0x40
#define INSTRUCTION_ADDRESS_AUTO 0x40
#define DISPLAY_TURN_ON 0x88
#define INSTRUCTION_READ_KEY 0x42
#define INSTRUCTION_ADDRESS_FIXED 0x44
```

Adat küldés:

![02](img/02.png)

```c
void writeData(uint8_t data) {
  shiftOut(DIO, CLK, LSBFIRST, data);
}
```

Inicializáció folyamata:

![03](img/03.png)

Google translate és némi próba alapján:

```c
//Kijelző mód parancs
void setDisplayMode(uint8_t displayMode) {
  digitalWrite(STB, LOW);
  writeData(displayMode);
  digitalWrite(STB, HIGH);
  delayMicroseconds(1);
}

//Utasítás külés
void setDataInstruction(uint8_t dataInstruction) {
  digitalWrite(STB, LOW);
  writeData(dataInstruction);
  digitalWrite(STB, HIGH);
  delayMicroseconds(1);
}

//Inicializálás tényleges kódja
void reset() {
  setDisplayMode(DISPLAY_TURN_ON);  //PULSE1/16-ra konfig
  setDataInstruction(INSTRUCTION_WRITE_DATA | INSTRUCTION_ADDRESS_AUTO);
  digitalWrite(STB, LOW);
  writeData(FIRST_DISPLAY_ADDRESS);
  for (uint8_t i = 0; i < 16; i++) {
    writeData(0);
  }
  digitalWrite(STB, HIGH);
}
```

## Gomb olvasás

![01](img/04.png)

```c
uint8_t getButtons() {
  digitalWrite(STB, LOW);
  writeData(INSTRUCTION_READ_KEY);
  pinMode(DIO, INPUT);
  digitalWrite(CLK, LOW);
  uint8_t data[4];
  for (uint8_t i = 0; i < sizeof(data); i++) {
    data[i] = shiftIn(DIO, CLK, LSBFIRST);
    delayMicroseconds(1);
  }
  pinMode(DIO, OUTPUT);
  digitalWrite(STB, HIGH);
  uint8_t buttons = 0;
  for (uint8_t i = 0; i < 4; i++) {
    buttons |= data[i] << i;
  }
  return buttons;
}
```

## 7 szegmenses kijelző meghajtása

![05](img/05.png)

```c
#define FIRST_DISPLAY_ADDRESS 0xC0
```

![05](img/05b.png)

![06](img/06.png)

* Táblázat félrevezető! B0-B7-ig kell nézni biteket
* A címkiosztás fura

```c
void writeDataAt(uint8_t displayAddress, uint8_t data) {
  digitalWrite(STB, LOW);
  writeData(displayAddress);
  writeData(data);
  digitalWrite(STB, HIGH);
  delayMicroseconds(1);
}

void displayDig(uint8_t digitId, uint8_t pgfedcba) {
  if (digitId > 7) return;
  setDisplayMode(DISPLAY_TURN_ON);
  setDataInstruction(INSTRUCTION_WRITE_DATA | INSTRUCTION_ADDRESS_FIXED);
  writeDataAt(FIRST_DISPLAY_ADDRESS + 14 - (digitId * 2), pgfedcba);
}
```

## PIN-ek definiálása

```c
#define STB 2  // Strobe digital pin
#define CLK 3  // clock digital pin
#define DIO 4  // data digital pin

#include "kijelzo.h"
```

## 7 Szegmenses dekódolás

```c
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
```

## Számláló meghajtása

```c
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
```

## setup() és loop()

```c
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
```