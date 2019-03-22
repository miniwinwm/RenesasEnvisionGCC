#ifndef RTC_H
#define RTC_H

typedef struct
{
    int tm_sec;     /* Seconds (0-59) */
    int tm_min;     /* Minute (0-59) */
    int tm_hour;    /* Hour (0-23) */
    int tm_mday;    /* Day of the month (1-31) */
    int tm_mon;     /* Month (0-11, 0=January) */
    int tm_year;    /* Year since 1900 (100-199, 100=2000)*/
    int tm_wday;    /* Day of the week (0-6, 0=Sunday) */
    int tm_yday;    /* Day of the year (0-365) */
    int tm_isdst;   /* Daylight Savings enabled (>0), disabled (=0), or unknown (<0)*/
} tm_t;

void rtc_init(void);
void rtc_set_time(tm_t *new_time);
void rtc_get_time(tm_t *p_current);

#endif
