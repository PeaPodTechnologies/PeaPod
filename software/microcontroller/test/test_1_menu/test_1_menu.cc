#include <Arduino.h>
#include <unity.h>

#include <I2CIP.h>

#include <peapod.h>
#include <air.h>

using namespace I2CIP;


PeaPod::PeaPodModule nomodule = PeaPod::PeaPodModule(I2CIP_MUX_NUM_FAKE);
i2cip_fqa_t fqa_encoder = nomodule.createFQA(I2CIP_MUX_BUS_FAKE, I2CIP_SEESAW_ADDRESS);
i2cip_fqa_t fqa_lcd = nomodule.createFQA(I2CIP_MUX_BUS_FAKE, I2CIP_MCP23008_ADDRESS);
i2cip_fqa_t fqa_sevenseg = nomodule.createFQA(I2CIP_MUX_BUS_FAKE, I2CIP_HT16K33_ADDRESS);

LCD* lcd = nullptr;

unsigned x = 0;
int32_t pos = 0;
bool button = false;

void test_ping_encoder(void) {
  i2cip_errorlevel_t errlev = nomodule.operator()<RotaryEncoder>(fqa_encoder, false);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Rotary Encoder Ping");
}

void test_ping_lcd(void) {
  i2cip_errorlevel_t errlev = nomodule.operator()<MCP23008>(fqa_lcd, false);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "LCD Ping");
}

void test_read_encoder(void) {
  i2cip_args_io_t args = { .g = true, .a = nullptr, .s = nullptr, .b = nullptr };
  i2cip_errorlevel_t errlev = nomodule.operator()<RotaryEncoder>(fqa_encoder, true, args);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "Rotary Encoder Read");

  Device** dptr = devicetree[fqa_encoder];
  RotaryEncoder* encoder = dptr == nullptr ? nullptr : (RotaryEncoder*)*dptr;
  if(encoder == nullptr) {
    TEST_FAIL_MESSAGE("Rotary Encoder not initialized");
    return;
  }

  if(errlev == I2CIP_ERR_NONE) {
    pos = encoder->getCache().encoder;
    button = encoder->getCache().button == PIN_ON;
  }
}

void test_write_lcd(void) {
  if(lcd == nullptr) {
    TEST_FAIL_MESSAGE("LCD not initialized");
    return;
  }

  String display = "LCD TEST\n" + String(++x) + "\n" + String(pos) + "\n" + (button ? "!" : ".");

  i2cip_errorlevel_t errlev = lcd->set(display, (i2cip_lcd_args_t)LCD_ARGS_NONE);
  
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "LCD Write");
}

void test_sevenseg(void) {
  i2cip_ht16k33_mode_t seg_mode = SEG_ASCII;
  i2cip_ht16k33_data_t seg_data = { .h = ('T') + ('E' << 8) + ('S' << 16) + ('T' << 24) };
  i2cip_args_io_t seg_args = { .g = false, .a = nullptr, .s = &seg_data, .b = &seg_mode };
  i2cip_errorlevel_t errlev = nomodule.operator()<HT16K33>(fqa_sevenseg, true, seg_args);
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(I2CIP_ERR_NONE, errlev, "7-Segment Display");
}

void setup() {
  Serial.begin(115200); 

  delay(2000);
  
  UNITY_BEGIN();
  
  delay(1000);
  RUN_TEST(test_ping_encoder);
  delay(1000);
  RUN_TEST(test_ping_lcd);

  Device** dptr = devicetree[fqa_lcd];
  lcd = dptr == nullptr ? nullptr : new LCD((MCP23008*)*dptr);

  delay(1000);
  RUN_TEST(test_sevenseg);
}

unsigned long i = 0;

void loop() {
  delay(1000);
  RUN_TEST(test_read_encoder);
  delay(1000);
  RUN_TEST(test_write_lcd);

  if(i++ > 100) {
    UNITY_END();
    while(true) delay(1000);
  }
}