
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "Board_LED.h" 

#define LED_shared 1
#define LED_child1 0
#define LED_child2 2

osThreadId_t ch_id1,ch_id2;
osSemaphoreId_t sem_id;
int sem,f1=0,f2=0;

void my_delay(int num){
	int i;
	for (i=0;i<=num;i++);
}

void get_my_led1(void *argument){
	int i = 5;
	//f1 = 1;
	while (i--){
		LED_On(LED_shared);
		my_delay(500000);
		LED_Off(LED_shared);
		my_delay(500000);
	}
	osSemaphoreRelease(sem_id);
	//my_delay(227847);
	//osThreadYield();
	//osThreadResume(ch_id2);
	//f1=0;
	osThreadExit();
	
}

void get_my_led2(void *argument){
	int i = 5;
	//f2=1;
	while (i--){
		LED_On(LED_shared);
		my_delay(100000);
		LED_Off(LED_shared);
		my_delay(100000);
	}
	osSemaphoreRelease(sem_id);
	//my_delay(400000);
	//osThreadYield();
	//osThreadResume(ch_id1);
	//f2 = 0;
	osThreadExit();
}

void child2 (void *argument) { 
	for (;;) {
		
		sem = osSemaphoreAcquire(sem_id,0);
		
		switch(sem){
			case osOK:
				osThreadNew(get_my_led2,NULL,NULL);
				break;
			default:
				//osThreadSuspend(ch_id2);
				/*if (f2==0)
					while (sem != osOK)
						sem = osSemaphoreAcquire(sem_id,0);
				else*/
					break;
		}
			LED_On(LED_child2);
			my_delay(100000);
			LED_Off(LED_child2);
			my_delay(100000);
		}
}

void child1(void *arguments){
	for (;;) {
		sem = osSemaphoreAcquire(sem_id,0);
		
		switch(sem){
			case osOK:
				osThreadNew(get_my_led1,NULL,NULL);
				break;
			default:
				//osThreadSuspend(ch_id1);
				/*if (f1==0)	
					while (sem != osOK)
						sem = osSemaphoreAcquire(sem_id,0);
				else*/
					break;
		}
			LED_On(LED_child1);
			my_delay(500000);
			LED_Off(LED_child1);
			my_delay(500000);
	}
}

int main (void) {
	LED_Initialize(); 
  SystemCoreClockUpdate();
  osKernelInitialize();
	sem_id = osSemaphoreNew(1,1,NULL);
  ch_id1 = osThreadNew(child1, NULL, NULL);
	ch_id2 = osThreadNew(child2, NULL, NULL);
  osKernelStart();
  for (;;) {}
}
