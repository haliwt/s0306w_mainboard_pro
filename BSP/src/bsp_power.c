/*
 * bsp_power.c
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */
#include "bsp.h"

#define SWITCH_THRESHOLD 2




typedef struct{

  uint8_t process_on_step;
  uint8_t process_off_step;


}POWER_RUN_STATE;

POWER_RUN_STATE gl_run;

typedef enum {
    DISPLAY_TEMP = 0,
    DISPLAY_HUM = 1,
	DISPLAY_TIME = 2
} DisplayMode;

//DisplayMode disp_temp_hum = DISPLAY_TEMP;  // 默认显示温度
uint8_t disp_temp_hum;

uint8_t fan_run_one_minute;


/**********************************************************************
	*
	*Functin Name: void power_on_init_ref(void)
	*Function : 
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
void power_onoff_handler(uint8_t data)
{
  static uint8_t power_on_flag;
   switch(data){	

	   case power_on :

	
		power_on_run_handler();
		link_wifi_to_tencent_handler(g_wifi.wifi_led_fast_blink_flag); //detected ADC of value 
		works_run_two_hours_state();
			
        break;

	  case power_off:
  
         if(power_on_flag==0){
             power_on_flag ++;
			 buzzer_sound();
		 }
         power_off_run_handler();

	   break;
  
      }
	}
/**********************************************************************
	*
	*Functin Name: void power_on_init_ref(void)
	*Function : 
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
void power_on_init_ref(void)
{
	       g_pro.gAI =1;
		   g_pro.gDry =1;
		   g_pro.gPlasma =1;
		   g_pro.gMouse = 1;
		   g_pro.gTemp_value = 40;
		   //display time timing value 
		   g_pro.gdisp_hours_value =0;
		   g_pro.gdisp_timer_hours_value =0; //设置定时时间，

		   g_pro.key_gtime_timer_define_flag=normal_time_mode; //
		   g_pro.g_disp_timer_or_temp_flag = normal_time_mode;
		
		 
		   // function led is turn on 
      
		   //display smg led turn on
		    Fan_Full_Speed();
		    DHT11_Display_Data(0); //display temperature value 
		    DRY_OPEN();
			PLASMA_OPEN();
			mouse_open();
		   
           //timer 
           g_pro.g_manual_shutoff_dry_flag = 0;
		   g_pro.gTimer_disp_time_second= 0;
	       g_pro.gTimer_timer_time_second=0;
		   g_wifi.set_wind_speed_value = 100;


}
/**********************************************************************
	*
	*Functin Name: power_on_run_handler(void)
	*Function : 
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
void power_on_run_handler(void)
{

   static uint8_t send_data_disp_counter,read_error_flag,switch_adc,swich_send_data;

	switch(gl_run.process_on_step){


     case 0:  //initial reference 
       gl_run.process_off_step =0 ; //clear power off process step .
		 
      
       if(g_wifi.gwifi_link_net_state_flag == wifi_no_link || g_wifi.app_timer_power_on_flag == 0){
	      
		   power_on_init_ref();

       }
	   else if(g_wifi.gwifi_link_net_state_flag == wifi_link_success &&  g_wifi.app_timer_power_on_flag == 0){ //has wifi net initial
		  
		   power_on_init_ref();
		
       	
		  MqttData_Publish_SetOpen(1);  
		  osDelay(50);//HAL_Delay(200);
		  Update_DHT11_ToDisplayBoard_Value();
		  osDelay(50);//HAL_Delay(200);
	        
		
		   MqttData_Publish_Update_Data();
		   osDelay(50);//HAL_Delay(200);
	   }
	   else{

		  smartphone_timer_power_handler();

	   }
	  
	   wifi_decoder_refer_init();
	   if(g_disp.g_second_disp_flag == 1){
	   	  SendData_Set_Command(CMD_POWER,open);
		  osDelay(5);
         send_data_disp_counter=200;
		  Update_DHT11_ToDisplayBoard_Value();
		  osDelay(5);
	   
	   }
	   
       
	   g_pro.gTimer_two_hours_counter = 0;
	   g_pro.g_fan_switch_gears_flag++;
	   gl_run.process_off_step=0;
	   
	   g_pro.works_two_hours_interval_flag=0; //WT.EDIT 2025.05.07
	   gl_run.process_on_step =1;
	 break;

	 case 1:


	  if(g_disp.g_second_disp_flag == 1){
          send_data_disp_counter++;
	    if(send_data_disp_counter > 150){ //3s
	        send_data_disp_counter=0;
         Update_DHT11_ToDisplayBoard_Value();

	   }
	  }
	   gl_run.process_on_step =2;


	case 2:

    
	  gl_run.process_on_step =3;

	 break;

	 case 3: //WIFI link process
	 
          if(g_pro.gTimer_switch_temp_hum > 2){

		  g_pro.gTimer_switch_temp_hum =0;
          swich_send_data = swich_send_data ^ 0x01;

		  if(swich_send_data ==1){
			  if(g_wifi.gwifi_link_net_state_flag == wifi_link_success){
			     Update_Dht11_Totencent_Value()  ;
			  }
		  }
		  else{
		 
		   sendData_Real_TimeHum(g_pro.g_humidity_value, g_pro.g_temperature_value);	
		   osDelay(5);

          }

          }

	   gl_run.process_on_step =4;

	 break;

	 case 4: // wifi function
         if(g_pro.gTimer_display_adc_value > 3){
		 	g_pro.gTimer_display_adc_value=0;

		    switch_adc = switch_adc ^ 0x01;
		    if(switch_adc==1){
               Get_PTC_Temperature_Voltage(ADC_CHANNEL_1,5);
			}
			else{
				

				if(g_pro.works_two_hours_interval_flag==0){
	                Get_Fan_Adc_Fun(ADC_CHANNEL_0,5);

		        }

            }

         }
		 
		

		 
		    
	    gl_run.process_on_step =1;

	 break;

	 default :

	  break;

	}
   
 }

/**********************************************************************
	*
	*Functin Name: void power_off_run_handler(void)
	*Function :
	*Input Ref: NO
	*Return Ref: NO
	*
**********************************************************************/
void power_off_run_handler(void)
{

   static uint8_t fan_flag,wifi_first_connect;
   switch(gl_run.process_off_step){

   case 0:
   	  gl_run.process_on_step =0;
   	  g_disp.g_second_disp_flag=1;
 
  
	
	  g_pro.key_gtime_timer_define_flag = normal_time_mode;

	  fan_run_one_minute = 1;
	  g_pro.gTimer_fan_run_one_minute =0;

	  if(g_wifi.gwifi_link_net_state_flag == 1){
            MqttData_Publish_SetOpen(0);  
			osDelay(50);
	        MqttData_Publish_PowerOff_Ref() ;//
	        osDelay(100);
           
	  }
	  if(g_disp.g_second_disp_flag ==1){

	  
	   	  SendData_Set_Command(CMD_POWER,close);
          osDelay(5);
	
         

       }
	   g_pro.g_fan_switch_gears_flag++;
      gl_run.process_off_step = 1;
	   g_pro.works_two_hours_interval_flag=0; //WT.EDIT 2025.05.07
	  

   break;

   case 1:

     if(fan_flag == 0){
	 	fan_flag++;
	  fan_run_one_minute =2;
     }

	 if(fan_run_one_minute ==1){

	     if(g_pro.gTimer_fan_run_one_minute  < 61){

              Fan_Full_Speed();
		 }
		 else{

		    fan_run_one_minute++;
			FAN_Stop();

		 }

	 }

     mainboard_close_all_fun();
	



	 if(g_wifi.gwifi_link_net_state_flag == 1 && wifi_first_connect==0){
	 	    wifi_first_connect++;
            MqttData_Publish_SetOpen(0);  
			osDelay(50);
	        MqttData_Publish_PowerOff_Ref() ;//
	        osDelay(100);
           
	 }

   

   break;

   
   	}

}



