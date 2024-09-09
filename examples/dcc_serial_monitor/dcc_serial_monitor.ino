#include <DCCSignal.h>

const byte dccPin = 2;  // digital pin with the incoming dcc signal

void setup(void) {
  Serial.begin(115200);

  DCC::begin(dccPin);
}

void loop() {
  // analyze DCC data
  if(DCC::hasDCCData()) {    // dcc packet received
    DCC::dcc_packet dccp = DCC::readDCCPacket();
    Serial.print("DCC: ");
    for(byte i = 0; i <= dccp.data_size; i++) {
      Serial.print(dccp.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}
