
#include "main.h"

#define mainQUEUE_LENGTH 5

void vLED_Init(void);
void vQueueSendMessegeTask(void *pvParameters);
void vLedToggleTask (void *pvParameters);

uint32_t ulIdleCnt = 0; // idle counter
static QueueHandle_t xQueue = NULL; // queue descriptor 
typedef struct // just for test
{
	char cMessageID;	
	long lMessageValue;
} xQueueMessage;


int main(void){
	
	vLED_Init();
	
	xQueue=xQueueCreate(mainQUEUE_LENGTH, sizeof(xQueueMessage));
	
	xTaskCreate(vQueueSendMessegeTask, "QueueSendMessege", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY+1), NULL);
	xTaskCreate(vLedToggleTask, "LedToggleTask", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY+1), NULL);
	
	vTaskStartScheduler();
	
	while(1){
	}
		
}

void vApplicationTickHook(void){ // this is FreeRTOS func.
	ulIdleCnt = 0; // reset idle counter for new tick
}

void vApplicationIdleHook(void){ // this is FreeRTOS func.
	__nop(); 
	ulIdleCnt++; // increment idle counter
}

void vQueueSendMessegeTask(void *pvParameters){ // send some to queue, so handle the vLedToggleTask
	xQueueMessage xMessage;
	while(1){
		xMessage.cMessageID = 1; // just for test
		xMessage.lMessageValue = 12; // just for test
		
		xQueueSend(xQueue, &xMessage, portMAX_DELAY); // send 
		
		vTaskDelay(200/portTICK_PERIOD_MS); // block this task for 200ms
	}
}

void vLedToggleTask(void *pvParameters){ // waiting for some messege, then toggle led- pin
	xQueueMessage xReceivedMessage;
	while(1){
		xQueueReceive( xQueue, &xReceivedMessage, portMAX_DELAY ); // wait until some messege 
		
		switch( xReceivedMessage.cMessageID ){ //just for test
			case 1:
				GPIOA->ODR ^= 1 << 5; // toggle PA5- pin
				break;
			default:
				break;
		}
	}
}

void vLED_Init(void){ // PA5(led) init
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); // GPIOA osc.
	while(!READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN)){} 
	
	SET_BIT(GPIOA->CRL, GPIO_CRL_MODE5_1); // PA5- output, max speed- 2 MHz
	CLEAR_BIT(GPIOA->CRL, GPIO_CRL_CNF5_0); // Push-Pull
}
	
