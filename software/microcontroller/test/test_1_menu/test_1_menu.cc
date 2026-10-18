#include <Arduino.h>
#include <unity.h>

#include <I2CIP.h>

#include <menu.h>

using namespace I2CIP;

unsigned x = 0;
int32_t pos = 0;
bool button = false;

void test_ping_encoder(void) {
  i2cip_errorlevel_t errlev = PeaPod::notamodule.operator()<RotaryEncoder>(PeaPod::Menu::fqa_encoder, false);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Rotary Encoder Ping");
}

void test_ping_lcd(void) {
  i2cip_errorlevel_t errlev = PeaPod::notamodule.operator()<MCP23008>(PeaPod::Menu::fqa_lcd, false);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "LCD Ping");
}

// void test_read_encoder(void) {
//   i2cip_args_io_t args = { .g = true, .a = nullptr, .s = nullptr, .b = nullptr };
//   i2cip_errorlevel_t errlev = PeaPod::notamodule.operator()<RotaryEncoder>(PeaPod::Menu::fqa_encoder, true, args);
//   TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Rotary Encoder Read");

//   Device** dptr = devicetree[PeaPod::Menu::fqa_encoder];
//   RotaryEncoder* encoder = dptr == nullptr || *dptr == nullptr ? nullptr : (RotaryEncoder*)*dptr;
//   if(encoder == nullptr) {
//     TEST_FAIL_MESSAGE("Rotary Encoder not initialized");
//     return;
//   }

//   if(errlev == I2CIP_ERR_NONE) {
//     pos = encoder->getCache().encoder;
//     button = encoder->getCache().button == PIN_ON;
//   }
// }

// void test_write_lcd(void) {
//   Device** dptr = devicetree[PeaPod::Menu::fqa_lcd];
//   TEST_ASSERT_TRUE_MESSAGE(dptr != nullptr && *dptr != nullptr, "LCD not initialized");
//   LCD lcd = LCD((MCP23008*)*dptr);

//   String display = "LCD TEST\n" + String(++x) + "\n" + String(pos) + "\n" + (button ? "!" : ".");

//   i2cip_errorlevel_t errlev = lcd.set(display, (i2cip_lcd_args_t)LCD_ARGS_NONE);
  
//   TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "LCD Write");
// }

void test_ping_sevenseg(void) {
  // i2cip_ht16k33_mode_t seg_mode = SEG_ASCII;
  // i2cip_ht16k33_data_t seg_data = { .h = ('T') + ('E' << 8) + ('S' << 16) + ('T' << 24) };
  // i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };
  // i2cip_errorlevel_t errlev = PeaPod::notamodule.operator()<HT16K33>(PeaPod::fqa_sevenseg, true, seg_args);
  i2cip_errorlevel_t errlev = PeaPod::notamodule.operator()<HT16K33>(PeaPod::fqa_sevenseg, true, _i2cip_args_io_default);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "7-Segment Display");
}

void test_menu_init(void) {
  i2cip_errorlevel_t errlev_init = PeaPod::Menu::initialize();
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev_init, "Menu Initialization");
}

void setup() {
  Serial.begin(115200); 

  delay(2000);
  
  UNITY_BEGIN();
  
  delay(1000);
  RUN_TEST(test_ping_encoder);

  delay(1000);
  RUN_TEST(test_ping_lcd);

  delay(1000);
  RUN_TEST(test_ping_sevenseg);

  delay(1000);
  RUN_TEST(test_menu_init);
  delay(1000);
}

void test_menu_update(void) {
  i2cip_errorlevel_t errlev_menu = PeaPod::Menu::update();
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev_menu, "Menu Update");
}

unsigned long i = 0;

void loop() {
  // delay(1000);
  // RUN_TEST(test_read_encoder);
  // delay(1000);
  // RUN_TEST(test_write_lcd);
  
  RUN_TEST(test_menu_update);
  delay(100);

  if(i++ > 1000) {
    UNITY_END();
    while(true) delay(1000);
  }
}