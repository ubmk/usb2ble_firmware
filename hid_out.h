#ifndef HID_OUT_H
#define HID_OUT_H

#include "bluefruit.h"
#include "utility/bonding.h"
#include "Adafruit_TinyUSB.h"

#include "config.h"
#include "debug.h"
#include "usb_report_parser.h"
#include "tranfer.h"

class HidOut {
  public:
    void begin();
    void sendKeys(UsbReport usbReport);
  private:
    Tranfer tranfer;
    bool useUsb;
    bool useBle;
};

#endif /* HID_OUT_H */
