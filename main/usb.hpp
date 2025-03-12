#pragma once

#include <cstdint>
#include <vector>

#include "logger.hpp"

extern "C" {
#include <class/cdc/cdc_device.h>
#include <tinyusb.h>
#include <tusb.h>
#include <tusb_cdc_acm.h>
}

void start_usb_cdc(const std::string& serial_number,
                   const std::string& manufacturer_nam="Finger563",
                   const std::string& product_name="ESP_USB_COM",
                   uint16_t vid=0xcafe,
                   uint16_t pid=0x0001,
                   uint16_t bcd=0x0100,
                   uint16_t usb_bcd=0x0200
                   );
void stop_usb_cdc();
bool is_usb_mounted();
