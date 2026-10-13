
#include <unity.h>
#include <stdint.h>
#include "drivers/motor.h"

/* Compile the pure-C motor logic in the native test environment. */
#include "../../src/drivers/motor.c"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_left_motor_forward(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_LEFT, 300, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_PWM, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_LOW, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(300U, output.in1_duty);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in2_duty);
}

void test_left_motor_reverse(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_LEFT, -300, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_LOW, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_PWM, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in1_duty);
    TEST_ASSERT_EQUAL_UINT16(300U, output.in2_duty);
}

void test_right_motor_forward(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_RIGHT, 300, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_PWM, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_LOW, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(300U, output.in1_duty);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in2_duty);
}

void test_right_motor_reverse(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_RIGHT, -300, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_PWM, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_HIGH, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(700U, output.in1_duty);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in2_duty);
}

void test_motor_coast(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_LEFT, 0, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_LOW, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_LOW, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in1_duty);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in2_duty);
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_RIGHT, 0, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_LOW, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_LOW, output.in2_mode);
}

void test_motor_brake(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeBrake(&output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_HIGH, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_HIGH, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in1_duty);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in2_duty);
}

void test_motor_inverted_direction(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_LEFT, 300, 1U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_LOW, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_PWM, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(300U, output.in2_duty);
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_RIGHT, 300, 1U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_PWM, output.in1_mode);
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_HIGH, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(700U, output.in1_duty);
}

void test_motor_duty_limits(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_LEFT, 1500, 0U, &output));
    TEST_ASSERT_EQUAL_UINT16(1000U, output.in1_duty);
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_LEFT, -1500, 0U, &output));
    TEST_ASSERT_EQUAL_UINT16(1000U, output.in2_duty);
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_RIGHT, -1500, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_HIGH, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in1_duty);
}

void test_motor_invalid_parameters(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(0U, motorComputeOutput(2U, 300, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(0U, motorComputeOutput(MOTOR_LEFT, 300, 2U, &output));
    TEST_ASSERT_EQUAL_UINT8(0U, motorComputeOutput(MOTOR_LEFT, 300, 0U, NULL));
    TEST_ASSERT_EQUAL_UINT8(0U, motorComputeBrake(NULL));
}

void test_motor_full_duty(void)
{
    motor_output_t output = {0};
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_LEFT, 1000, 0U, &output));
    TEST_ASSERT_EQUAL_UINT16(1000U, output.in1_duty);
    TEST_ASSERT_EQUAL_UINT8(1U, motorComputeOutput(MOTOR_RIGHT, -1000, 0U, &output));
    TEST_ASSERT_EQUAL_UINT8(MOTOR_OUTPUT_HIGH, output.in2_mode);
    TEST_ASSERT_EQUAL_UINT16(0U, output.in1_duty);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_left_motor_forward);
    RUN_TEST(test_left_motor_reverse);
    RUN_TEST(test_right_motor_forward);
    RUN_TEST(test_right_motor_reverse);
    RUN_TEST(test_motor_coast);
    RUN_TEST(test_motor_brake);
    RUN_TEST(test_motor_inverted_direction);
    RUN_TEST(test_motor_duty_limits);
    RUN_TEST(test_motor_invalid_parameters);
    RUN_TEST(test_motor_full_duty);
    return UNITY_END();
}
