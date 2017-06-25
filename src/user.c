#include "main.h"

extern void tick_handler(void);

/**
  * @brief Timer1 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    TIM1_ClearITPendingBit(TIM1_IT_UPDATE);

    tick_handler();
}

void system_clock_init(uint32_t freq)
{
    /*设置内部高速时钟16M为主时钟*/
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

void timer_init(uint32_t tick)
{
    TIM1_TimeBaseInit(16, TIM1_COUNTERMODE_UP, 1000, 0);//16·ÖÆµ£¬ÏòÉÏ¼ÆÊý£¬¼ÆÊýÍêÁË´¥·¢ÖÐ¶Ï£¬Èç¹ûÏë
    //ÔÙ 1MS ´¥·¢ÖÐ¶Ï£¬Ôò¼ÆÊýÆ÷Ó¦¸Ã¼ÆÊý 1000 ´Î£¬1MHZ/1000=1KHZ£¬ÕýºÃ¾ÍÊÇ 1ms¡£
    TIM1_ARRPreloadConfig(ENABLE);//Ê¹ÄÜ×Ô¶¯ÖØ×°
//  TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);//Êý¾Ý¸üÐÂÖÐ¶Ï
//  TIM1_Cmd(ENABLE);//¿ª¶¨Ê±Æ÷
}

void timer_start(void)
{
    TIM1_Cmd(ENABLE);//开定时器
}

void timer_interrupt_enable(void)
{
    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);//数据更新中断
}

void int_enable(void)
{
    enableInterrupts();   /* enable interrupts */
}

void int_disable(void)
{
    disableInterrupts();  /* disable interrupts */
}

void sleep(void)
{
    wfi();
}

void mcu_init(void)
{
    system_clock_init(16000000);

    int_enable();
}

void tasks_init(void)
{
    // Set up WDT
    // Timeout is WDT count value: approx 32 per millisecond
    // => a count of 64 gives a timeout of approx 2 ms
    // NOTE: WDT driven by RC oscillator - timing varies with temperature
    WATCHDOG_Init(60);

    // Prepare for switch task
    SWITCH_BUTTON1_Init();

    // Prepare for heartbeat-switch task
    HEARTBEAT_SW_U_Init();

    // Prepare for UART1 task (set baud rate)
    UART2_BUF_O_Init(BAUDRATE);

    // Report mode (via buffer)
    UART2_BUF_O_Write_String_To_Buffer("\nNormal mode\n");

    // Add tasks to schedule.
    // Parameters are:
    // A. Task name
    // B. Initial delay / offset (in Ticks)
    // C. Task period (in Ticks): Must be > 0
    //           A                       B  C
    SCH_Add_Task(WATCHDOG_Update,        0, 1);     // Feed iWDT
    SCH_Add_Task(SWITCH_BUTTON1_Update,  0, 1);     // Switch interface
    SCH_Add_Task(HEARTBEAT_SW_U_Update2, 0, 1000);  // Heartbeat LED
    SCH_Add_Task(UART2_BUF_O_Update,     0, 1);     // UART-USB reports
    SCH_Add_Task(protocol_update,        0, 1);    // protocol task
}

void _sdcc_external_startup(void)
{
    IWDG_ReloadCounter();
}
