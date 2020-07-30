#include "hid_out.h"

Adafruit_USBD_HID usbhid;
BLEDis bledis;
BLEHidAdafruit blehid;
static int deviceIndex = 0;
static uint16_t devices[3] = {9, 9, 9};

uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(),
};

static void bleConnectCallback(uint16_t conn_handle) {
  devices[deviceIndex] = conn_handle;
  
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);
  char deviceName[32] = { 0 };
  connection->getPeerName(deviceName, sizeof(deviceName));
  ble_gap_addr_t addr = connection->getPeerAddr();
  LOG("Connected [");
  LOG(conn_handle);
  LOG("] to [");
  LOG(deviceName);
  LOG("] with addr: ");
  LOG(addr.addr[0], HEX);
  LOG("-");
  LOG(addr.addr[1], HEX);
  LOG("-");
  LOG(addr.addr[2], HEX);
  LOG("-");
  LOG(addr.addr[3], HEX);
  LOG("-");
  LOG(addr.addr[4], HEX);
  LOG("-");
  LOG(addr.addr[5], HEX);
  LOG("\r\n");
}

static void bleDisconnectCallback(uint16_t conn_handle, uint8_t reason) {
  (void) conn_handle;
  (void) reason;

  devices[deviceIndex] = 9;
}

void HidOut::begin() {
  useUsb = true;
  useBle = true;
  
  LOG("Begin USB HID\r\n");
  usbhid.setPollInterval(2);
  usbhid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usbhid.begin();

  LOG("Begin BLE HID\r\n");
  Bluefruit.begin(MAX_DEVICE_CONNECT, 0);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName(BLE_NAME);
  Bluefruit.Periph.setConnectCallback(bleConnectCallback);
  Bluefruit.Periph.setDisconnectCallback(bleDisconnectCallback);

  // Configure and Start Device Information Service
  bledis.setManufacturer(BLE_Manufacturer);
  bledis.setModel(BLE_Model);
  bledis.begin();

  blehid.begin();

  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(false);
  Bluefruit.Advertising.setInterval(32, 244);    // in units of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode

  bond_print_list(BLE_GAP_ROLE_PERIPH);
  Bluefruit.clearBonds();
}

void HidOut::sendKeys(UsbReport usbReport) {
  Fn fn = tranfer.getFn(usbReport);
  if (fn == Fn::None) {
    if (usbhid.ready() && useUsb) {
      LOG("Send keys via USB\n\r");
      usbhid.keyboardReport(0, usbReport.mods, usbReport.keys);
    }
    if (useBle && !Bluefruit.Advertising.isRunning() && devices[deviceIndex] != 9) {
      LOG("Send keys via BLE\n\r");
      blehid.keyboardReport(devices[deviceIndex], usbReport.mods, usbReport.keys);
    }
  } else {
    if (fn == Fn::DV0) {
      switchDevice(0);
    } else if (fn == Fn::DV1) {
      switchDevice(1);
    } else if (fn == Fn::DV2) {
      switchDevice(2);
    } else if (fn == Fn::BLE_TG) {
      useBle = !useBle;
      if (!useBle) {
        stopAdv();
      }
    } else if (fn == Fn::USB_TG) {
      useUsb = !useUsb;
    }
  }
}

void HidOut::switchDevice(int _index) {
  if (!useBle) {
    return;
  }
  deviceIndex = _index;
  if (devices[deviceIndex] == 9) {
    startAdv();
  } else {
    stopAdv();
  }
}

void HidOut::startAdv() {
  if (useBle && !Bluefruit.Advertising.isRunning()) {
    Bluefruit.Advertising.start(ADV_TIMEOUT);      // Stop advertising entirely after ADV_TIMEOUT seconds
  }
}

void HidOut::stopAdv() {
  if (Bluefruit.Advertising.isRunning()) {
    Bluefruit.Advertising.stop();
  }
}
