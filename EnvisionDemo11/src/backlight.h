#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include <stdint.h>
#include "iodefine.h"

/**
 * Intialize the backlight driver. The backlight is not switch on until a level is set.
 */
void backlight_init(void);

/**
 * Set the backlight level as a percentage
 *
 * @param duty_percent The level, 0 is fully off
 */
void backlight_set(uint8_t duty_percent);

#endif
