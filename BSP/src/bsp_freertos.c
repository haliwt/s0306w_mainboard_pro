#include "bsp.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#define POWER_BIT_0	        (1 << 0)
#define MODE_BIT_1	        (1 << 1)
#define DOWN_BIT_2           (1 << 2)
#define UP_BIT_3           (1 << 3)

#define KEY_LONG_POWER_BIT_4      (1<<4)
#define KEY_LONG_MODE_BIT_5			(1<<5)


#define MODE_LONG_KEY_8         (1 << 8)
#define DECODER_BIT_9          (1<< 9)


uint8_t inputBuf[1];


/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
static void vTaskRunPro(void *pvParameters);
//static void vTaskDecoderPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);



/* 创建任务通信机制 */
//static void AppObjCreate(void);


/***********************************************************************************************************
											变量声明
***********************************************************************************************************/
static TaskHandle_t xHandleTaskRunPro = NULL;
//static TaskHandle_t xHandleTaskDecoderPro= NULL;
static TaskHandle_t xHandleTaskStart = NULL;





typedef struct Msg
{

	uint8_t  usData[12];
    uint8_t  ucMessageID;
    uint8_t  rx_data_counter;
    uint8_t  disp_rx_cmd_done_flag;
    uint8_t  bcc_check_code;
    volatile uint8_t ulid;

}MSG_T;

MSG_T   gl_tMsg; /* 定义丢�个结构体用于消息队列 */
uint8_t ucKeyCode;
uint8_t uckey_number;
uint8_t key_power_flag,decoder_flag ;
uint8_t check_code;

uint8_t power_on_key_counter, mode_key_counter;


void freeRTOS_Handler(void)
{
    /* 创建任务 */
	  AppTaskCreate();

	  /* 创建任务通信机制 */
	//  AppObjCreate();

	  /* 启动调度，开始执行任�?1�?7 */
	   vTaskStartScheduler();
}
/**********************************************************************************************************
*	Function Name:static void vTaskStart(void *pvParameters)
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传��的形参
*	Return Ref:
*   priority: 1  (数�?�越小优先级越低，这个跟uCOS相反)
**********************************************************************************************************/
#if 1
static void vTaskRunPro(void *pvParameters)
{
    while(1){

    power_onoff_handler(g_pro.gpower_on);
	
	if(g_wifi.wifi_led_fast_blink_flag ==0 ){
		wifi_communication_tnecent_handler();//
		getBeijingTime_cofirmLinkNetState_handler();
		wifi_auto_detected_link_state();
	}
	
	copy_cmd_hanlder();

     osDelay(20);//30



	}

}
#endif 
/**********************************************************************************************************
*	Function Name:static void vTaskStart(void *pvParameters)
*	Function:
*	Input Ref: pvParameters 是在创建该任务时传��的形参
*	Return Ref:
*   priority: 1  (数�?�越小优先级越低，这个跟uCOS相反)
**********************************************************************************************************/
static void vTaskStart(void *pvParameters)
{
    BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5000); /* 设置�?大等待时间为30ms */
	uint32_t ulValue;


    while(1)
    {

	xResult = xTaskNotifyWait(0x00000000,
								  0xFFFFFFFF,     /* Reset the notification value to 0 on */
								&ulValue,        /* 保存ulNotifiedValue到变量ulValue�? */
								xMaxBlockTime);//portMAX_DELAY);  /* 阻塞时间30ms，释放CUP控制�?,给其它任务执行的权限*/

	if( xResult == pdPASS )
	{
			/* 接收到消息，�?测那个位被按�? */

		if((ulValue & DECODER_BIT_9) != 0){
  
			    gl_tMsg.disp_rx_cmd_done_flag = 0;
				check_code =  bcc_check(gl_tMsg.usData,gl_tMsg.ulid);

				if(check_code == gl_tMsg.bcc_check_code ){

				 receive_data_from_displayboard(gl_tMsg.usData);
				// memset(gl_tMsg.usData, 0, 4); // 初始化为 0
				 
			}
				
		 }
	 }

	 
	 }
}

