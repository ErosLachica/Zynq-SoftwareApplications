/*
 * Eros Aldin Lachica
 * 3/10/2021
 * Lab_2: UART and GPIO
 * ECE 520
 * Professor Mirzaei
 */

/* Include Files */
#include <stdio.h>
#include "math.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xil_printf.h"
#include "math.h"
/* Definitions */
#define GPIO_DEVICE_ID XPAR_SW_4BIT_DEVICE_ID       /* GPIO device that switches are connected to */
#define GPIO1_DEVICE_ID  XPAR_BTNS_4BIT_DEVICE_ID	/* GPIO device that buttons are connected to */
#define GPIO2_DEVICE_ID XPAR_LED_4BIT_DEVICE_ID		/* GPIO device that LEDs are connected to */
#define LED 0x0										/* Initial LED value - 0000 */
#define SWITCH 0x0									/* Initial SW value - 0000 */
#define POLLING_DELAY 10000000						/* POLLING delay length */
#define SOFTWARE_DELAY 100000000					/* Delay for print statements */
#define BUTTON_CHANNEL 1						    /* GPIO port for buttons */
#define SW_CHANNEL 1							    /* GPIO port for switches */
#define LED_CHANNEL 1								/* GPIO port for LEDs */
#define printf xil_printf							/* smaller, optimised printf */

//====================================================

XGpio Button_Gpio;                                  /* GPIO Device driver instance */
XGpio SW_Gpio;
XGpio LED_Gpio;

int sum(u32 sw_val, u32 b_val) {
	return (sw_val+b_val);
}

int product(u32 sw_val, u32 b_val) {
	return (sw_val*b_val);
}

int diff(u32 sw_val, u32 b_val) {
	return (sw_val-b_val);
}

int remainders(u32 sw_val, u32 b_val) {
	return (sw_val%b_val);
}

int square_ceiling(u32 sw_val) {
	double result = 0;
	result = sqrt(sw_val);
	result = ceil(result);
	return (int) result;
}

int spowb(u32 sw_val, u32 b_val) {
	int n = sw_val;
	int p = b_val;
	int c = 1;
	int temp;

	for (int i = 1; i <= p; i++) {
		temp = n;
		c = c*temp;
	}
	return c;
}

int main (void) 
{


	//declare variables here
	volatile int Delay;
	u32 switch_Val = 0;
	u32 button_Val = 0;
	int result = 0;
	
    //Initialization using function XGpio_Initialize for dip switches
	XGpio_Initialize(&SW_Gpio, GPIO_DEVICE_ID);
	
	//Setting data direction using function XGpio_SetDataDirection for dip switches
	XGpio_SetDataDirection(&SW_Gpio, SW_CHANNEL, 0xF);
	
	//Initialization using function XGpio_Initialize for push buttons 
	XGpio_Initialize(&Button_Gpio, GPIO1_DEVICE_ID);
	
	//Setting data direction using function XGpio_SetDataDirection for push buttons
	XGpio_SetDataDirection(&Button_Gpio, BUTTON_CHANNEL, 0xF);
	
	//Initialization using function XGpio_Initialize for LEDs
	XGpio_Initialize(&LED_Gpio, GPIO2_DEVICE_ID);

	//Setting data direction using function XGpio_SetDataDirection for LEDs
	XGpio_SetDataDirection(&LED_Gpio, LED_CHANNEL, 0x0);

	//Constantly read the push button and dip switches values 
	//and write them to the output terminal using xil_printf function.
	//Also assign a delay between each pair of the write function
	while(1) {
		for (int i = 0; i < POLLING_DELAY; i++) { // Read via polling
			switch_Val = XGpio_DiscreteRead(&SW_Gpio, SW_CHANNEL); // Read value of switches
			button_Val = XGpio_DiscreteRead(&Button_Gpio, BUTTON_CHANNEL); // Read value of buttons
			result = switch_Val;

			/* LED Output Corresponding to Switch Value */
			XGpio_DiscreteWrite(&LED_Gpio, LED_CHANNEL, result);
			for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);

			/* Printing values for switch and button values */
			xil_printf("Switch Value: %d || Button Value: %d\n", switch_Val, button_Val);
			for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);

			/* Printing Sum */
			xil_printf(" Switch Value: %d || Button Value: %d || Sum: %d\n", switch_Val, button_Val, sum(switch_Val, button_Val));
			for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);

			/* Printing Difference */
			xil_printf(" Switch Value: %d || Button Value: %d || Difference: %d\n", switch_Val, button_Val, diff(switch_Val, button_Val));
			for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);

			/* Printing Product */
			xil_printf(" Switch Value: %d || Button Value: %d || Product: %d\n", switch_Val, button_Val, product(switch_Val, button_Val));
			for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);

			/* Printing Remainder */
			xil_printf(" Switch Value: %d || Button Value: %d || Remainder: %d\n", switch_Val, button_Val, remainders(switch_Val, button_Val));
			for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);

			/* Printing Switch Square Ceiling */
			xil_printf(" Switch Value: %d || Switch Square Ceiling: %d\n", switch_Val, square_ceiling(switch_Val));
			for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);

			/* Printing x^y (where x = switch_Val, and y = button_Val) */
			xil_printf(" Switch Value: %d || Button Value: %d || Sw_Pow_b: %d\n", switch_Val, button_Val, spowb(switch_Val, button_Val));
			for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);

			/* Delay for observing switch/button inputs */
			//for (Delay = 0; Delay < SOFTWARE_DELAY; Delay++);
		}

	}
}
 
