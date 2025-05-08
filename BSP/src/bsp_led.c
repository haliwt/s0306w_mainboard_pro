/*
 * bsp_led.c
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */
#include "bsp.h"

/**************************************************************************
 * power_on_led(void)
 * 功能:开启所有LED
 * 参数:无
 * 返回值:无
 ************************************************************************/

/**************************************************************************
 * power_off_led(void)
 * 功能:关闭所有LED
 * 参数:无
 * 返回值:无
 ************************************************************************/

/************************************************************************
 * LED_Power_Breathing(void)
 * 功能:LED呼吸灯函数
 * 参数:无
 * 返回值:无
 ************************************************************************/
 #if 0
void LED_Power_Breathing(void)
{
    static uint8_t breathing_time = 0;
    static uint8_t led_state = 0;
    
    breathing_time++;
    if(breathing_time >= 100)  // 呼吸周期1秒
    {
        breathing_time = 0;
        led_state = !led_state;
        
        if(led_state)
        {
            LED_POWER_ON();
        }
        else
        {
            LED_POWER_OFF();
        }
    }
}
#endif 
/************************************************************************
 * Function Name: LED_Power_Breathing(void)
 * 功能:
 * 参数:无
 * 返回值:无
 ************************************************************************/
void LED_FUN_ON(void)
{
  //smg 

  

}
/************************************************************************
 * Function Name: void wifi_led_fast_blink(void)
 * 功能:
 * 参数:无
 * 返回值:无
 ************************************************************************/



void wifi_led_slowly_blink(void)
{
    static uint8_t breathing_time = 0;
   
    
    breathing_time++;
    if(breathing_time > 25)  // 1.5s // blink 周期1秒 =50 =3s
    {
        breathing_time = 0;
       
        LED_WIFI_TOGGLE();
        
    }
}





























