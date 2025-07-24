/*
 * bsp_message.c
 *
 *  Created on: 2025骞�3鏈�4鏃�
 *      Author: Administrator
 */
#include "bsp.h"

static void copy_receive_data(uint8_t cmd,uint8_t data);


uint8_t power_off_test_counter;
uint8_t temperature_value;
uint8_t power_on_counter;

/**********************************************************************
    *
    *Function Name:uint8_t bcc_check(const unsigned char *data, int len) 
    *Function: BCC鏍￠獙鍑芥暟
    *Input Ref:NO
    *Return Ref:NO
    *
**********************************************************************/
uint8_t bcc_check(const unsigned char *data, int len) 
{
    unsigned char bcc = 0;
    for (int i = 0; i < len; i++) {
        bcc ^= data[i];
    }
    return bcc;
}
/**********************************************************************
    *
    *Function Name:void receive_data_from_displayboard(uint8_t *pdata,uint8_t len)
    *Function: receive usart touchkey of command 
    *Input Ref:NO
    *Return Ref:NO
    *
**********************************************************************/
void receive_data_from_displayboard(uint8_t *pdata)
{
  
   if(pdata[1] == 0x02){ //the second display board DEVICE_ID 

    switch(pdata[2]){

     case 0:


     break;

     case 0x01: //琛ㄧず寮€鏈烘寚浠�

        if(pdata[3] == 0x00){ // comand 鍒ゆ柇鏄暟鎹繕鏄懡浠�

		
          if(pdata[4] == 0x01){ 
		  g_disp.g_second_disp_flag = 1;
		  g_pro.gpower_on = power_on;
		  power_on_counter++;
          buzzer_sound();
		 
		  //g_pro.g_copy_power_onoff_flag = power_on;
          SendWifiData_Answer_Cmd(CMD_POWER,0x01); //WT.EDIT 2025.01.07 
           osDelay(5);
        }
        else{ //close 
         
          buzzer_sound();
		  g_disp.g_second_disp_flag = 1;
		  g_pro.gpower_on = power_off;
          power_off_test_counter++;
		 // g_pro.g_copy_power_onoff_flag = 0x02;
		 SendWifiData_Answer_Cmd(CMD_POWER,0x0); //WT.EDIT 2025.01.07
		 osDelay(5);

        }

    	}

     break;

     case 0x02: //PTC鎵撳紑鍏抽棴鎸囦护

     if(pdata[3] == 0x00){ //鍒ゆ柇鏄惁鏄暟鎹紝鎴栬€呮寚浠ら€氱煡锛� 00- 鍛戒护鍜屾寚浠わ紝涓嬩竴涓瓧鑺傛槸鎸囦护 锛�0x0F- 鏁版嵁锛屼笅涓€涓瓧鑺傛槸鏁版嵁涓暟
	 	if(pdata[4]==0x01){
	 	if(g_pro.gpower_on == power_on){
		 
          buzzer_sound();
          g_pro.gDry = 1;
		  
		  //manual close flag :
		   SendWifiData_Answer_Cmd(CMD_PTC,0x01); //WT.EDIT 2025.01.07
		   osDelay(5);
		  g_pro.g_manual_shutoff_dry_flag = 0;
		  if(g_pro.works_two_hours_interval_flag==0){
		      DRY_OPEN();
		  }

	 	 
         if(g_wifi.gwifi_link_net_state_flag==1){
              MqttData_Publish_SetPtc(0x01);
	  	      osDelay(50);//HAL_Delay(350);
          }
	 		}
	 	}
         else if(pdata[4] == 0x0 && g_pro.gpower_on == power_on){
	   	 if(g_pro.gpower_on == power_on){
		  g_pro.g_manual_shutoff_dry_flag = 1;
          buzzer_sound();
          g_pro.gDry =0;
		
          DRY_CLOSE();
		  if(g_disp.g_second_disp_flag ==1){
		  SendWifiData_Answer_Cmd(CMD_PTC,0x0); //WT.EDIT 2025.01.07
		  //manual close flag :
		  }
            
         if(g_wifi.gwifi_link_net_state_flag==1){
              MqttData_Publish_SetPtc(0x0);
	  	      osDelay(50);//HAL_Delay(350);
          }
	   	 }
       }
     }
     break;

     case 0x03: //PLASMA 鎵撳紑鍏抽棴鎸囦护

       if(pdata[3] == 0x00){
	   if(pdata[4]==0x01 && g_pro.gpower_on == power_on){
	     
		   	  
	            buzzer_sound();
				g_pro.gPlasma = 1;
			    
			    if(g_pro.works_two_hours_interval_flag==0){
	                PLASMA_OPEN();
			   }
			if(g_wifi.gwifi_link_net_state_flag==1){
	           MqttData_Publish_SetPlasma(1);
			       osDelay(50);//HAL_Delay(350);
	          }
	       	
	      }
	      else if(pdata[4] == 0x0 && g_pro.gpower_on == power_on){
	          
	            buzzer_sound();
				g_pro.gPlasma = 0;
			   
			     PLASMA_CLOSE();
				
				
			   
			    if(g_wifi.gwifi_link_net_state_flag==1){
	              MqttData_Publish_SetPlasma(0);
			       osDelay(50);//HAL_Delay(350);
	            }
	           
	        
	        }
       		
	   	
       	}
      break;


      case 0x04: //ultrasonic  鎵撳紑鍏抽棴鎸囦护

       if(pdata[3] == 0x00){ // 00-銆嬭〃绀烘槸鎸囦护鎴栬€呴€氱煡锛屼笉鏄暟鎹紝涓嬩竴涓暟鎹氨鏄懡浠ゆ垨鑰呴€氱煡
	 	if(pdata[4]==0x01){
          if(g_pro.gpower_on == power_on){ 
            buzzer_sound();
			g_pro.gMouse = 1;
		   
		    if(g_pro.works_two_hours_interval_flag==0){
                mouse_open();
		   }
		if(g_wifi.gwifi_link_net_state_flag==1){
           MqttData_Publish_SetUltrasonic(1);
		       osDelay(50);//HAL_Delay(350);
          }
           
          }
        }
        else if(pdata[4] == 0x0 && g_pro.gpower_on == power_on){ //close 
		 if(g_pro.gpower_on == power_on){ 
					buzzer_sound();
					g_pro.gMouse = 0;
				
					mouse_close();
          if(g_wifi.gwifi_link_net_state_flag==1){
				MqttData_Publish_SetUltrasonic(0);
			    osDelay(50);//HAL_Delay(350);
			}

        }
       }
    
       	}
     break;

      case 0x05: // link wifi command

       if(pdata[3] == 0x00){
	 	if(pdata[4]==0x01){
        if(g_pro.gpower_on == power_on){ 
		  SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
		  osDelay(5);
          buzzer_sound();
		  
          g_wifi.gTimer_wifi_led_fast_counter=0;
	      g_wifi.link_net_step=0;
	      g_wifi.gwifi_link_net_state_flag=0 ; //clear wifi link net flag .repeat be detected wifi state.
		  g_wifi.wifi_led_fast_blink_flag=1;   // led blink flag .
        
         }
        
       	}
       	}

     break;

     case 0x06: //buzzer sound command 

        if(pdata[3] == 0x00){  //buzzer sound 
          if(pdata[4]==0x01){
           buzzer_sound();
        }
        else if(pdata[4] == 0x0){ // don't buzzer sound .
        
        }
    	}

     break;


	 case 0x10: //has two display board.

	   if(pdata[3] == 0x01){ 
           g_disp.g_second_disp_flag = 1; 

	    }
		else{
		   g_disp.g_second_disp_flag = 0; 


		}
     break;

     case 0x16 : //buzzer sound command with answer .

      
       if(pdata[3] == 0x01){  //buzzer sound 

          if(g_pro.gpower_on == power_on){  
          SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.01.07
          buzzer_sound();

          }
            
       }
        else if(pdata[3] == 0x0){ // don't buzzer sound .
 
 
 
        }


     break;


      case 0x1A: //read 娓╁害鏁版嵁

       if(pdata[3] == 0x0F){ //鏁版嵁
       
	     

          

	   }
      break;

      case 0x1B: //婀垮害鏁版嵁

        if(pdata[3] == 0x0F){ //鏁版嵁
            

        }
      break;

      case 0x1C: //琛ㄧず鏃堕棿锛氬皬鏃讹紝鍒嗭紝绉�

        if(pdata[3] == 0x0F){ //鏁版嵁



        }
      break;

        case 0x1D: //琛ㄧず鏃ユ湡锛� 骞达紝鏈堬紝鏃�

        if(pdata[3] == 0x0F){ //鏁版嵁

             
            

        }
      break;

	  
	 case 0x2A: //display board set up tempeature value send data to mainboard
	 
			if(pdata[3] == 0x0F){ //鏁版嵁

			   if(pdata[4]==0x01){ // has dat only one value ,next receive byte is value
	         
			   if(g_pro.gpower_on == power_on){ 
				
              
        		g_pro.g_manual_shutoff_dry_flag=0;
			    g_pro.set_temp_value_success =1; //WT.EDIT 2025.05.09
				
			  
			   
				g_pro.gset_temperture_value = pdata[5];
				g_wifi.wifi_set_temperature_value = pdata[5];

                 if(g_wifi.gwifi_link_net_state_flag==1){
					 MqttData_Publis_SetTemp(g_wifi.wifi_set_temperature_value);
			         osDelay(100);//HAL_Delay(350);
                 }
				
               
				
             }

			 }
				
	 
			}
	 break;

	  

     case 0x22: //PTC鎵撳紑鍏抽棴鎸囦护,buzzer don't sound,娓╁害瀵规瘮鍚庣殑鎸囦护

	  if(pdata[3]==0){ //琛ㄧず鏄寚浠�

      if(pdata[4] == 0x01){
        
        if(g_pro.gpower_on == power_on){

		//g_pro.g_manual_shutoff_dry_flag=0;
        g_pro.gDry = 1;
		
     	if(g_pro.works_two_hours_interval_flag==0){
		  	DRY_OPEN();
     	 }

	 	 
         if(g_wifi.gwifi_link_net_state_flag==1){
              MqttData_Publish_SetPtc(0x01);
	  	      osDelay(20);//HAL_Delay(350);
          }
       
       
				}
	  }
      else if(pdata[4] == 0x0 && g_pro.gpower_on == power_on){
        

		   //g_pro.g_manual_shutoff_dry_flag=0;
         
            g_pro.gDry =0;
		    
          	DRY_CLOSE();
		  
		
            
         if(g_wifi.gwifi_link_net_state_flag==1){
              MqttData_Publish_SetPtc(0x0);
	  	      osDelay(50);//HAL_Delay(350);
          }
	   	 
       
      
		}
	  	}
     break;

     case 0x27: //smart phone set AI mode

      if(pdata[3] == 0x02){ //don't AI
       
        
          
       }
       else if(pdata[3] == 0x01){ //AI mode 
       
      
       }


     break;

	 case 0x4C: //set up timer timing value 
		if(pdata[3] == 0x0F){ //鏁版嵁

			if(pdata[4]==0x01){ // has dat only one value ,next receive byte is value

			if(g_pro.gpower_on == power_on){ 
                buzzer_sound();
			
				g_pro.key_gtime_timer_define_flag = input_set_timer_mode;
				g_pro.key_set_timer_flag =1;
				
			  
			    g_pro.gdisp_timer_hours_value = pdata[5];
			
			    g_pro.g_disp_timer_or_temp_flag = input_set_timer_mode;//WT.EDIT 2025.04.23//input_temp_time_mode  ;
			   
				

			}

			}


		}
	 	
     break;

     case 0xFF: //copy send cmd acknowlege

	 copy_receive_data(pdata[3],pdata[4]);

	       
     break;

	 default:
	 break;
        
     
     }

   }
 
}

