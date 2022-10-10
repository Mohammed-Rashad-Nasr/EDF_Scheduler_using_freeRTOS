/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"
/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

extern BaseType_t xTaskPeriodicCreate( TaskFunction_t pxTaskCode,
                            const char * const pcName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask , TickType_t period);
/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/


TaskHandle_t 	* Periodic_Transmitter_Hnd =NULL;
TaskHandle_t 	* Uart_Receiver_Hnd = NULL;
TaskHandle_t 	* Button_2_Hnd = NULL;
TaskHandle_t 	* Button_1_Hnd = NULL;
TaskHandle_t 	* Load2_Hnd = NULL;
TaskHandle_t 	* Load1_Hnd = NULL;

//QueueHandle_t xQueue ;														
QueueHandle_t xQueue1 ;														
QueueHandle_t xQueue2 ;
QueueHandle_t xQueue3 ;														

//Donot forget to change pin mode in GPIO_cfg.c file 														
#define BUT1_PIN PIN0
#define BUT2_PIN PIN1
#define BUT1_PORT PORT_0
#define BUT2_PORT PORT_0
/*
//Periodicity: 50, Deadline: 50
This task will monitor rising and falling edge on button 1 and send this event to the consumer task. 
(Note: The rising and failling edges are treated as separate events, hence they have separate strings
*/
void Button_1_Monitor ( void *pvParameter){
	 pinState_t  CurrentState = GPIO_read(BUT1_PORT,BUT1_PIN);
	 pinState_t  PrevState    = GPIO_read(BUT1_PORT,BUT1_PIN);
	 TickType_t xLastWakeTime = xTaskGetTickCount();
	const char * FallingString = "BUT1_FALL\n";
	const char * RisingString = "BUT1_RISE\n";
	for(;;){
	CurrentState = GPIO_read(BUT1_PORT,BUT1_PIN);
	if (CurrentState != PrevState) 
		{//edge cahnge detected
			PrevState = CurrentState ;
			if (CurrentState){
			//Rising Edge Detected
			//xQueueSend( xQueue1,( void * ) &RisingString,( TickType_t ) 50 );
				xQueueOverwrite(xQueue1,(void*)"R");
			}
			else {
				//Falling Edge Detected
			//xQueueSend( xQueue1,( void * ) &FallingString,( TickType_t ) 50 );
			xQueueOverwrite(xQueue1,(void*)"F");
			}
		}
	else 
		{
			xQueueOverwrite(xQueue1,(void*)"N");
		}
	vTaskDelayUntil( &xLastWakeTime , 50);}
	
}




/*
//Periodicity: 50, Deadline: 50
This task will monitor rising and falling edge on button 1 and send this event to the consumer task. 
(Note: The rising and failling edges are treated as separate events, hence they have separate strings
*/
void Button_2_Monitor ( void *pvParameter){
	 pinState_t  CurrentState = GPIO_read(BUT2_PORT,BUT2_PIN);
	 pinState_t  PrevState    = GPIO_read(BUT2_PORT,BUT2_PIN);
	TickType_t xLastWakeTime2 = xTaskGetTickCount();
	const char * FallingString = "BUT2_FALL\n";
	const char * RisingString = "BUT2_RISE\n";
	for(;;){
	CurrentState = GPIO_read(BUT2_PORT,BUT2_PIN);
	if (CurrentState != PrevState) 
		{//edge cahnge detected
			PrevState = CurrentState ;
			if (CurrentState){
			//Rising Edge Detected
			//xQueueSend( xQueue2,( void * ) &RisingString,( TickType_t ) 50 );
			xQueueOverwrite(xQueue2,(void*)"R");
			}
			else {
				//Falling Edge Detected
			//xQueueSend( xQueue2,( void * ) &FallingString,( TickType_t ) 50 );
			xQueueOverwrite(xQueue2,(void*)"F");
			}
		}
	else 
		{
			xQueueOverwrite(xQueue2,(void*)"N");
		}
	vTaskDelayUntil( &xLastWakeTime2 , 50);}
	
}






const char * s = "From Periodic_Transmitter..!\n";
int i =0;
typedef struct{
	uint8_t ID;
	char string[29];
}xMESSAGE;

xMESSAGE m1 ={ 3,"Periodic-Task.\n"};

