#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "Board_LED.h"
#include "stdio.h"

#define OS_ROBIN 0
#define delay 300000
#define error_delay 100000
#define green 0
#define orange 1
#define red 2
#define blue 3

/*
***************************global variables**********************
*/
int smallest_period = 0;
int largest_period = 0;
int max_task = 5;
int curr_task = 0;
int max_count=0;
int count=0;

typedef struct my_tcb{
	int id;
	os_pthread thread; //thread def
	int arrival;
	int length;
	int period;
} my_tcb;

int i,tcbpter=0,myid=0;
osThreadId gth,oth,rth,bth;
static uint32_t arg=2;
static osTimerId timer;
int schedular[5][4]={{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}};
/*
************************global variables end********************
*/
/*
***********************macro definition*************************
*/
#define mytask(tid,name,priority,instances,stacksz,arvl,len,prd) \
const osThreadDef_t os_thread_def_##name = \
{ (name), (priority), (instances), (stacksz)  } ;\
my_tcb mytcb_##name = {.id = tid ,\
				.thread = (name) ,\
				.arrival = (arvl) ,\
				.length = (len) ,\
				.period = (prd) \
};

#define myThread(name) mytcb_##name


/*
*********************macro definition end***********************
*/
/*
**********************function declaration**********************
*/
void my_delay(int num);
int error(void);
int hcf(int a,int b);
int lcm(int a,int b);
void myKernelStart(void);
osThreadId myThreadCreate(my_tcb tcb, const osThreadDef_t *thread_def, void *argument);
void greenchild(void const *arg);
void bluechild(void const *arg);
static void timer_callback(void const *arg);
/*
*********************function declaration end*******************
*/
/*
***********************global definition************************
*/
mytask(0,greenchild,osPriorityNormal,1,0,0,2500,5000);
mytask(1,bluechild,osPriorityNormal,1,0,0,2500,15000);
osTimerDef(t,timer_callback);
/*
*********************global definition end**********************
*/

int main(){
	LED_Initialize();
	SystemCoreClockUpdate();
	osKernelInitialize();
	/*create task*/
	
	gth = myThreadCreate(myThread(greenchild),osThread(greenchild),NULL);
	(gth==0)?error():curr_task++;
	bth = myThreadCreate(myThread(bluechild),osThread(bluechild),NULL);
	(bth==0)?error():curr_task++;
	
	/*create timer*/
	timer = osTimerCreate(osTimer(t),osTimerPeriodic,&arg);

	myKernelStart();
	osTimerStart(timer,smallest_period);
	
	osKernelStart();
	
	
	
	for(;;){
		osThreadYield();
	}
	
}
/*
**********************all functions*****************************
*/
static void timer_callback(void const *arg){
	count++;
	if (count>max_count)
		count=1;
	LED_On(red);
	my_delay(delay*2);
	
	osThreadTerminate(gth);
	osThreadTerminate(bth);
	
	curr_task-=2;
	

	
	if (count%schedular[0][3] == 0){
		gth = myThreadCreate(myThread(greenchild),osThread(greenchild),NULL);
		(gth==0)?error():curr_task++;
	}
	if (count%schedular[1][3] == 0){
		bth = myThreadCreate(myThread(bluechild),osThread(bluechild),NULL);
		(bth==0)?error():curr_task++;
	}
	LED_Off(red);
}


void greenchild(void const *arg){
	for (i=0;i<25;i++) 
	{
		LED_On(green);
		my_delay(100000);
		LED_Off(green);
		my_delay(100000);
	}
}

void bluechild(void const *arg){
	for(i=0;i<25;i++)
	{
		LED_On(blue);
		my_delay(100000);
		LED_Off(blue);
		my_delay(100000);
	}
}

osThreadId myThreadCreate(my_tcb tcb,const osThreadDef_t *thread_def, void *argument){
	if (curr_task < max_task){
		schedular[tcb.id][0]=tcb.arrival;
		schedular[tcb.id][1]=tcb.length;
		schedular[tcb.id][2]=tcb.period;
		return osThreadCreate(thread_def,argument);
	}
	else 
		return 0;
}

void myKernelStart(void){
	
	//find smallest period
	smallest_period = schedular[0][2];
	for (i=0;i<5;i++){
		if(schedular[i][2] !=-1)
			if (schedular[i][2]<smallest_period)
				smallest_period = hcf(smallest_period,schedular[i][2]);
	}
	//update after how many callback repeat the thread
	for(i=0;i<5;i++){
		if (schedular[i][0]!=-1)
			schedular[i][3] = schedular[i][2]/smallest_period;
	}
	
	//find the major cycle
	largest_period = schedular[0][2];
	for(i=1;i<5;i++){
		if (schedular[i][2]!=-1)
			largest_period = lcm(largest_period,schedular[i][2]);
	}
	max_count = largest_period / smallest_period;
	
/*	int j;
	for(i=0;i<5;i++)
		for(j=0;j<5;j++)
			printf("%d",schedular[i][j]);
	*/
}

int lcm(int a,int b){
	int temp;
	int x=a,y=b;
	while(b!=0){
		temp=b;
		b=a%b;
		a=temp;
	}
	return (x*y)/a;
}
int hcf(int a,int b){
	int temp;
	while(b!=0){
		temp=b;
		b=a%b;
		a=temp;
	}
	return a;
}

int error(void){
	for (;;){
		LED_On(red);
		LED_On(blue);
		LED_On(green);
		LED_On(orange);
		my_delay(error_delay);
		LED_Off(red);
		LED_Off(blue);
		LED_Off(green);
		LED_Off(orange);
		my_delay(error_delay);
	}
	//initialize
}

void my_delay(int num){
	while(num--);
}
/*
********************all functions end***************************
*/
