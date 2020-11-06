#include "hid_out.h"

Adafruit_USBD_HID usbhid;
BLEDis bledis;
BLEHidAdafruit blehid;

uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(),
};

static void bleConnectCallback(uint16_t conn_handle) {
  BLEConnection* connection = Bluefruit.Connection(conn_handle);
  char deviceName[32] = { 0 };
  connection->getPeerName(deviceName, sizeof(deviceName));
  ble_gap_addr_t addr = connection->getPeerAddr();
  
  LOG("Connected [");
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

  LOG("Disconnected [");
  LOG(conn_handle);
  LOG("]\r\n");
}

void HidOut::begin() {
  useUsb = true;
  useBle = true;
  
  LOG("Begin USB HID\r\n");
  usbhid.setPollInterval(2);
  usbhid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usbhid.begin();

  LOG("Begin BLE HID\r\n");
  Bluefruit.configPrphBandwidth(BANDWIDTH_HIGH);
  Bluefruit.begin(1, 0);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName(BLE_NAME);
  Bluefruit.autoConnLed(true);
  Bluefruit.setConnLedInterval(1000);
  Bluefruit.Periph.setConnInterval(6, 12);
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
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds

  bond_print_list(BLE_GAP_ROLE_PERIPH);
  // Bluefruit.clearBonds();
  // startAdv();
}

void HidOut::sendKeys(UsbReport usbReport) {
  Fn fn = tranfer.getFn(usbReport);
  if (fn == Fn::None) {
    if (usbhid.ready() && useUsb) {
      LOG("Send keys via USB\n\r");
      usbhid.keyboardReport(0, usbReport.mods, usbReport.keys);
    }
    if (useBle && !Bluefruit.Advertising.isRunning()) {
      LOG("Send keys via BLE\n\r");
      blehid.keyboardReport(0, usbReport.mods, usbReport.keys);
    }
  } else {
  }
}
