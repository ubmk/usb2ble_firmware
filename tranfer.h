#ifndef TRANFER_H
#define TRANFER_H

#include "debug.h"
#include "usb_report_parser.h"

enum class Fn: int {
  None = 0,
  DV0 = 1,
  DV1 = 2,
  DV2 = 3,
  BLE_TG = 20,
  USB_TG = 31
};

class Tranfer {
  public:
    void begin();
    Fn getFn(UsbReport usbReport);
};

#endif /* TRANFER_H */
