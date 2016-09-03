
/* ThroughHardwareSerial enables PJON communication through the Serial port.
   Copyright (c) 2016 by Giovanni Blu Mitolo and Fred Larsen All rights reserved.

   With ThroughHardwareSerial PJON can run through the Serial port of your device.
   Both ports should be free from usb computer connection and data pin should
   be wired inverted (TX->RX, RX->TX).

   Why to add PJON over Serial?
   Serial communication is an hardware integrated communication medium that can
   reach very fast communication speed but it is quite basic.

   Serial Downsides:
     - No CRC, checksum or parity bit (ensure correct data transfert)
     - No transmission certainty (lack of acknowledge from receiver)
     - No packet handling (no retrasmission in case of error)
     - No idea of id (no chance to connect more than 2 devices together)

   Adding PJON over Serial you get:
     - Correct transmission certainty with the use of CRC 8-bit
     - Acnowledge byte to inform transmitter of correct data reception
     - Packet handling, i.e. example schedule a repeated packet transmission.
     - Connect 2 slaves with a 1 master having univocal communication with device id

   ThroughHardwareSerial can run a 2 device bus in multi-master or a n devices bus
   in a master-slave setup. (n still unkown, tested with 1 master and 2 slaves) */

#include <Arduino.h>
#include <SoftwareSerial.h>

#define THROUGH_HARDWARE_SERIAL_MAX_TIME 10000 // Wait up to 10 ms for an incoming byte
#define THROUGH_HARDWARE_SERIAL_MIN_CHANNEL_SILELNCE_WINDOW 500 // min 0.5 ms silence to start sending

class ThroughHardwareSerial {
  public:
    Stream *serial = NULL;

    /* Pass the Serial port where you want to operate with */

    void set_serial(HardwareSerial *serial_port) {
      serial = serial_port;
    };

    void set_serial(SoftwareSerial *serial_port) {
      serial = serial_port;
    };

    /* Returns the Serial object value i.e. if(Serial) */

    boolean can_start(uint8_t input_pin, uint8_t output_pin) {
      if(serial->available() > 0) return false;    
  
      if (micros() - _last_busy_channel_ts < random(THROUGH_HARDWARE_SERIAL_MIN_CHANNEL_SILELNCE_WINDOW, 2 * THROUGH_HARDWARE_SERIAL_MIN_CHANNEL_SILELNCE_WINDOW) ) return false;

      return (serial != NULL); // Check if initialized
    };


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b, uint8_t input_pin, uint8_t output_pin) {
      serial->write(b);
      serial->flush();
    };


    /* Try to receive a byte with a maximum waiting time */

    uint16_t receive_byte(uint8_t input_pin, uint8_t output_pin) {
      uint32_t time = micros();
      while(micros() - time < THROUGH_HARDWARE_SERIAL_MAX_TIME)
        if(serial->available() > 0){
          _last_busy_channel_ts = micros();
          return (uint8_t)serial->read();
        };
      return FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response(uint8_t input_pin, uint8_t output_pin) {
      return receive_byte(input_pin, output_pin);
    };


    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response, uint8_t input_pin, uint8_t output_pin) {
      send_byte(response, input_pin, output_pin);
    };

   private:
     uint32_t     _last_busy_channel_ts;
};
