/*
 * bsp_cmd_link.c
 *
 *  Created on: 2025幄1�73朄1�74旄1�7
 *      Author: Administrator
 */
#include "bsp.h"

#define MAX_BUFFER_SIZE  12

#define USART1_IT_FLAG   0

#define FRAME_HEADER 0x5A
#define FRAME_END 0xFE


//uint8_t  inputBuf[4];
uint8_t  inputCmd[30];
uint8_t  wifiInputBuf[1];


uint8_t rx_wifi_data[7];

uint8_t txComplete;

uint8_t rxBuffer[RX_DATA_SIZE];  // DMA接收缓冲匄1�7


static uint8_t transferSize;
uint8_t outputBuf[MAX_BUFFER_SIZE];
volatile uint8_t dataReceived = 0;  // 接收完成标志


// 公共函数：填充帧数据
void FillFrame(uint8_t *buf, uint8_t cmd, uint8_t *data, uint8_t dataLen) 
{
    buf[0] = FRAME_HEADER;
    buf[1] = 0x10; // Mainboard device number
    buf[2] = cmd;
    buf[3] = (dataLen > 0) ? 0x0F : 0x00; // Data or command

	if(buf[3] ==0){
       buf[4] = data[0];
       buf[5] = FRAME_END;
	   buf[6] = bcc_check(buf, 6 );
	}
	else{
	      buf[4] = dataLen;
	
		   for (uint8_t i = 0; i < dataLen; i++) {
			   buf[5 + i] = data[i];
		   }
	
		   buf[5 + dataLen] = FRAME_END;
		   buf[6 + dataLen] = bcc_check(buf, 6 + dataLen);


	}
}

void FillFrame_Response(uint8_t *buf, uint8_t cmd, uint8_t *data, uint8_t dataLen) 
{
    buf[0] = FRAME_HEADER;          // 帧头
    buf[1] = 0x10;                  // 主板设备叄1�7
    buf[2] = 0xFF;                  // 应答信号标志
    buf[3] = cmd;                   // 命令类型
    buf[4] = (dataLen > 0) ? 0x0F : 0x00; // 数据标志＄1�70x0F 表示有数据，0x00 表示无数捄1�7

    if (buf[4] == 0x00) {           // 无数据的情况
        buf[5] = data[0];           // 具体指令
        buf[6] = FRAME_END;         // 帧尾
        buf[7] = bcc_check(buf, 7); // 校验砄1�7
    } else {                        // 有数据的情况
        buf[5] = dataLen;           // 数据长度
        if (data != NULL) {         // 棢�查数据指针是否有敄1�7
            for (uint8_t i = 0; i < dataLen; i++) {
                buf[6 + i] = data[i]; // 填充数据
            }
        }
        buf[6 + dataLen] = FRAME_END;         // 帧尾
        buf[7 + dataLen] = bcc_check(buf, 7 + dataLen); // 校验砄1�7
    }
}



// 公共函数：发送数捄1�7
void TransmitData(uint8_t *buf, uint8_t size) 
{
    transferSize = size;

    #if USART1_IT_FLAG
    if (transferSize) {
        while (transOngoingFlag); // 等待传输完成
        transOngoingFlag = 1;
        HAL_UART_Transmit_IT(&huart1, buf, transferSize);
    }
    #else
    HAL_UART_Transmit_DMA(&huart1, buf, transferSize);
    #endif
}

// 发��实时温湿度数据
void sendData_Real_TimeHum(uint8_t hum, uint8_t temp) 
{
    uint8_t data[2] = {hum, temp};
    FillFrame(outputBuf, 0x1A, data, 2);
    TransmitData(outputBuf, 9);
}

// 发��时间数捄1�7
void SendWifiData_To_PanelTime(uint8_t hours, uint8_t minutes, uint8_t seconds) 
{
    uint8_t data[3] = {hours, minutes, seconds};
    FillFrame(outputBuf, 0x1C, data, 3);
    TransmitData(outputBuf, 10);
}

// 发��命令数捄1�7
void SendData_Set_Command(uint8_t cmd, uint8_t data) 
{
    uint8_t cmdData[1] = {data};
    FillFrame(outputBuf, cmd, cmdData, 0);
    TransmitData(outputBuf, 7);
}

// 发��风速数捄1�7
void SendWifiData_To_PanelWindSpeed(uint8_t speed) {
    uint8_t data[1] = {speed};
    FillFrame(outputBuf, 0x1E, data, 1);
    TransmitData(outputBuf, 8);
}

// 发��命令响庄1�7
void SendWifiData_Answer_Cmd(uint8_t cmd, uint8_t cmdata) 
{
    uint8_t cmdData[1] = {cmdata};
    FillFrame_Response(outputBuf, cmd,cmdData ,0);
    TransmitData(outputBuf,8);
}
// smart phone send command
void SendWifiData_To_Cmd(uint8_t cmd,uint8_t data)
{
	 uint8_t cmdData[1] = {data};
    FillFrame(outputBuf, cmd, cmdData, 0);
    TransmitData(outputBuf, 7);

}


