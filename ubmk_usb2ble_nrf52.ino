#include "Arduino.h"
#include "config.h"
#include "debug.h"
#include "usb_scanner.h"
#include "hid_out.h"

UsbScanner usbScanner;
HidOut hidOut;

void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
  Serial.begin( 115200 );
    #if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
    #endif
  #endif /* DEBUG */
  
  hidOut.begin();
  usbScanner.begin(hidOut);
}

void loop() {
  // put your main code here, to run repeatedly:
  usbScanner.scan();
}
