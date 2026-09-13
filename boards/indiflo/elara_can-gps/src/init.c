/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file init.c
 *
 * board specific early startup code.  This file implements the
 * board_app_initialize() function that is called early by nsh during startup.
 *
 * Code here is run before the rcS script is invoked; it should start required
 * subsystems and perform board-specific initialization.
 */

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/tasks.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>
#include <errno.h>
#include <syslog.h>

#include <nuttx/board.h>

#include <stm32.h>
#include "board_config.h"
#include "led.h"
#include <stm32_uart.h>

#include <arch/board/board.h>

#include <drivers/drv_hrt.h>
#include <drivers/drv_board_led.h>
#include <drivers/drv_watchdog.h>

#include <systemlib/px4_macros.h>

#include <px4_platform_common/init.h>
#include <px4_platform/gpio.h>

# if defined(FLASH_BASED_PARAMS)
#  include <parameters/flashparams/flashfs.h>
#endif

#define TMR_BASE        STM32_TIM1_BASE
#define TMR_FREQUENCY   STM32_APB2_TIM1_CLKIN
#define TMR_REG(o)      (TMR_BASE+(o))


/************************************************************************************
 * Name: stm32_boardinitialize
 *
 * Description:
 *   All STM32 architectures must provide the following entry point.  This entry point
 *   is called early in the initialization -- after all memory has been configured
 *   and mapped but before any devices have been initialized.
 *
 ************************************************************************************/

__EXPORT void stm32_boardinitialize(void)
{
	watchdog_init();

        /* configure LEDs */

        board_autoled_initialize();

	/* configure pins */
	const uint32_t gpio[] = PX4_GPIO_INIT_LIST;
	px4_gpio_init(gpio, arraySize(gpio));

	// Configure SPI all interfaces GPIO & enable power.
	stm32_spiinitialize();

	//rgb_led(128, 128, 128, 10);

	// Check if button is held. If so go into gps passthrough mode
	if (stm32_gpioread(GPIO_BTN_SAFETY)) {
		rgb_led(128, 128, 128, 10);
		//rgb_led(128, 0, 0, 2);
		stm32_configgpio(GPIO_USART1_TX_GPIO);
		stm32_configgpio(GPIO_USART1_RX_GPIO);
		stm32_configgpio(GPIO_USART2_TX_GPIO);
		stm32_configgpio(GPIO_USART2_RX_GPIO);

		while (1) {
			watchdog_pet();
			stm32_gpiowrite(GPIO_USART2_TX_GPIO, stm32_gpioread(GPIO_USART1_RX_GPIO));
			stm32_gpiowrite(GPIO_USART1_TX_GPIO, stm32_gpioread(GPIO_USART2_RX_GPIO));
		}
	}
}

/****************************************************************************
 * Name: board_app_initialize
 *
 * Description:
 *   Perform application specific initialization.  This function is never
 *   called directly from application code, but only indirectly via the
 *   (non-standard) boardctl() interface using the command BOARDIOC_INIT.
 *
 * Input Parameters:
 *   arg - The boardctl() argument is passed to the board_app_initialize()
 *         implementation without modification.  The argument has no
 *         meaning to NuttX; the meaning of the argument is a contract
 *         between the board-specific initalization logic and the the
 *         matching application logic.  The value cold be such things as a
 *         mode enumeration value, a set of DIP switch switch settings, a
 *         pointer to configuration data read from a file or serial FLASH,
 *         or whatever you would like to do with it.  Every implementation
 *         should accept zero/NULL as a default configuration.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is returned on
 *   any failure to indicate the nature of the failure.
 *
 ****************************************************************************/

__EXPORT int board_app_initialize(uintptr_t arg)
{
	px4_platform_init();

        //uint16_t cr1  = getreg16(TMR_REG(STM32_BTIM_CR1_OFFSET));
        //uint16_t ccer = getreg16(TMR_REG(STM32_GTIM_CCER_OFFSET));
        //uint16_t bdtr = getreg16(TMR_REG(STM32_ATIM_BDTR_OFFSET));

        //uint16_t ccr1 = getreg16(TMR_REG(STM32_GTIM_CCR1_OFFSET));
        //uint16_t ccr2 = getreg16(TMR_REG(STM32_GTIM_CCR2_OFFSET));
        //uint16_t ccr3 = getreg16(TMR_REG(STM32_GTIM_CCR3_OFFSET));

        //uint16_t arr  = getreg16(TMR_REG(STM32_BTIM_ARR_OFFSET));
        //uint16_t psc  = getreg16(TMR_REG(STM32_BTIM_PSC_OFFSET));
	//	
        //syslog(LOG_INFO, "[boot] CR1=0x%04X\n", cr1);
        //syslog(LOG_INFO, "[boot] ccer=0x%04X\n", ccer);
        //syslog(LOG_INFO, "[boot] bdtr=0x%04X\n", bdtr);
        //syslog(LOG_INFO, "[boot] srr=0x%04X\n", arr);
        //syslog(LOG_INFO, "[boot] psc=0x%04X\n", psc);
        //syslog(LOG_INFO, "[boot] CR1=0x%04X\n", ccr1);
        //syslog(LOG_INFO, "[boot] CR2=0x%04X\n", ccr2);
        //syslog(LOG_INFO, "[boot] CR3=0x%04X\n", ccr3);

        //printf("Disabling TIM1...\n");

        //modifyreg16(TMR_REG(STM32_BTIM_CR1_OFFSET), ATIM_CR1_CEN, 0); // this stopped blinking 
        //modifyreg16(TMR_REG(STM32_ATIM_BDTR_OFFSET), ATIM_BDTR_MOE, 0);

	//rgb_led(128, 128, 128, 10);

#if defined(FLASH_BASED_PARAMS)
	static sector_descriptor_t params_sector_map[] = {
		{2, 16 * 1024, 0x08008000},
		{3, 16 * 1024, 0x0800C000},
		{0, 0, 0},
	};

	/* Initialize the flashfs layer to use heap allocated memory */
	int result = parameter_flashfs_init(params_sector_map, NULL, 0);

	if (result != OK) {
		syslog(LOG_ERR, "[boot] FAILED to init params in FLASH %d\n", result);
	}

#endif // FLASH_BASED_PARAMS

	/* Configure the HW based on the manifest */
	//px4_platform_configure();

        rgb_led(0, 255, 0, 0);
	return OK;
}
