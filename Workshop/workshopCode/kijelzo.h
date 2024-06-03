#ifndef KIJELZO_H
#define KIJELZO_H

#define INSTRUCTION_WRITE_DATA 0x40
#define INSTRUCTION_ADDRESS_AUTO 0x40
#define FIRST_DISPLAY_ADDRESS 0xC0
#define DISPLAY_TURN_ON 0x88
#define INSTRUCTION_READ_KEY 0x42
#define INSTRUCTION_ADDRESS_FIXED 0x44

void writeData(uint8_t data) {
  shiftOut(DIO, CLK, LSBFIRST, data);
}

void setDisplayMode(uint8_t displayMode) {
  digitalWrite(STB, LOW);
  writeData(displayMode);
  digitalWrite(STB, HIGH);
  delayMicroseconds(1);
}

void setDataInstruction(uint8_t dataInstruction) {
  digitalWrite(STB, LOW);
  writeData(dataInstruction);
  digitalWrite(STB, HIGH);
  delayMicroseconds(1);
}

void reset() {
  setDisplayMode(DISPLAY_TURN_ON | 0);  //PULSE1/16-ra konfig
  setDataInstruction(INSTRUCTION_WRITE_DATA | INSTRUCTION_ADDRESS_AUTO);
  digitalWrite(STB, LOW);
  writeData(FIRST_DISPLAY_ADDRESS);
  for (uint8_t i = 0; i < 16; i++) {
    writeData(0);
  }
  digitalWrite(STB, HIGH);
}

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

#endif