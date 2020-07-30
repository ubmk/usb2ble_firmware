#ifndef USB_REPORT_PARSER_H
#define USB_REPORT_PARSER_H

#include "usbhid.h"

#define REPORT_SIZE 8
#define REPORT_KEYS 6

typedef union {
    uint8_t raw[REPORT_SIZE];
    struct {
        uint8_t mods;
        uint8_t reserved;
        uint8_t keys[REPORT_KEYS];
    };
} __attribute__((packed)) UsbReport;

class UsbRptParser: public HIDReportParser {
  public:
    UsbReport report;
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
    bool hasUpdate();
  private:
    uint16_t timeStamp;
    uint16_t lastTimeStamp;
};


#endif /* KBD_REPORT_PARSER_H */
