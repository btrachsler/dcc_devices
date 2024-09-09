/**
 * DCC.h - Library for decoding DCC signal.
 * Created by Beat Trachsler, December 1, 2022.
 * Released into the public domain.
*/
#ifndef DCCSignal_h
#define DCCSignal_h

#include <Arduino.h>

class DCC
{
  public:
    static const byte maxNumberOfBytes = 6;  // maximal number of bytes building a dcc packet

    /**
     * DCC packet data
     */
    struct dcc_packet                        // DCC packet
    {
      volatile byte data_size;               // number of data bytes
      volatile byte preamble_size;           // number of preamble bits
      volatile byte data[maxNumberOfBytes];  // data bytes
    };
    
    /** 
     * DCC is initialized with
     * @param dccPin pin for DCC interrupt
     */
    static void begin(byte dccPin);
    /** 
     * test if there is some new DCC data
     * @return true if there is some new DCC data
     */
    static bool hasDCCData();
    /** 
     * get DCC packet
     * @return DCC packet
     */
    static dcc_packet readDCCPacket();
    /** 
     * get DCC data byte with index
     * @param i index of the data byte
     * @return DCC data byte i
     */
    static byte readDCCDataByte(unsigned short i);
    /** 
     * get number of DCC data bytes
     * @return number of DCC data bytes
     */
    static unsigned short readDCCDataSize();
    /** 
     * get size of the preamble
     * @return size of the preamble in bit
     */
    static byte readPreambleSize();

  private:
    static const byte maxMicros = 160;              // maximal number of micros for a period for 1 (= 116 micros)
    static const byte minNumberOfPreambleBits = 10; // minimal size of the preamble (sequence of "1")
    static const byte interruptMode[2];             // interrupt mode

    static byte dccPin;                             // digital pin for the incoming dcc signal
    static volatile unsigned long lastInterruptTime;// timestamp of last interrupt in micros
    static volatile byte countOne;                  // variable counting the sequence of "1" building the preamble
    static volatile short countBits;                // bit counter, starting at the 7th bit
    static dcc_packet current_data;                 // current dcc packet
    static dcc_packet stored_data;                  // stored dcc packet
    static volatile byte checkXOR;                  // checksum for xor byte (error detection)
    static volatile bool findPreamble;              // looking for the preamble?
    static volatile bool newData;                   // new dcc data is ready?
    static volatile bool changeInterruptMode;       // change interrupt mode if necessary
    
    /** 
     * add bit to the current dcc packet
     * @param bit to be added
     */
    static void addBit(bool bit);
    /** 
     * callback function for reading data when the signal is rising from low to high
     */
    static void readData();
};

#endif
