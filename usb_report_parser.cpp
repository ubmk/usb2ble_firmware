#include "usb_report_parser.h"

void UsbRptParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  ::memcpy(&report, buf, sizeof(UsbReport));
  timeStamp = millis();
}

bool UsbRptParser::hasUpdate() {
  if (timeStamp != lastTimeStamp) {
    lastTimeStamp = timeStamp;
    return true;
  }
  return false;
}
