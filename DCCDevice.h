/**
 * DCCDevice.h - Library for DCC Devices like turnouts and signals.
 * Created by Beat Trachsler, December 1, 2022.
 * Released into the public domain.
*/
#ifndef DCCDevice_h
#define DCCDevice_h

#include <Arduino.h>

/**
 * DCCDevice class (for lights, turnouts, signals, etc.)
 */
class DCCDevice
{
  public:
    /**
     * constructor with
     * @param DCCAddress address of the device
     */
    DCCDevice(unsigned short DCCAddress);
    /**
     * get the address of the device
     * @return DCC address of the device
     */
    virtual unsigned short readDCCAddress();
    
  protected:
    unsigned short _DCCAddress; // address of the device
};

/**
 * Accessory class for lights, turnouts, signals, etc.
 */
class Accessory : public DCCDevice
{
  public:
    /**
     * constructor with
     * @param address of the accessory
     */
    Accessory(unsigned short address);
    /**
     * virtual method to switch accessory according to
     * @param dataBits data bits containing the settings
     */
    virtual void switchAccessory(byte dataBits) {};
};

/**
 * Light class for switching lights
 */
class Light : public Accessory
{
  public:
    /**
     * constructor with
     * @param address of the light
     * @param pin pin for switching the light
     */
    Light(unsigned short address, byte pin);
    /**
     * switch light according to
     * @param dataBits data bits containing the settings
     */
    void switchAccessory(byte dataBits);

  private:
    byte _pin;
};

/**
 * Turnout class for switching Turnouts
 */
class Turnout : public Accessory
{
  public:
    /**
     * constructor with
     * @param address of the turnout
     * @param pin1 first pin of h-bridge for switching the turnout
     * @param pin2 second pin of h-bridge for switching the turnout
     * @param enablePin enable pin for switch drive
     */
    Turnout(unsigned short address, byte pin1, byte pin2, byte enablePin);
    /**
     * switch turnout according to
     * @param dataBits data bits containing the settings
     */
    void switchAccessory(byte dataBits);
    /**
     * get turnout state 
     * @return turnout state (true for straight, false for curve)
     */
    bool readTurnoutState();
    
  private:
    byte _pin1;
    byte _pin2;
    byte _enablePin;
    bool _turnoutState;
};

/**
 * ThreeLightSignal class for switching three light signals
 */
class ThreeLightSignal : public Accessory
{
  public:
    /**
     * constructor with
     * @param address1 first address of the signal
     * @param address2 second address of the signal
     * @param pin1 pin for green signal led
     * @param pin2 pin for yellow signal led
     * @param pin3 pin for red signal led
     */
    ThreeLightSignal(unsigned short address1, unsigned short address2, byte pin1, byte pin2, byte pin3);
    /**
     * get the address of the device
     * @return DCC address of the device
     */
    unsigned short readDCCAddress(); 
    /**
     * switch signal according to
     * @param dataBits data bits containing the settings
     */
    void switchAccessory(byte dataBits);

  private:
    unsigned short _DCCaddress2;
    byte _pin1;
    byte _pin2;
    byte _pin3;
    bool _signalBit1;
    bool _firstBitSet;
};

#endif
