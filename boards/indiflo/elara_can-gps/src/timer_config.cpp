/****************************************************************************
 *
 *   Copyright (c) 2026 IndiFlo. All rights reserved.
 *
 ****************************************************************************/

/**
 * @file timer_config.cpp
 *
 * PWM configuration for the onboard RGB LED.
 */

#include <px4_arch/io_timer_hw_description.h>

#if defined(BOARD_HAS_LED_PWM)

constexpr io_timers_t led_pwm_timers[MAX_LED_TIMERS] = {
	initIOTimer(Timer::Timer1),
};

static inline constexpr timer_io_channels_t initRGBLedChannel(Timer::TimerChannel timer, GPIO::GPIOPin pin,
		unsigned channel)
{
	timer_io_channels_t ret = initIOTimerChannel(led_pwm_timers, timer, pin);

	// The LED anodes can be supplied from 3.3 V or 5 V, so drive each cathode open-drain.
	ret.gpio_out |= GPIO_OPENDRAIN;
	ret.masks = ATIM_CCER_CC1P << ((channel - 1) * 4);
	return ret;
}

constexpr timer_io_channels_t led_pwm_channels[MAX_TIMER_LED_CHANNELS] = {
	initRGBLedChannel({Timer::Timer1, Timer::Channel1}, {GPIO::PortA, GPIO::Pin8}, 1),
	initRGBLedChannel({Timer::Timer1, Timer::Channel2}, {GPIO::PortA, GPIO::Pin9}, 2),
	initRGBLedChannel({Timer::Timer1, Timer::Channel3}, {GPIO::PortA, GPIO::Pin10}, 3),
};

#endif // BOARD_HAS_LED_PWM
