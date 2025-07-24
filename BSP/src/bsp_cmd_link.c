/*
 * bsp_cmd_link.c
 *
 *  Created on: 2025骞�3鏈�4鏃�
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

uint8_t rxBuffer[RX_DATA_SIZE];  // DMA鎺ユ敹缂撳啿鍖�


static uint8_t transferSize;
uint8_t outputBuf[MAX_BUFFER_SIZE];
volatile uint8_t dataReceived = 0;  // 鎺ユ敹瀹屾垚鏍囧織


// 鍏叡鍑芥暟锛氬～鍏呭抚鏁版嵁
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
    buf[0] = FRAME_HEADER;          // 甯уご
    buf[1] = 0x10;                  // 涓绘澘璁惧鍙�
    buf[2] = 0xFF;                  // 搴旂瓟淇″彿鏍囧織
    buf[3] = cmd;                   // 鍛戒护绫诲瀷
    buf[4] = (dataLen > 0) ? 0x0F : 0x00; // 鏁版嵁鏍囧織锛�0x0F 琛ㄧず鏈夋暟鎹紝0x00 琛ㄧず鏃犳暟鎹�

    if (buf[4] == 0x00) {           // 鏃犳暟鎹殑鎯呭喌
        buf[5] = data[0];           // 鍏蜂綋鎸囦护
        buf[6] = FRAME_END;         // 甯у熬
        buf[7] = bcc_check(buf, 7); // 鏍￠獙鐮�
    } else {                        // 鏈夋暟鎹殑鎯呭喌
        buf[5] = dataLen;           // 鏁版嵁闀垮害
        if (data != NULL) {         // 妫€鏌ユ暟鎹寚閽堟槸鍚︽湁鏁�
            for (uint8_t i = 0; i < dataLen; i++) {
                buf[6 + i] = data[i]; // 濉厖鏁版嵁
            }
        }
        buf[6 + dataLen] = FRAME_END;         // 甯у熬
        buf[7 + dataLen] = bcc_check(buf, 7 + dataLen); // 鏍￠獙鐮�
    }
}



// 鍏叡鍑芥暟锛氬彂閫佹暟鎹�
void TransmitData(uint8_t *buf, uint8_t size) 
{
    transferSize = size;

    #if USART1_IT_FLAG
    if (transferSize) {
        while (transOngoingFlag); // 绛夊緟浼犺緭瀹屾垚
        transOngoingFlag = 1;
        HAL_UART_Transmit_IT(&huart1, buf, transferSize);
    }
    #else
    HAL_UART_Transmit_DMA(&huart1, buf, transferSize);
    #endif
}

// 鍙戦€佸疄鏃舵俯婀垮害鏁版嵁
void sendData_Real_TimeHum(uint8_t hum, uint8_t temp) 
{
    uint8_t data[2] = {hum, temp};
    FillFrame(outputBuf, 0x1A, data, 2);
    TransmitData(outputBuf, 9);
}

// 鍙戦€佹椂闂存暟鎹�
void SendWifiData_To_PanelTime(uint8_t hours, uint8_t minutes, uint8_t seconds) 
{
    uint8_t data[3] = {hours, minutes, seconds};
    FillFrame(outputBuf, 0x1C, data, 3);
    TransmitData(outputBuf, 10);
}

// 鍙戦€佸懡浠ゆ暟鎹�
void SendData_Set_Command(uint8_t cmd, uint8_t data) 
{
    uint8_t cmdData[1] = {data};
    FillFrame(outputBuf, cmd, cmdData, 0);
    TransmitData(outputBuf, 7);
}

// 鍙戦€侀閫熸暟鎹�
void SendWifiData_To_PanelWindSpeed(uint8_t speed) {
    uint8_t data[1] = {speed};
    FillFrame(outputBuf, 0x1E, data, 1);
    TransmitData(outputBuf, 8);
}

// 鍙戦€佸懡浠ゅ搷搴�
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
    // 娓呯┖缂撳啿鍖�
    memset(rxBuffer, 0, MAX_BUFFER_SIZE);
    dataReceived = 0;
    
    // 鍚姩DMA鎺ユ敹
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

		txComplete = 1;//uartTxComplete = 1; // 鏍囪鍙戦€佸畬鎴�

		#endif 
	}

//	if(huart== &huart2){
//
//       usart2_transOngoingFlag =0;
//
//	}

}

/**
  * @brief  UART閿欒鍥炶皟鍑芥暟锛屽鐞哢SART1閫氫俊閿欒
  * @param  huart: UART鍙ユ焺鎸囬拡
  */

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) 
{
    

	if (huart->Instance == USART1) {
        // 閲嶆柊鍒濆鍖栨垨鎶ヨ
        #if 0
          __HAL_UART_CLEAR_OREFLAG(&huart1);
          __HAL_UART_CLEAR_NEFLAG(&huart1);
          __HAL_UART_CLEAR_FEFLAG(&huart1);
           
          
          temp=USART1->ISR;
          temp = USART1->RDR;
		  
     
		  UART_Start_Receive_IT(&huart1,inputBuf,1);
		 #endif 
	    /* 1. 娓呴櫎鎵€鏈夊彲鑳藉嚭鐜扮殑閿欒鏍囧織 */
	    // 浣跨敤鍗曟潯璇彞娓呴櫎澶氫釜鏍囧織锛堟洿楂樻晥锛�
	    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_FEF);

	    /* 2. 璇诲彇鐘舵€佸拰鏁版嵁瀵勫瓨鍣紙娓呯┖娈嬬暀鏁版嵁锛�*/
	    // 浣跨敤UNUSED瀹忛伩鍏嶇紪璇戝櫒璀﹀憡锛堝鏋滀笉闇€瑕佸疄闄呭€硷級
	    //UNUSED(uint32_t temp_isr = huart->Instance->ISR);  // 璇诲彇ISR浼氭竻闄ら儴鍒嗘爣蹇�
	    //UNUSED(uint32_t temp_rdr = huart->Instance->RDR);  // 娓呯┖鎺ユ敹瀵勫瓨鍣�
	      /* 2. 娓呯┖瀵勫瓨鍣紙绠€娲佸啓娉曪級*/
		    (void)huart->Instance->ISR;  // 娓呴櫎鐘舵€佹爣蹇�
		    (void)huart->Instance->RDR;  // 娓呯┖鎺ユ敹鏁版嵁

	    /* 3. 閲嶅惎鎺ユ敹锛堝甫閿欒妫€鏌ワ級*/
	    if (HAL_UART_GetState(huart) == HAL_UART_STATE_READY) {
	         Start_DMA_Receive(); // HAL_UART_Receive_IT(huart, inputBuf, 1);  // 閲嶆柊鍚姩鍗曞瓧鑺備腑鏂帴鏀�
	    } else {
	        // 鍙€夛細纭欢澶嶄綅USART锛堜弗閲嶉敊璇椂锛�
	        __HAL_UART_DISABLE(huart);
	        __HAL_UART_ENABLE(huart);
	        HAL_UART_Receive_IT(huart, inputBuf, 1);
	    }

	    /* 4. 鍙€夛細璁板綍閿欒鏃ュ織鎴栬Е鍙戞姤璀� */
	    //Error_Counter++;  // 鍏ㄥ眬閿欒璁℃暟鍣�
    }
	else if (huart->Instance == USART2){

		 /* 1. 娓呴櫎鎵€鏈夊彲鑳藉嚭鐜扮殑閿欒鏍囧織 */
	    // 浣跨敤鍗曟潯璇彞娓呴櫎澶氫釜鏍囧織锛堟洿楂樻晥锛�
	    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_FEF);

	    /* 2. 璇诲彇鐘舵€佸拰鏁版嵁瀵勫瓨鍣紙娓呯┖娈嬬暀鏁版嵁锛�*/
	    // 浣跨敤UNUSED瀹忛伩鍏嶇紪璇戝櫒璀﹀憡锛堝鏋滀笉闇€瑕佸疄闄呭€硷級
	    //UNUSED(uint32_t temp_isr = huart->Instance->ISR);  // 璇诲彇ISR浼氭竻闄ら儴鍒嗘爣蹇�
	    //UNUSED(uint32_t temp_rdr = huart->Instance->RDR);  // 娓呯┖鎺ユ敹瀵勫瓨鍣�
		  /* 2. 娓呯┖瀵勫瓨鍣紙绠€娲佸啓娉曪級*/
		(void)huart->Instance->ISR;  // 娓呴櫎鐘舵€佹爣蹇�
		(void)huart->Instance->RDR;  // 娓呯┖鎺ユ敹鏁版嵁

		  /* 3. 閲嶅惎鎺ユ敹锛堝甫閿欒妫€鏌ワ級*/
	    if (HAL_UART_GetState(huart) == HAL_UART_STATE_READY) {
	          UART_Start_Receive_IT(&huart2,wifi_rx_inputBuf,1);// 閲嶆柊鍚姩鍗曞瓧鑺備腑鏂帴鏀�
	    }

	}
}


