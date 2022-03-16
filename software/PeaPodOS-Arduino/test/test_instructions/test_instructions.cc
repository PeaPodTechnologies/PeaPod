#include <Arduino.h>

#include <utils/base.h>
#include <actuators/actuator.h>
#include <utils/instructions.h>

class TestActuator : public Actuator {
  public:
    TestActuator(void);
  private:
    errorlevel_t initialize(void) override;
    errorlevel_t set(float target) override;
}

TestActuator::TestActuator(void) : Actuator(ACTUATOR_NULL, 0) { }

errorlevel_t TestActuator::initialize(void) {
  return ERR_NONE;
}

errorlevel_t TestActuator::set(float target) {
  return ERR_NONE;
}

TestActuator actuatorA();
TestActuator actuatorB();

static const InstructionActuatorMatrix matrix = {
  .numActuators = 2,
  .actuators = {&actuatorA, &actuatorB},
  .instructions = {"A", "B"}
}

void test_non_json(void) {
  TEST_ASSERT_EQUAL(handleInstructions("what"), ERR_FATAL);
}

void test_incomplete_json(void) {
  TEST_ASSERT_EQUAL(handleInstructions("{\"incomplete"), ERR_FATAL);
}

void test_invalid_target(void) {
  TEST_ASSERT_EQUAL(handleInstructions("{\"incomplete\":abc"), ERR_FATAL);
}

void test_bad_instruction(void) {
  TEST_ASSERT_EQUAL(handleInstructions("{\"C\":0}"), ERR_WARNING);
}

void test_empty_instruction_set(void) {
  TEST_ASSERT_EQUAL(handleInstructions("{}"), ERR_NONE);
  TEST_ASSERT_EQUAL(handleInstructions("{ }"), ERR_NONE);
}

void test_one_instruction(void) {
  TEST_ASSERT_EQUAL(handleInstructions("{\"A\":1.3}"), ERR_NONE);
  TEST_ASSERT_EQUAL(actuatorA.update()->lasttarget, 1.3);
}

void test_multiple_instructions(void) {
  TEST_ASSERT_EQUAL(handleInstructions("{\"A\":2.4,\"B\":4.3}"), ERR_NONE);
  TEST_ASSERT_EQUAL(actuatorA.update()->lasttarget, 2.4);
  TEST_ASSERT_EQUAL(actuatorB.update()->lasttarget, 4.3);
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