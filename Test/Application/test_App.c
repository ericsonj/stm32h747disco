#include "unity.h"
#include "App.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_APP_sum()
{
	uint32_t res = APP_sum(12, 10);
	TEST_ASSERT_EQUAL_INT32(22, res);
}

void test_APP_multi()
{
	uint32_t res = APP_multi(12, 10);
	TEST_ASSERT_EQUAL_INT32(120, res);
}

