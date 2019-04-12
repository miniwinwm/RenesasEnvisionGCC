#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "iodefine.h"

/* task stacks */
static StackType_t task_1_stack[128];
static StackType_t task_2_stack[128];
static StackType_t task_3_stack[128];

/* task handles */
static StaticTask_t task_1_handle;
static StaticTask_t task_2_handle;
static StaticTask_t task_3_handle;

/* thread synchronization */
static SemaphoreHandle_t semaphore_handle;
static StaticSemaphore_t semaphore_buffer;

void system_clock_config(void);

static void task_1(void *parameters)
{
	while (true)
	{
		xSemaphoreGive(semaphore_handle);
		vTaskDelay(500U);
    }
}

static void task_2(void *parameters)
{
	static bool flip = false;

	while (true)
	{
		if (xSemaphoreTake(semaphore_handle, portMAX_DELAY))
		{
			flip = !flip;
		}
    }
}

static void task_3(void *parameters)
{
	while (true)
	{
		vTaskDelay(100U);
    }
}

int main( void )
{
	system_clock_config();

	semaphore_handle = xSemaphoreCreateBinaryStatic(&semaphore_buffer);

	/* create task 1 */
	(void)xTaskCreateStatic(task_1, "task 1", 128U, NULL, 1, task_1_stack, &task_1_handle);

	/* create task 2 */
	(void)xTaskCreateStatic(task_2, "task 2", 128U, NULL, 1, task_2_stack, &task_2_handle);

	/* create task 3 */
	(void)xTaskCreateStatic(task_3, "task 3", 128U, NULL, 2, task_3_stack, &task_3_handle);

	/* start the scheduler */
	vTaskStartScheduler();

	/* Don't expect to reach here. */
	return 0;
}

/**
 * FreeRTOS memory allocation for idle task
 *
 * @param ppxIdleTaskTCBBuffer Pointer to pointer of idle task TCB
 * @param ppxIdleTaskStackBuffer Pointer to pointer of idle task stack
 * @param pulIdleTaskStackSize Pointer to idle task stack size
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer,
		uint32_t *pulIdleTaskStackSize)
{
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/**
 * FreeRTOS memory allocation for timer task
 *
 * @param ppxIdleTaskTCBBuffer Pointer to pointer of timer task TCB
 * @param ppxIdleTaskStackBuffer Pointer to pointer of timer task stack
 * @param pulIdleTaskStackSize Pointer to timer task stack size
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
		StackType_t **ppxTimerTaskStackBuffer,
		uint32_t *pulTimerTaskStackSize)
{
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void system_clock_config(void)
{
	/* enable all protect register bits */
	SYSTEM.PRCR.WORD = 0xa50bU;

	/* select resonator as main clock source */
	SYSTEM.MOFCR.BIT.MOSEL = 0U;

	/* stop the HOCO, not using it */
	SYSTEM.HOCOCR.BIT.HCSTP = 1U;

	/* turn off power to HOCO */
	SYSTEM.HOCOPCR.BIT.HOCOPCNT = 1U;

    /* disable the sub-clock oscillator as no crystal fitted */
    SYSTEM.SOSCCR.BIT.SOSTP = 1U;

	/* not using USB clock */
	SYSTEM.SCKCR2.WORD = 0x0001U;

	/* set drive level for 12MHz input */
	SYSTEM.MOFCR.BIT.MODRV2 = 2U;

	/* set wait time for starting main clock oscillator */
	SYSTEM.MOSCWTCR.BYTE = 0x53U;

	/* start main clock oscillator */
    SYSTEM.MOSCCR.BIT.MOSTP = 0U;

	/* set rom wait state appropriate to main clock speed */
	SYSTEM.ROMWT.BIT.ROMWT = 2U;

	/* set PLL division to 1 */
	SYSTEM.PLLCR.BIT.PLIDIV = 0U;

	/* set PLL multiplication to 20 */
	SYSTEM.PLLCR.BIT.STC = 39U;

	/* start the PLL */
	SYSTEM.PLLCR2.BIT.PLLEN = 0U;

	/* wait for clock stabilization */
	while (0 == SYSTEM.OSCOVFSR.BIT.PLOVF)
	{
		__asm("NOP");
	}

	/* ICLK to 120MHz */
	SYSTEM.SCKCR.BIT.ICK = 1U;

	/* FCLK to 60 MHz */
	SYSTEM.SCKCR.BIT.FCK = 2U;

	/* PCLKA to 120MHz */
	SYSTEM.SCKCR.BIT.PCKA = 1U;

	/* PCLKB to 60MHz */
	SYSTEM.SCKCR.BIT.PCKB = 2U;

	/* PCLKC to 60MHz */
	SYSTEM.SCKCR.BIT.PCKC = 2U;

	/* PCLKD to 60MHz */
	SYSTEM.SCKCR.BIT.PCKD = 2U;

	/* SDCLK not used */
	SYSTEM.SCKCR.BIT.PSTOP0 = 1U;

	/* BCLK not used */
	SYSTEM.SCKCR.BIT.PSTOP1 = 1U;

	/* switch main clock source to PLL */
	SYSTEM.SCKCR3.BIT.CKSEL = 4U;

	/* stop LOCO as finished with it now */
	SYSTEM.LOCOCR.BYTE = 1U;

	/* disable all protect register bits */
	SYSTEM.PRCR.WORD = 0xa500U;
}