/***********************************************************************
	*
	*Function Name:void SendWifiData_One_Data(uint8_t cmd,uint8_t data)
	*Function:
	*Input Ref: 1-cmd   2. data.
	*Return Ref:NO
	*
************************************************************************/
void SendWifiData_One_Data(uint8_t cmd,uint8_t data)
{
      #if 0
		outputBuf[0]=0x5A; //head : main board 0x5A
        outputBuf[1]=0x10; //main board device No: 0x10
        outputBuf[2]=cmd; //command type: fan speed of value 
        outputBuf[3]=0x0F; // 0x0F : is data ,don't command order.
        outputBuf[4]= 0x01; // don't data ,onlay is command order,recieve data is 1byte .
        outputBuf[5]= data; // don't data 
        
        outputBuf[6] = 0xFE;
        outputBuf[7] = bcc_check(outputBuf,7);
		
        #if USART1_IT_FLAG 
        transferSize=8;
        if(transferSize)
        {
            while(transOngoingFlag); //UART interrupt transmit flag ,disable one more send data.
            transOngoingFlag=1;
            HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
        }
		#else 
		
	      HAL_UART_Transmit_DMA(&huart1,outputBuf,transferSize);


		#endif 
	#endif 

	
	uint8_t cmdata[1] = {data};
	FillFrame(outputBuf, cmd, cmdata, 1);
	TransmitData(outputBuf, 8);

}

/***********************************************************************
	*
	*Function Name:void SendWifiData_One_Data(uint8_t cmd,uint8_t data)
	*Function:
	*Input Ref: 1-cmd   2. data.
	*Return Ref:NO
	*
************************************************************************/
void SendWifidata_Two_Data(uint8_t cmd,uint8_t data1,uint8_t data2)
{
   
    uint8_t cmdata[2] = {data1,data2};
	FillFrame(outputBuf, cmd, cmdata, 2);
	TransmitData(outputBuf, 9);

}

void SendWifidata_Three_Data(uint8_t cmd,uint8_t data1,uint8_t data2,uint8_t data3)
{
   
    uint8_t cmdata[3] = {data1,data2,data3};
	FillFrame(outputBuf, cmd, cmdata, 3);
	TransmitData(outputBuf, 10);

}


#endif 

//void EUSART_SetTxInterruptHandler(void (* interruptHandler)(void))
//{
//    EUSART_TxDefaultInterruptHandler = interruptHandler;
//}

/********************************************************************************
	**
	*Function Name:
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
void Start_DMA_Receive(void) 
{
    // 清空缓冲匄1�7
    memset(rxBuffer, 0, MAX_BUFFER_SIZE);
    dataReceived = 0;
    
    // 启动DMA接收
    HAL_UART_Receive_DMA(&huart1, rxBuffer,sizeof(rxBuffer));
}

/********************************************************************************
	**
	*Function Name:
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
        #if USART1_IT_FLAG 
		transOngoingFlag=0; //UART Transmit interrupt flag =0 ,RUN
		#else

		txComplete = 1;//uartTxComplete = 1; // 标记发��完戄1�7

		#endif 
	}

//	if(huart== &huart2){
//
//       usart2_transOngoingFlag =0;
//
//	}

}

/**
  * @brief  UART错误回调函数，处理USART1通信错误
  * @param  huart: UART句柄指针
  */

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) 
{
    

	if (huart->Instance == USART1) {
        // 重新初始化或报警
        #if 0
          __HAL_UART_CLEAR_OREFLAG(&huart1);
          __HAL_UART_CLEAR_NEFLAG(&huart1);
          __HAL_UART_CLEAR_FEFLAG(&huart1);
           
          
          temp=USART1->ISR;
          temp = USART1->RDR;
		  
     
		  UART_Start_Receive_IT(&huart1,inputBuf,1);
		 #endif 
	    /* 1. 清除扢�有可能出现的错误标志 */
	    // 使用单条语句清除多个标志（更高效＄1�7
	    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_FEF);

	    /* 2. 读取状��和数据寄存器（清空残留数据＄1�7*/
	    // 使用UNUSED宏避免编译器警告（如果不霢�要实际��）
	    //UNUSED(uint32_t temp_isr = huart->Instance->ISR);  // 读取ISR会清除部分标忄1�7
	    //UNUSED(uint32_t temp_rdr = huart->Instance->RDR);  // 清空接收寄存噄1�7
	      /* 2. 清空寄存器（箢�洁写法）*/
		    (void)huart->Instance->ISR;  // 清除状��标忄1�7
		    (void)huart->Instance->RDR;  // 清空接收数据

	    /* 3. 重启接收（带错误棢�查）*/
	    if (HAL_UART_GetState(huart) == HAL_UART_STATE_READY) {
	         Start_DMA_Receive(); // HAL_UART_Receive_IT(huart, inputBuf, 1);  // 重新启动单字节中断接攄1�7
	    } else {
	        // 可��：硬件复位USART（严重错误时＄1�7
	        __HAL_UART_DISABLE(huart);
	        __HAL_UART_ENABLE(huart);
	        HAL_UART_Receive_IT(huart, inputBuf, 1);
	    }

	    /* 4. 可��：记录错误日志或触发报譄1�7 */
	    //Error_Counter++;  // 全局错误计数噄1�7
    }
	else if (huart->Instance == USART2){

		 /* 1. 清除扢�有可能出现的错误标志 */
	    // 使用单条语句清除多个标志（更高效＄1�7
	    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_FEF);

	    /* 2. 读取状��和数据寄存器（清空残留数据＄1�7*/
	    // 使用UNUSED宏避免编译器警告（如果不霢�要实际��）
	    //UNUSED(uint32_t temp_isr = huart->Instance->ISR);  // 读取ISR会清除部分标忄1�7
	    //UNUSED(uint32_t temp_rdr = huart->Instance->RDR);  // 清空接收寄存噄1�7
		  /* 2. 清空寄存器（箢�洁写法）*/
		(void)huart->Instance->ISR;  // 清除状��标忄1�7
		(void)huart->Instance->RDR;  // 清空接收数据

		  /* 3. 重启接收（带错误棢�查）*/
	    if (HAL_UART_GetState(huart) == HAL_UART_STATE_READY) {
	          UART_Start_Receive_IT(&huart2,wifi_rx_inputBuf,1);// 重新启动单字节中断接攄1�7
	    }

	}
}


