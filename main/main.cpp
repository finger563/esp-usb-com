#include <chrono>
#include <thread>

#include <bootloader_random.h>
#include <esp_random.h>
#include <esp_system.h>

#include "gaussian.hpp"
#include "logger.hpp"
#include "task.hpp"
#include "qtpy.hpp"

#include "usb.hpp"

using namespace std::chrono_literals;

extern "C" void app_main(void) {
  espp::Logger logger({.tag = "USB-COM", .level = espp::Logger::Verbosity::DEBUG});

  logger.info("Bootup");

  auto &qtpy = espp::QtPy::get();

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

  logger.info("Initializing the button");
  qtpy.initialize_button(nullptr);

  // Initialize and test the LED
  logger.info("Initializing the LED");
  qtpy.initialize_led();

  espp::Gaussian gaussian{{.gamma = 0.1f, .alpha = 1.0f, .beta = 0.5f}};

  // now loop forever
  int current_sleep_ms = 0;
  int total_sleep_ms = 1000;
  while (true) {
    if (is_usb_mounted()) {
      float led_brightness = gaussian((total_sleep_ms - current_sleep_ms) / (float)total_sleep_ms);

      qtpy.led(espp::Rgb(0.0, led_brightness, 0.0));
      logger.info("USB mounted, resetting processor in 1000ms");
      if (!qtpy.button_state()) {
        std::this_thread::sleep_for(30ms);
        current_sleep_ms += 30;
      }
      if (current_sleep_ms >= total_sleep_ms) {
        esp_restart();
      }
    } else {
      std::this_thread::sleep_for(100ms);
    }
  }
}
