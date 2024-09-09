/*
 * DCC.cpp - Library for decoding DCC signal.
 * Created by Beat Trachsler, December 1, 2022.
 * Released into the public domain.
*/
#include "DCCSignal.h"

#if !defined(digitalPinToInterrupt)
  #define digitalPinToInterrupt(p) ( p == 2 ? 0 : -1)
#endif

const byte DCC::interruptMode[2] = {RISING, FALLING}; // interrupt mode

byte DCC::dccPin;                                     // digital pin for the incoming dcc signal
struct DCC::dcc_packet DCC::current_data;             // current dcc packet
struct DCC::dcc_packet DCC::stored_data;              // stored dcc packet
volatile unsigned long DCC::lastInterruptTime = 0;    // timestamp of last interrupt in micros
volatile byte DCC::countOne = 0;                      // variable counting the sequence of "1" building the preamble
volatile short DCC::countBits = 7;                    // bit counter, starting at the 7th bit
volatile byte DCC::checkXOR = 0;                      // checksum for xor byte (error detection)
volatile bool DCC::findPreamble = true;               // looking for the preamble?
volatile bool DCC::newData = false;                   // new dcc data is ready?
volatile bool DCC::changeInterruptMode = false;       // change interrupt mode if necessary

void DCC::begin(byte pin) {
  dccPin = pin;
  pinMode(dccPin, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(dccPin), readData, interruptMode[changeInterruptMode]);

  current_data.data_size = 0;
  stored_data.data_size = 0;
  current_data.preamble_size = 0;
  stored_data.preamble_size = 0;
  for(byte i = 0; i < maxNumberOfBytes; i++) {
    current_data.data[i] = 0;
    stored_data.data[i] = 0;
  }
}

bool DCC::hasDCCData() {
  if(newData) {
    newData = false;
    return true;
  }
  else {
    return false;
  }
}

DCC::dcc_packet DCC::readDCCPacket() {
  return stored_data;
}

byte DCC::readDCCDataByte(unsigned short i) {
  return stored_data.data[i];
}

unsigned short DCC::readDCCDataSize() {
  return stored_data.data_size;
}

byte DCC::readPreambleSize() {
  return stored_data.preamble_size;
}

void DCC::addBit(bool bit) { 
  if(countBits >= 0) {
    bitWrite(current_data.data[current_data.data_size], countBits, bit);
    countBits--;
  }
  else {
    countBits = 7;
    if(bit == 1) { // if subsequent bit is "1" the dcc packet ends here
      if(current_data.data[current_data.data_size] == checkXOR) {
        stored_data = current_data;
        newData = true;
      }
      else {
        changeInterruptMode = !changeInterruptMode;
        attachInterrupt(digitalPinToInterrupt(dccPin), readData, interruptMode[changeInterruptMode]);
      }
      checkXOR = 0;
      findPreamble = true;
      current_data.data_size = 0;
    }
    else { // otherwise the next byte of the current DCC packet begins
      checkXOR ^= current_data.data[current_data.data_size];
      current_data.data_size++;
    }
  }
}

void DCC::readData() {
  unsigned long deltaTime = micros() - lastInterruptTime;  // measure the time since last interrupt
  lastInterruptTime = micros();    // save the current time stamp in micros
  noInterrupts();                  // disable interrupts to avoid more then one function editing the data
  if(deltaTime < maxMicros) {      // "1" bit found
    if(findPreamble) {             // looking for preamble
      countOne++;
    }
    else {                         // or adding "1" bit
      addBit(1);
    }
  }
  else {                           // "0" bit found
    if(findPreamble) {             // testing if preamble is long enough:
      if(countOne > minNumberOfPreambleBits) {
        current_data.preamble_size = countOne;
        findPreamble = false;
      }
      countOne = 0;
    }
    else {                         // or adding "0" bit
      addBit(0);
    }
  }
  interrupts();                    // enable interrupts again
}
