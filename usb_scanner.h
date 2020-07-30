#ifndef USB_SCANNNER_H
#define USB_SCANNNER_H

#include "debug.h"
#include "hidboot.h"
#include "usbhub.h"
#include "usb_report_parser.h"
#include "hid_out.h"

class UsbScanner {
  public:
    void begin(HidOut _hidOut);
    void scan();
  private:
    HidOut hidOut;
    int usbState;
    void usbStateUpdate();
    void scanReport();
    void or_report(UsbReport _report);
    void printKeys(UsbReport _report);
};


#endif /* USB_SCANNNER_H */
