
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "Board_LED.h"

#define LED_shared 0
#define LED_child1 3
#define LED_child2 1
#define DELAY1 3315541
#define DELAY2 5946561

void child1 (void const *argument);
void child2 (void const *argument);
osThreadDef (child1, osPriorityNormal, 1, 0);
osThreadDef (child2, osPriorityNormal, 1, 0);

osSemaphoreDef(LED);
osSemaphoreId (sem_id);
int sem,l2,ls,l1;

void my_delay(int num){
	int i;
	for (i=0;i<=num;i++);
}

int main (void) {
	LED_Initialize(); 
  SystemCoreClockUpdate();
  osKernelInitialize();
	
	osKernelStart();
	sem_id = osSemaphoreCreate(osSemaphore(LED),1);
  osThreadCreate(osThread(child1), NULL);
	osThreadCreate(osThread(child2), NULL);
  
  for (;;) {}
}


void child2 (void const *argument) { 
	for (;;) {
		if(osSemaphoreWait(sem_id,osWaitForever)>0){
				int i = 5;
				while (i--){
					LED_On(LED_shared);
					LED_On(LED_child1);
					l1=0;l2=1;ls=1;
					my_delay(DELAY1);
					LED_Off(LED_shared);
					LED_Off(LED_child1);
					l1=0;l2=0;ls=0;
					my_delay(DELAY1);
				}
				osSemaphoreRelease(sem_id);
		}
	}
}

void child1(void const *arguments){
	for (;;) {
		if(osSemaphoreWait(sem_id,osWaitForever)>0){
				int i = 5;
				while (i--){
					LED_On(LED_shared);
					LED_On(LED_child2);
					l1=1;l2=0;ls=1;
					my_delay(DELAY2);
					LED_Off(LED_shared);
					LED_Off(LED_child2);
					l1=0;l2=0;ls=0;
					my_delay(DELAY2);
				}
				osSemaphoreRelease(sem_id);
		}
	}
}
