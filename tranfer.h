#ifndef TRANFER_H
#define TRANFER_H

#include "debug.h"
#include "usb_report_parser.h"

enum class Fn: int {
  Waiting = -1,
  None = 0
};

class Tranfer {
  public:
    Fn getFn(UsbReport usbReport);
};

#endif /* TRANFER_H */
