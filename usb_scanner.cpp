#include "usb_scanner.h"

USB usbHost;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> kbd0(&usbHost);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> kbd1(&usbHost);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> kbd2(&usbHost);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> kbd3(&usbHost);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> kbd4(&usbHost);
UsbRptParser parser0;
UsbRptParser parser1;
UsbRptParser parser2;
UsbRptParser parser3;
UsbRptParser parser4;

static UsbReport usbReport;

void UsbScanner::begin(HidOut _hidOut) {
  if (usbHost.Init() == -1) {
    LOG("OSC did not start.");
  } else {
    LOG("USB Host Shield init.\r\n");
  }
  kbd0.SetReportParser(0, (HIDReportParser*)&parser0);
  kbd1.SetReportParser(0, (HIDReportParser*)&parser1);
  kbd2.SetReportParser(0, (HIDReportParser*)&parser2);
  kbd3.SetReportParser(0, (HIDReportParser*)&parser3);
  kbd4.SetReportParser(0, (HIDReportParser*)&parser4);
  hidOut = _hidOut;
}

void UsbScanner::scan() {
  scanReport();
  usbHost.Task();
  usbStateUpdate();
}

void UsbScanner::usbStateUpdate() {
  int newState = usbHost.getUsbTaskState();
  if (usbState != newState) {
    usbState = newState;
    LOG("USB Host state: ");
    LOG(usbState);
    LOG("\r\n");
    if (usbState == USB_STATE_RUNNING) {
      LOG("USB Host speed: ");
      LOG(usbHost.getVbusState()==FSHOST ? "full" : "low");
      LOG("\r\n");
    }
  }
}

void UsbScanner::or_report(UsbReport _report) {
    usbReport.mods |= _report.mods;
    for (uint8_t i = 0; i < REPORT_KEYS; i++) {
        for (uint8_t j = 0; j < REPORT_KEYS; j++) {
          if (_report.keys[i] && ! usbReport.keys[j]) {
            usbReport.keys[j] = _report.keys[i];
            break;
          }
        }
    }
}

void UsbScanner::scanReport() {
  if (usbState == USB_STATE_RUNNING) {
    // check report came from keyboards
    if (parser0.hasUpdate() ||
        parser1.hasUpdate() ||
        parser2.hasUpdate() ||
        parser3.hasUpdate() ||
        parser4.hasUpdate()) {
        // clear and integrate all reports
        usbReport = {};
        or_report(parser0.report);
        or_report(parser1.report);
        or_report(parser2.report);
        or_report(parser3.report);
        or_report(parser4.report);

        // Send key
        printKeys(usbReport);
        hidOut.sendKeys(usbReport);
    }
  }
}

void UsbScanner::printKeys(UsbReport _report) {
  LOG("Mod: [ ");
  LOG(_report.mods, HEX);
  LOG(" ] - Keys: [ ");
  LOG(_report.keys[0], HEX);
  LOG(", ");
  LOG(_report.keys[1], HEX);
  LOG(", ");
  LOG(_report.keys[2], HEX);
  LOG(", ");
  LOG(_report.keys[3], HEX);
  LOG(", ");
  LOG(_report.keys[4], HEX);
  LOG(" ]\r\n");
}