/********************************************************************
	*
	*Function Name: static void copy_receive_data(uint8_t cmd,uint8_t data)
	*Function: mainboard of key be pressed that the second display board receive
	*          command and repeat mainboard order and  send to mainboard,
	*		   then mainboard run command .
	*Input Ref:NO
	*R
	*
*********************************************************************/
static void copy_receive_data(uint8_t cmd,uint8_t data)
{
     switch(cmd){

       case CMD_POWER:

	        if(data == 1){
                buzzer_sound();
			    g_pro.gpower_on = power_on;

			}
			else{
               buzzer_sound();
			   g_pro.gpower_on = power_off;


			}
	 

	   break;
	  

	   case CMD_PTC :
	   	if(data == 1){

		   buzzer_sound();
		   g_pro.gDry=1;
		   
		   if(g_pro.works_two_hours_interval_flag ==0){

               DRY_OPEN();
              }

		  }
		  else{
			  g_pro.gDry=0;

              DRY_CLOSE();
            

		 }

	   break;

	   case CMD_CONNECT_WIFI:
	   	if(data == 1){
			//buzzer_sound();
         
			
			//g_wifi.gwifi_link_net_state_flag=0 ; //clear wifi link net flag .repeat be detected wifi state.
			//g_wifi.wifi_led_fast_blink_flag=1;   // led blink flag .

		}
		else{



		}

	   break;


	 }


}


