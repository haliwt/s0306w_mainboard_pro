/*
 * bsp_subsribe.c
 *
 *  Created on: 2025Âπ?3Êú?10Êó?
 *      Author: Administrator
 */
#include "bsp.h"

void smartphone_timer_power_handler(void)
{

         g_pro.gAI =1;

          Parse_Json_phone_timer_power_on_ref();

		   SendWifidata_Three_Data(0x20,g_pro.gDry,g_pro.gPlasma,g_pro.gMouse);
		   osDelay(10);
           if(g_pro.gDry==0){
			   MqttData_Publish_SetPtc(0);
			   osDelay(50);//HAL_Delay(350);
           	}
		    else{
			   MqttData_Publish_SetPtc(1);
			   osDelay(50);//HAL_Delay(350);


			}

			 if(g_pro.gPlasma==0){
			   MqttData_Publish_SetPlasma(0);
			   osDelay(50);//HAL_Delay(350);
           	}
		    else{
			   MqttData_Publish_SetPlasma(1);
			   osDelay(50);//HAL_Delay(350);


			}

			
		  
	       
		   //property_report_phone_timer_on_data();// MqttData_Publish_Update_Data();
		   //osDelay(100);//HAL_Delay(200);
           g_pro.g_manual_shutoff_dry_flag = 0;
		   g_pro.gTimer_disp_time_second= 0;
	       g_pro.gTimer_timer_time_second=0;
		   g_wifi.set_wind_speed_value = 100;

			
	  
}
			


void SetPowerOff_ForDoing(void)
{
   
     g_wifi.set_wind_speed_value =10;
 
   
    g_pro.gDry=0;//gctl_t.gDry = 0;
  
	g_pro.gPlasma=0;//gctl_t.gPlasma =0;       //"Êù?Ëè?"
	g_pro.gMouse=0;//gctl_t.gUlransonic = 0; // "È©±Ëô´"
	g_pro.gAI =1;


    
	PLASMA_CLOSE();//PLASMA_SetLow(); //
	
	DRY_CLOSE();//PTC_SetLow();
	mouse_close();





}