/**********************************************************************************************************
*	Function Name: AppTaskCreate
*	功能说明: 创建应用任务
*	Input Ref:
*	Return Ref:
**********************************************************************************************************/
void AppTaskCreate (void)
{

      xTaskCreate( vTaskRunPro,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务�?1�?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 1,              		/* 任务优先�?1�?7 数��越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskRunPro );   /* 任务句柄  */

	  xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务�?1�?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 2,              		/* 任务优先�?1�?7 数��越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskStart );   /* 任务句柄  */
}




/********************************************************************************
	**
	*Function Name:void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	*Function :UART callback function  for UART interrupt for receive data
	*Input Ref: structure UART_HandleTypeDef pointer
	*Return Ref:NO
	*
*******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
     static uint8_t state,rx_end_flag ;
     BaseType_t xHigherPriorityTaskWoken = pdFALSE;


    if(huart->Instance == USART1) // mainBoard receive data from display board send data USART1
	{

     //  DISABLE_INT();
       switch(state)
		{
		case 0:  //#0
			if(inputBuf[0] == 0xA5){  // 0xA5 -- second display board ID
               gl_tMsg.rx_data_counter=0;
				         gl_tMsg.ulid=0;
               gl_tMsg.usData[gl_tMsg.rx_data_counter] = inputBuf[0];
				state=1; //=1

             }
            else
                state=0;
		break;


		case 1: //#1

            if(gl_tMsg.disp_rx_cmd_done_flag ==0){
              /* 初始化结构体指针 */
               gl_tMsg.rx_data_counter++;

	          gl_tMsg.usData[gl_tMsg.rx_data_counter] = inputBuf[0];


              if(rx_end_flag == 1){

                state = 0;

                gl_tMsg.ulid = gl_tMsg.rx_data_counter;
                rx_end_flag=0;

                gl_tMsg.rx_data_counter =0;

                gl_tMsg.disp_rx_cmd_done_flag = 1 ;

                gl_tMsg.bcc_check_code=inputBuf[0];

                #if 1
                xTaskNotifyFromISR(xHandleTaskStart,  /* 目标任务 */
                                    DECODER_BIT_9,     /* 设置目标任务事件标志位bit0  */
                                    eSetBits,  /* 将目标任务的事件标志位与BIT_0进行或操作， 将结果赋值给事件标志�? */
                                    &xHigherPriorityTaskWoken);

                /* 如果xHigherPriorityTaskWoken = pdTRUE，那么�??出中断后切到当前�?高优先级任务执行 */
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
				#endif 

              }

              }

              if(gl_tMsg.usData[gl_tMsg.rx_data_counter] ==0xFE && rx_end_flag == 0 &&   gl_tMsg.rx_data_counter > 4){

                     rx_end_flag = 1 ;

              }

        break;



		}

        //  ENABLE_INT();
    __HAL_UART_CLEAR_OREFLAG(&huart1);
	HAL_UART_Receive_IT(&huart1,inputBuf,1);//UART receive data interrupt 1 byte

   }
   else if(huart->Instance==USART2) //WIFI USART2
	   {
	  //  DISABLE_INT();
		if(g_wifi.linking_tencent_cloud_doing ==1){
	
			   g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter] =wifi_rx_inputBuf[0];
			   g_wifi.wifi_rx_data_counter++;
	
			   if(*wifi_rx_inputBuf==0x0A) // 0x0A = "\n"
			   {
				   
				   Wifi_Rx_InputInfo_Handler();
				   g_wifi.wifi_rx_data_counter=0;
			   }
	
		} 
		else{
	
			   if(g_wifi.get_rx_beijing_time_enable==1){
					  g_wifi.wifi_rx_data_array[g_wifi.wifi_rx_data_counter] = wifi_rx_inputBuf[0];
					  g_wifi.wifi_rx_data_counter++;
					   
			   }
			   else
				   Subscribe_Rx_Interrupt_Handler();
		}
		//	ENABLE_INT();
		 __HAL_UART_CLEAR_OREFLAG(&huart2);
		 HAL_UART_Receive_IT(&huart2,wifi_rx_inputBuf,1);
	}
}

