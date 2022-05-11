#include <Arduino.h>
#include <unity.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <utils/instructions.h>

class TestActuator : public Actuator {
  public:
    TestActuator(void);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;
};

TestActuator::TestActuator(void) : Actuator(ACTUATOR_NULL, 0) { }

errorlevel_t TestActuator::initialize(void) {
  return ERR_NONE;
}

errorlevel_t TestActuator::set(float target) {
  return ERR_NONE;
}

TestActuator actuatorA = TestActuator();
TestActuator actuatorB = TestActuator();

const char* ins[2] = { "A", "B" };
Actuator* acts[2] = { &actuatorA, &actuatorB };

static const InstructionActuatorMatrix matrix = {
  .numActuators = 2,
  .actuators = acts,
  .instructions = ins
};

void test_non_json(void) {
  uint8_t result = handleInstructions("hello world", &matrix);
  TEST_ASSERT_EQUAL_UINT8(ERR_FATAL, result);
}

void test_incomplete_json(void) {
  uint8_t result = handleInstructions("{\"incomplete", &matrix);
  TEST_ASSERT_EQUAL_UINT8(ERR_FATAL, result);
}

void test_invalid_target(void) {
  uint8_t result = handleInstructions("{\"incomplete\":abc", &matrix);
  TEST_ASSERT_EQUAL_UINT8(ERR_FATAL, result);
}

void test_bad_instruction(void) {
  uint8_t result = handleInstructions("{\"C\":0}", &matrix);
  TEST_ASSERT_EQUAL_UINT8(ERR_WARNING, result);
}

void test_empty_instruction_set(void) {
  uint8_t result = handleInstructions("{}"), &matrix);
  TEST_ASSERT_EQUAL_UINT8(ERR_NONE, result);
}

void test_one_instruction(void) {
  uint8_t result = handleInstructions("{\"A\":1.3}", &matrix);
  TEST_ASSERT_EQUAL_UINT8(ERR_NONE, result);
  float target = actuatorA.update()->lasttarget;
  TEST_ASSERT_EQUAL_FLOAT(1.3, target);
}

void test_multiple_instructions(void) {
  uint8_t result = handleInstructions("{\"A\":2.4,\"B\":4.3}", &matrix);
  TEST_ASSERT_EQUAL_UINT8(ERR_NONE, result);
  float target = actuatorA.update()->lasttarget;
  TEST_ASSERT_EQUAL_FLOAT(2.4, target);
  target = actuatorB.update()->lasttarget;
  TEST_ASSERT_EQUAL_FLOAT(4.3, target);
}

void setup(void) {
  delay(2000);

  UNITY_BEGIN();

  RUN_TEST(test_non_json);
  RUN_TEST(test_incomplete_json);
  RUN_TEST(test_invalid_target);
  RUN_TEST(test_bad_instruction);
  RUN_TEST(test_empty_instruction_set);
  RUN_TEST(test_one_instruction);
  RUN_TEST(test_multiple_instructions);

  UNITY_END();
}

void loop(void) { }