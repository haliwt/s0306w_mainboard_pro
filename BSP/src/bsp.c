/*
 * bsp.c
 *
 *  Created on: 2025年3月4日
 *      Author: Administrator
 */
#include "bsp.h"


process_t g_pro;

 uint16_t mainboard_time;

 static void mainboard_special_fun(void);

/******************************************************************************
	*
	*Function Name:void bsp_init(void)
	*Funcion: 
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
void bsp_init(void)
{
	 delay_init(64); 
	 dht11_init();
	 
	 g_pro.gset_temperture_value=40; //WT.EDIT 2025.05.05
	 g_pro.gDry = 1;
	 

}

/******************************************************************************
	*
	*Function Name:void mainboard_fun_handler(void)
	*Funcion: dry,plasma,mouse,fan ADC
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
void mainboard_fun_handler(void)
{
   //static uint16_t mainboard_time;

   mainboard_time ++ ;

   if(mainboard_time > 100){// 2s  //300 ~= 6s, 50 ~=1s
      mainboard_time=0;


   if(g_pro.gAI == 1){
  
      
   	
   	}
    else{
      

	}

	if(g_pro.gDry == 1 && read_wifi_dry_value()==0){
		DRY_OPEN();
		
	}
	else{
	
		DRY_CLOSE();

	}

	if(g_pro.gMouse == 1){
	
		mouse_open();
	}
	else{
	
		mouse_close();
	}

	if(g_pro.gPlasma == 1){
		
		PLASMA_OPEN();
	}
	else{
		
		PLASMA_CLOSE();
	}

	Fan_RunSpeed_Fun();
	
   	}
	

}

static void mainboard_special_fun(void)
{
   //static uint16_t mainboard_time;

  

	if(g_pro.gDry == 1){
		DRY_OPEN();
		
	}
	else{
		DRY_CLOSE();
	

	}

	if(g_pro.gMouse == 1){
		mouse_open();
		
	}
	else{
		mouse_close();
		
	}

	if(g_pro.gPlasma == 1){
		PLASMA_OPEN();
	
	}
	else{
		PLASMA_CLOSE();
		
	}
	
   Fan_RunSpeed_Fun();
	

}

void mainboard_close_all_fun(void)
{
	DRY_CLOSE();
	mouse_close();

	PLASMA_CLOSE();


}


/**********************************************************************
    *
    *Functin Name: void works_run_two_hours_state(void)
    *Function :  
    *Input Ref: NO
    *Return Ref: NO
    *
************************************************************************/
void works_run_two_hours_state(void)
{
   static uint8_t timer_fan_flag;

   #if TEST_UNIT
	if(g_pro.gTimer_two_hours_counter > 300 && g_pro.works_two_hours_interval_flag==0){ //five minutes 5x60=300s


   #else 
    if(g_pro.gTimer_two_hours_counter > 7200 ){ //two hours

   #endif 
	g_pro.gTimer_two_hours_counter= 0;

    g_pro.works_two_hours_interval_flag=1;

	PLASMA_CLOSE(); //
	DRY_CLOSE();
	mouse_close();
    g_pro.gTimer_fan_run_one_minute=0;


	timer_fan_flag=1;

   
   }

   switch(g_pro.works_two_hours_interval_flag){

    case 1:

   

     #if TEST_UNIT 
	 if(g_pro.gTimer_two_hours_counter  > 600){ //2minutes x 60s = 120s
           
         g_pro.gTimer_two_hours_counter =0;  
		 
         g_pro.works_two_hours_interval_flag=0;
    
     

         mainboard_special_fun();
            
      }
     #else 

      if(g_pro.gTimer_two_hours_counter  > 600){ //10*60s=600s
         g_pro.gTimer_two_hours_counter =0;  
		 
         g_pro.works_two_hours_interval_flag=0;
    
     

         mainboard_special_fun();
                
      }


      #endif 

	 if(timer_fan_flag ==1){

	      if(g_pro.gTimer_fan_run_one_minute < 60){
	  
	              Fan_Full_Speed();
				  PLASMA_CLOSE(); //
				  DRY_CLOSE();
	              mouse_close();
	        } 
			else if(g_pro.gTimer_fan_run_one_minute > 59){
	           
			   g_pro.gTimer_fan_run_one_minute=0;
			
			  timer_fan_flag=0;
             
			   FAN_Stop();
	       }

	  }

   
    break;

    case 0:

	    mainboard_fun_handler();
        
    break;
   }

}




void copy_cmd_hanlder(void)
{
    if(g_pro.g_copy_power_onoff_flag == power_on){
		
	   
	    SendWifiData_Answer_Cmd(CMD_POWER,0x01); //WT.EDIT 2025.01.07 
			osDelay(5);
      g_pro.g_copy_power_onoff_flag =0xff;

	}
	else if( g_pro.g_copy_power_onoff_flag ==0x02){

	  g_pro.g_copy_power_onoff_flag =0xfe;
	  SendWifiData_Answer_Cmd(CMD_POWER,0x0); //WT.EDIT 2025.01.07 
        osDelay(5);

	}

}

void temperature_compare_value_handler(void)
{
  static uint8_t first_one_flag;

    if(g_pro.gTimer_read_dht11_data > 3){
		g_pro.gTimer_read_dht11_data= 0;
    Update_DHT11_ToDisplayBoard_Value();

    if(g_pro.set_temp_value_success ==1){
		
	    
      if(g_pro.set_up_temperature_value > g_pro.g_temperature_value){ //PTC TURN ON

      if(g_pro.g_manual_shutoff_dry_flag == 0){ //allow open dry function 
 
        g_pro.gDry = 1;
       
	    DRY_OPEN();
        SendData_Set_Command(0x02,0x01);//SendData_Set_Command(DRY_ON_NO_BUZZER);
		osDelay(5);
		
        
     }
     }
     else{ //PTC turn off 
         g_pro.gDry =0;
         DRY_CLOSE();
     
    	 SendData_Set_Command(0x02,0x0);//SendData_Set_Command(DRY_OFF_NO_BUZZER);
    	  osDelay(5);
    
        }

   }
   else{
        if(g_pro.g_temperature_value >39){

         g_pro.gDry =0;
         DRY_CLOSE();
      
    	 SendData_Set_Command(0x01,0x0);//SendData_Set_Command(DRY_OFF_NO_BUZZER);
    	 osDelay(5);
        
         first_one_flag =1;
        }
        else{

           if(first_one_flag==1 && (g_pro.g_temperature_value <38) && g_pro.g_manual_shutoff_dry_flag == 0){

              
                 g_pro.gDry =1;
            
                 DRY_OPEN();
		         SendData_Set_Command(0X02,0x01);
				 osDelay(5);

              
            }
		    else{

			 if(first_one_flag==0 && (g_pro.g_temperature_value <39) && g_pro.g_manual_shutoff_dry_flag == 0){
			                g_pro.gDry =1;
						
							 DRY_OPEN();
							 SendData_Set_Command(0X02,0x01);
							 osDelay(5);



			 }


			}
           }

      }
    	}
}


