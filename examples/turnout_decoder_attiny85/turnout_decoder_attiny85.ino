#include <DCCSignal.h>
#include <DCCDevice.h>

// DCC
const byte dccPin = 2;  // digital pin for the incoming dcc signal
const byte accessoryPrefix = 0b10;  // prefix for accessory digital decoder packet
const byte addressMask1 = 0b00111111;  // mask for the six address bits of the first byte
const byte addressMask2 = 0b01110000;  // mask for the three address bits oft the second byte
const byte addressMask3 = 0b00000110;  // mask for the two data bits which are part of the address
const byte dataMask = 0b00001111;  // mask for the four data bits
const bool useRCN = false;  // default setting at Z21 is false

// turnout
const short turnoutAccessoryNumber = 2022;  // number of the turnout used by the Z21
const byte hBridgePin1a = 1;  //  pin 1A of the H-bridge
const byte hBridgePin2a = 0;  //  pin 2A of the H-bridge
const byte hBridgeEnablePin = 3;  //  enable pin of the H-bridge

Turnout turnout(turnoutAccessoryNumber, hBridgePin1a, hBridgePin2a, hBridgeEnablePin);

void setup(void) {
  // DCC
  DCC::begin(dccPin);
}

void loop() {
  // analyze DCC data
  if(DCC::hasDCCData()){    // dcc packet received
    DCC::dcc_packet dccp = DCC::readDCCPacket();
    if(!((dccp.data[0] >> 6)^accessoryPrefix) && dccp.data_size > 1){  // prefix matches accessory digital decoder packet
      // address is computed using the corresponding data bits (cf. NMRA Standard S-9.2.1)
      unsigned short addressBits = dccp.data[0] & addressMask1;
      byte dataBits = (dccp.data[1] & dataMask);
      addressBits |= (~dccp.data[1] & addressMask2) << 2;  // add ones complement of the three address bits from the second byte
      addressBits = addressBits << 2;
      addressBits |= (dccp.data[1] & addressMask3) >> 1;  // add bit 1 and 2 of the data byte to the address
      if(useRCN){
        addressBits -= 3;  // minus 3 because addresses start at 4 (equals number 1 of Z21, cf. RCN-213)
      }
      else {
        addressBits += 1;  // plus 1 because numbers of Z21 start at 1 
      }
      if(addressBits == turnout.readDCCAddress()){ // address matches the stored number
        turnout.switchAccessory(dataBits);
      }
    }
  }
}
