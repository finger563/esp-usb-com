#include <chrono>
#include <thread>

#include <bootloader_random.h>
#include <esp_random.h>
#include <esp_system.h>

#include "logger.hpp"
#include "task.hpp"

#include "usb.hpp"

using namespace std::chrono_literals;

extern "C" void app_main(void) {
  espp::Logger logger({.tag = "USB-COM", .level = espp::Logger::Verbosity::DEBUG});

  logger.info("Bootup");

  // enable internal entropy source (SAR ADC) manually, since we won't be using
  // WiFi or BLE
  bootloader_random_enable();

  // generate a random serial number
  std::string serial_number = "";
  // generate a random 10-digit serial number
  for (int i = 0; i < 10; i++) {
    serial_number += std::to_string(esp_random() % 10);
  }
  // now start the USB CDC
  start_usb_cdc(serial_number);

  // now loop forever
  while (true) {
    std::this_thread::sleep_for(100ms);
    if (is_usb_mounted()) {
      logger.info("USB mounted, resetting processor in 100ms");
      std::this_thread::sleep_for(100ms);
      esp_restart();
    }
  }
}
