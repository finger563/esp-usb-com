#include "usb.hpp"

static espp::Logger logger({.tag = "USB"});
static std::atomic<bool> usb_mounted = false;

/************* TinyUSB descriptors ****************/

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + CFG_TUD_CDC * TUD_CDC_DESC_LEN)
static_assert(CFG_TUD_CDC >= 1, "CFG_TUD_CDC must be at least 1");

static void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);

static tusb_desc_device_t desc_device = {.bLength = sizeof(tusb_desc_device_t),
                                         .bDescriptorType = TUSB_DESC_DEVICE,
                                         .bcdUSB = 0x0200,
                                         .bDeviceClass = TUSB_CLASS_MISC,
                                         .bDeviceSubClass = MISC_SUBCLASS_COMMON,
                                         .bDeviceProtocol = MISC_PROTOCOL_IAD,

                                         .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

                                         .idVendor = 0,  // NOTE: to be filled out later
                                         .idProduct = 0, // NOTE: to be filled out later
                                         .bcdDevice = 0, // NOTE: to be filled out later

                                         // Index of manufacturer description string
                                         .iManufacturer = 0x01,
                                         // Index of product description string
                                         .iProduct = 0x02,
                                         // Index of serial number description string
                                         .iSerialNumber = 0x03,
                                         // Number of configurations
                                         .bNumConfigurations = 0x01};

static const char *usb_string_descriptor[5] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "Finger563",          // 1: Manufacturer, NOTE: to be filled out later
    "ESP USB COM",        // 2: Product, NOTE: to be filled out later
    "20011201",           // 3: Serials, NOTE: to be filled out later
    "USB CDC",            // 4: CDC
};

// update the configuration descriptor with the new report descriptor size
static uint8_t usb_configuration_descriptor[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, EP notification address and size, ESP data address (out, in) and size
    TUD_CDC_DESCRIPTOR(0, 4, 0x81, 8, 0x02, 0x82, 64),
};

void start_usb_cdc(const std::string& serial_number, const std::string& manufacturer_name, const std::string& product_name,
                   uint16_t vid, uint16_t pid, uint16_t bcd, uint16_t usb_bcd) {
  // update the usb descriptors
  usb_string_descriptor[1] = manufacturer_name.c_str();
  usb_string_descriptor[2] = product_name.c_str();
  usb_string_descriptor[3] = serial_number.c_str();
  desc_device.idVendor = vid;
  desc_device.idProduct = pid;
  desc_device.bcdDevice = bcd;
  desc_device.bcdUSB = usb_bcd;

  const tinyusb_config_t tusb_cfg = {.device_descriptor = &desc_device,
                                     .string_descriptor = usb_string_descriptor,
                                     .string_descriptor_count = sizeof(usb_string_descriptor) /
                                                                sizeof(usb_string_descriptor[0]),
                                     .external_phy = false,
                                     .configuration_descriptor = usb_configuration_descriptor,
                                     .self_powered = false,
                                     .vbus_monitor_io = -1};

  if (tinyusb_driver_install(&tusb_cfg) != ESP_OK) {
    logger.error("Failed to install tinyusb driver");
    return;
  }

  tinyusb_config_cdcacm_t acm_cfg = {
    .usb_dev = TINYUSB_USBDEV_0,
    .cdc_port = TINYUSB_CDC_ACM_0,
    .rx_unread_buf_sz = 64,
    .callback_rx = &tinyusb_cdc_rx_callback,
    .callback_rx_wanted_char = NULL,
    .callback_line_state_changed = NULL,
    .callback_line_coding_changed = NULL
  };

  // ensure the CDC ACM is not already initialized
  if (tusb_cdc_acm_initialized(TINYUSB_CDC_ACM_0)) {
    logger.error("CDC ACM 0 already initialized");
    return;
  }

  // Initialize CDC ACM
  if (tusb_cdc_acm_init(&acm_cfg) != ESP_OK) {
    logger.error("Failed to initialize CDC ACM");
    return;
  }

  // validate the CDC ACM is initialized
  if (!tusb_cdc_acm_initialized(TINYUSB_CDC_ACM_0)) {
    logger.error("Failed to initialize CDC ACM");
    return;
  }

  logger.info("USB initialization DONE");
}

void stop_usb_cdc() {
  if (tinyusb_driver_uninstall() != ESP_OK) {
    logger.error("Failed to uninstall tinyusb driver");
    return;
  }
  logger.info("USB initialization DONE");
}

bool is_usb_mounted() {
  return usb_mounted;
}

/********* TinyUSB callbacks ***************/

extern "C" void tud_mount_cb(void) {
  // Invoked when device is mounted
  logger.info("USB Mounted");
  usb_mounted = true;
}

extern "C" void tud_umount_cb(void) {
  // Invoked when device is unmounted
  logger.info("USB Unmounted");
  usb_mounted = false;
}

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event) {
}