#define UART_RECEIVER_MAX_LENGHT 28

#define BUT_TASK_MAX_LENGHT 10
#define Periodic_Transmitter_PERIODICITY 100
/*
Task 3: ""Periodic_Transmitter"", {Periodicity: 100, Deadline: 100}
This task will send preiodic string every 100ms to the consumer task*/
#if 1
void Periodic_Transmitter (void *pvParameter)
{
	TickType_t  PreviousWakeTime = xTaskGetTickCount();
	const char * s ="\nFrom Periodic_Transmitter..!\n";//20 ,5
	for(;;){
		//Write to the the Queue
    if( xQueue3 != 0 )
    {
       /* Send an unsigned long.  Wait for 10 ticks for space to become available if necessary. */
		int i =0;
		for( i = 0 ; i < 29 ; i++){//&s[i]
        	 xQueueSend( xQueue3,(&s[i]), Periodic_Transmitter_PERIODICITY ) ;
		}
	}
		vTaskDelayUntil(&PreviousWakeTime,Periodic_Transmitter_PERIODICITY);//Delay a task until a specified time. This function can be used by periodic tasks to ensure a constant execution frequency.
	}
}
#endif

#define UART_RECEIVER_PERIODICITY 20

/*
Task 4: ""Uart_Receiver"", {Periodicity: 20, Deadline: 20}
This is the consumer task which will write on UART any received string from other tasks	*/
void Uart_Receiver (void *pvParameter)
{
	TickType_t  PreviousWakeTime = xTaskGetTickCount();
	
	//signed  char string_buffer1[BUT_TASK_MAX_LENGHT];//="mohamed\n";
	//xMESSAGE * pxMEASSAGE ;
	//signed  char string_buffer2[BUT_TASK_MAX_LENGHT];
  signed char button1 ;
	signed char button2;
	signed  char string_buffer3[29];
	int j,t,h;
	
	for(;;){
		GPIO_write(PORT_0,PIN2,!GPIO_read(PORT_0,PIN2));
		//Read from Queue and send it over UART TX
				#if 1
		if(xQueueReceive( xQueue1,  &button1,  0 ) && button1 != 'N'){
			//for( j = 0 ; j < BUT_TASK_MAX_LENGHT; j++){
				//xQueueReceive( xQueue1,  &button1,  0 ); //0 >> DONOT BLOCK THE CODE		
			//}
		  xSerialPutChar('B');
			xSerialPutChar('1');
			xSerialPutChar(' ');
			xSerialPutChar('-');
			xSerialPutChar('>');
			xSerialPutChar(' ');
			xSerialPutChar(button1);
			xSerialPutChar('|');
			xSerialPutChar('_');

		xQueueReset(xQueue1);
			}
		if(xQueueReceive( xQueue2,  &button2,  0 ) && button2 != 'N' ){
			//for( t = 0 ; t < BUT_TASK_MAX_LENGHT; t++){
				//xQueueReceive( xQueue2,  &button2,  0 ); //0 >> DONOT BLOCK THE CODE		
			//}
			//vSerialPutString(( signed char *)"\nbutton2 has : ",UART_RECEIVER_MAX_LENGHT);
			xSerialPutChar('B');
			xSerialPutChar('2');
			xSerialPutChar(' ');
			xSerialPutChar('-');
			xSerialPutChar('>');
			xSerialPutChar(' ');
			xSerialPutChar(button2);
			xSerialPutChar('|');
			xSerialPutChar('_');
			//vSerialPutString(( signed char *)" edge\n",UART_RECEIVER_MAX_LENGHT);
		xQueueReset(xQueue2);
			}
		if(uxQueueMessagesWaiting(xQueue3) != 0){
			for( h = 0 ; h < 29; h++){
				xQueueReceive( xQueue3,  (string_buffer3+h),  0 ); //0 >> DONOT BLOCK THE CODE		
			}
		vSerialPutString(( signed char *)&string_buffer3,UART_RECEIVER_MAX_LENGHT);
		xQueueReset(xQueue3);
			}
				#endif
			
		vTaskDelayUntil(&PreviousWakeTime,UART_RECEIVER_PERIODICITY);//Delay a task until a specified time. This function can be used by periodic tasks to ensure a constant execution frequency.
	}

}

