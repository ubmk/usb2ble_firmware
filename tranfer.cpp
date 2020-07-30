#include "tranfer.h"

Fn Tranfer::getFn(UsbReport usbReport) {
  if (usbReport.keys[0] == 0x68) {
    return Fn::DV0;
  } else if (usbReport.keys[0] == 0x69) {
    return Fn::DV1;
  } else if (usbReport.keys[0] == 0x6A) {
    return Fn::DV2;
  }
  return Fn::None;
}
