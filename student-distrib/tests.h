#ifndef TESTS_H
#define TESTS_H

/* RTC test macros */
#define RTC_HEADER_CHK			11
#define RTC_HEADER_LIM			25
#define RTC_START_PRIN			10
#define NUM_FREQ_TESTS          12
#define NUM_VALID_BYTES          4
#define TICK_DELAY               5
#define INIT_UPPER_BND          RTC_START_PRIN
#define SIXTEEN_UPPER_BND       50
#define SIXTYFOUR_UPPER_BND    200
#define FIVETWELVE_UPPER_BND  2000
#define MAX_FREQ_UPPER_BND    4000
#define OPEN_UPPER_BND        4010
#define READ_UPPER_BND          20
#define BLOCK_CASE_10           RTC_START_PRIN
#define BLOCK_CASE_15           15

#include "rtc.h"

// test launcher
void launch_tests();
void RTC_test();

#endif /* TESTS_H */