void Load1 (void *pvParameter){
	TickType_t  PreviousWakeTime = xTaskGetTickCount();
	//GPIO_write(PORT_0,PIN2,1);
	int a,b;
	for(;;){
		//GPIO_write(PORT_0,PIN0,1);
	for (a =0 ; a<37300;a++){
		
	}
		//GPIO_write(PORT_0,PIN0,0);
		for (b =0 ; b<37300;b++){
			
		}
	vTaskDelayUntil(&PreviousWakeTime,10);
	}
}

void Load2 (void *pvParameter){
	TickType_t  PreviousWakeTime = xTaskGetTickCount();
	//GPIO_write(PORT_0,PIN2,1);
	int d,f;
	for(;;){
		GPIO_write(PORT_0,PIN0,1);
	for (d =0 ; d<8950;d++){
		
	}
		GPIO_write(PORT_0,PIN0,0);
		for (f =0 ; f<8950;f++){
			
		}
	vTaskDelayUntil(&PreviousWakeTime,100);
	}
}


//for debug
void TestGPIO(void *pvParameter){
	TickType_t  PreviousWakeTime = xTaskGetTickCount();
	for(;;){
	GPIO_write(PORT_0,PIN2,!GPIO_read(PORT_0,PIN2));//toggling
	vTaskDelayUntil(&PreviousWakeTime,5);//Delay a task until a specified time. This function can be used by periodic tasks to ensure a constant execution frequency.
	}
}



int main( void )
{
	
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
 // xSerialPortInitMinimal(ser115200);
	


	#if(configUSE_EDF_SCHEDULER == 1)
	xQueue1=xQueueCreate( 10, sizeof( char ));
	xQueue2=xQueueCreate( 10, sizeof( char ));
	xQueue3=xQueueCreate( 29, sizeof( char ));
	#if 0
	xQueue1=xQueueCreate( 3, sizeof( xMESSAGE * ));
	
	
		
	if( xQueueSend( xQueue1, ( void * ) &m1, ( TickType_t ) Periodic_Transmitter_PERIODICITY ) != pdPASS )
					{
							/* Failed to post the message, even after 10 ticks. */
					}
#endif
	//xTaskPeriodicCreate( Button_1_Monitor, ( const char * ) "Button_1_Monitor", configMINIMAL_STACK_SIZE, NULL,1, Button_1_Hnd, 50 );
	xTaskPeriodicCreate( Button_2_Monitor, ( const char * ) "Button_2_Monitor", configMINIMAL_STACK_SIZE, NULL,1, Button_2_Hnd, 50 );
	xTaskPeriodicCreate( Periodic_Transmitter, ( const char * ) "Periodic_Transmitter", configMINIMAL_STACK_SIZE, NULL,1, Periodic_Transmitter_Hnd, Periodic_Transmitter_PERIODICITY );
	xTaskPeriodicCreate( Uart_Receiver , ( const char * ) "Uart_Receiver", configMINIMAL_STACK_SIZE, NULL,1, Uart_Receiver_Hnd, UART_RECEIVER_PERIODICITY );
	xTaskPeriodicCreate( Load1 , ( const char * ) "Load1", configMINIMAL_STACK_SIZE, NULL,1, Load1_Hnd, 10 );
	xTaskPeriodicCreate( Load2 , ( const char * ) "Load2", configMINIMAL_STACK_SIZE, NULL,1, Load2_Hnd, 100 );
	//For debug
	///xTaskPeriodicCreate( TestGPIO , ( const char * ) "Test", configMINIMAL_STACK_SIZE, NULL,1, NULL, 5 );
	#else
	xTaskCreate( TestGPIO , ( const char * ) "Test", configMINIMAL_STACK_SIZE, NULL,1, NULL );
	#endif
	
    /* Create Tasks here */
	

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void ){
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/
#if (configUSE_TICK_HOOK == 1) 
void vApplicationTickHook( void ){
		GPIO_write(PORT_0,PIN3,!GPIO_read(PORT_0,PIN3));
}

#endif


#if (configUSE_IDLE_HOOK == 1) 
void vApplicationIdleHook( void ){
GPIO_write(PORT_0,PIN4,!GPIO_read(PORT_0,PIN4));
}

#endif
