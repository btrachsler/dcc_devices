/*
 * DCCDevice.cpp - Library for DCC Devices like turnouts and signals.
 * Created by Beat Trachsler, December 1, 2022.
 * Released into the public domain.
*/
#include "DCCDevice.h"

DCCDevice::DCCDevice(unsigned short DCCAddress)
  : _DCCAddress(DCCAddress)
{
}

unsigned short DCCDevice::readDCCAddress() {
  return _DCCAddress;
}

Accessory::Accessory(unsigned short address)
  : DCCDevice(address)
{
}

Light::Light(unsigned short address, byte pin)
  : Accessory(address), _pin(pin)
{
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void Light::switchAccessory(byte dataBits) {
  // switch light according to the first data bit
  if(bitRead(dataBits, 0)) {
    digitalWrite(_pin, HIGH);
  }
  else {
    digitalWrite(_pin, LOW);
  }
}

Turnout::Turnout(unsigned short address, byte pin1, byte pin2, byte enablePin)
  : Accessory(address), _pin1(pin1), _pin2(pin2), _enablePin(enablePin)
{
  pinMode(_pin1, OUTPUT);
  pinMode(_pin2, OUTPUT);
  pinMode(_enablePin, OUTPUT);
  _turnoutState = true;
}

void Turnout::switchAccessory(byte dataBits) {
  // switch turnout according to the first and the third data bit
  _turnoutState = bitRead(dataBits, 0);
  if(_turnoutState) {
    digitalWrite(_pin1, HIGH);
    digitalWrite(_pin2, LOW);
  }
  else {
    digitalWrite(_pin1, LOW);
    digitalWrite(_pin2, HIGH);
  }
  if(bitRead(dataBits, 3)) {
    digitalWrite(_enablePin, HIGH);
  }
  else {
    digitalWrite(_enablePin, LOW);
  }
}

bool Turnout::readTurnoutState() {
  return _turnoutState;
}

ThreeLightSignal::ThreeLightSignal(unsigned short address1, unsigned short address2,  byte pin1, byte pin2, byte pin3)
  : Accessory(address1), _DCCaddress2(address2), _pin1(pin1), _pin2(pin2), _pin3(pin3)
{
  pinMode(_pin1, OUTPUT);
  pinMode(_pin2, OUTPUT);
  pinMode(_pin3, OUTPUT);
  digitalWrite(_pin1, HIGH);
  digitalWrite(_pin2, HIGH);
  digitalWrite(_pin3, LOW);
  _firstBitSet = false;
}

unsigned short ThreeLightSignal::readDCCAddress() {
  if(_firstBitSet) {
    return _DCCaddress2;
  }
  else {
    return _DCCAddress;
  }
}

void ThreeLightSignal::switchAccessory(byte dataBits) {
  if(_firstBitSet) {
    // switch signal according to the two signal bits
    bool signalBit2 = bitRead(dataBits, 0);
    if(_signalBit1 && signalBit2) {        // yellow signal
      digitalWrite(_pin1, HIGH);
      digitalWrite(_pin3, HIGH);
      digitalWrite(_pin2, LOW);
    }
    else if(!_signalBit1 && !signalBit2) { // green signal
      digitalWrite(_pin2, HIGH);
      digitalWrite(_pin3, HIGH);  
      digitalWrite(_pin1, LOW);
    }
    else {                                 // red signal
      digitalWrite(_pin1, HIGH);
      digitalWrite(_pin2, HIGH);
      digitalWrite(_pin3, LOW);
    }
    _firstBitSet = false;
  }
  else {
    _signalBit1 = bitRead(dataBits, 0);    // store the first bit of the signal
    _firstBitSet = true;
  }
}
