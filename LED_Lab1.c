/*
 * Eros Aldin Lachica
 * 02/17/2021
 * Lab_1: First Design on Zynq GPIO
 * ECE 520
 * Professor Mirzaei
 */

/* Include Files */
#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xil_printf.h"

/* Definitions */
#define GPIO_DEVICE_ID  XPAR_AXI_GPIO_0_DEVICE_ID	/* GPIO device that LEDs are connected to */
#define LED 0x0										/* Initial LED value - 0000 */
#define SWITCH 0x0									/* Initial SW value - 0000 */
#define LED_DELAY 10000000							/* Software delay length */
#define LED_PORT 1								    /* GPIO port for LEDs */
#define SW_PORT 2									/* GPIO port for switches */
#define printf xil_printf							/* smaller, optimised printf */

int LED_NUM = 0; // Variable tracks LED output value

// Turn all LEDs ON (DIP: 000)
void all_ON(int *customLED) {
	*customLED = 0xF;
}

// Turn all LEDs OFF (DIP: 001)
void all_OFF(int *customLED) {
	*customLED = 0x0;
}

// Ring Counter (DIP: 011)
void ringCounter(int *customLED) {
	*customLED = LED_NUM;
	if (LED_NUM >= 8) {
		LED_NUM = 1;
	}
	else if (LED_NUM >= 1) {
		LED_NUM *= 2;
	}
	else LED_NUM = 1;
}

// Johnson Counter - Hardcode (DIP: 100)
void johnsonCounter(int *customLED) {
	switch (*customLED) {
		case 0:
			LED_NUM = 8;
		break;

		case 8:
			LED_NUM = 12;
		break;

		case 12:
			LED_NUM = 14;
		break;

		case 14:
			LED_NUM = 15;
		break;

		case 15:
			LED_NUM = 7;
		break;

		case 7:
			LED_NUM = 3;
		break;

		case 3:
			LED_NUM = 1;
		break;

		case 1:
			LED_NUM = 0;
		break;

		default:
			LED_NUM = 0;
	}
	*customLED = LED_NUM;
}

// Binary Counter (DIP: 101)
void binaryCounter(int *customLED) {
	*customLED = LED_NUM;

	if (LED_NUM >= 15) {
		LED_NUM = 0;
	}
	else LED_NUM += 1;
}

// Gray Counter (DIP: 110)
void grayCounter(int *customLED) {
	*customLED = LED_NUM ^ (LED_NUM >> 1);

	if (LED_NUM >= 255) {
		LED_NUM = 0;
	}
	else LED_NUM += 1;
}

// Fibonacci Counter (DIP: 111)
void fibonacciCounter(int *customLED, int *num1, int *num2) {
	*customLED = *num1 + *num2;
	*num1 = *num2;
	*num2 = *customLED;

	if (*customLED == 13) { // since 13 is the greatest number that can be represented by LEDs
		*num1 = 0;
		*num2 = 1;
	}
}
void outputLED()
{
	/* GPIO Device driver instance */
	XGpio Gpio;

	volatile int Delay;
	int switch_Val = SWITCH; /* Hold current switch value. Initialize to SWITCH definition */
	int led = LED;
	int NUM1 = 0;
	int NUM2 = 1;

	/* GPIO driver initialization */
	XGpio_Initialize(&Gpio, GPIO_DEVICE_ID);

	/* Set the direction for the LEDs to output. */
	XGpio_SetDataDirection(&Gpio, LED_PORT, 0x0);

	/* Set the direction for the switch ports to be input */
	XGpio_SetDataDirection(&Gpio, SW_PORT, 0xF);

	while(1) {
		switch_Val = XGpio_DiscreteRead(&Gpio, SW_PORT); // Getting value from three DIP switches
		switch (switch_Val) {
			case 0:
				xil_printf("All LEDs On\n");
				all_ON(&led);
			break;

			case 1:
				xil_printf("All LEDs Off\n");
				all_OFF(&led);
			break;

			case 2:
				while (XGpio_DiscreteRead(&Gpio, SW_PORT) == 2) { // need to check state of DIP switch in while loop
					xil_printf("LEDs blinking ON and OFF\n");
					// ON state
					all_ON(&led);
					XGpio_DiscreteWrite(&Gpio, LED_PORT, led);
					for (Delay = 0; Delay < LED_DELAY; Delay++);

					// OFF state
					all_OFF(&led);
					XGpio_DiscreteWrite(&Gpio, LED_PORT, led);
					for (Delay = 0; Delay < LED_DELAY; Delay++);

				} // end of while
			break;

			case 3:
				xil_printf("Ring Counter in process...\n");
				ringCounter(&led);
			break;

			case 4:
				xil_printf("Johnson Counter in process...\n");
				johnsonCounter(&led);
			break;

			case 5:
				xil_printf("Binary Counter in process...\n");
				binaryCounter(&led);
			break;

			case 6:
				xil_printf("Gray Counter in process...\n");
				grayCounter(&led);
			break;

			case 7:
				xil_printf("Fibonacci Counter in process...\n");
				fibonacciCounter(&led, &NUM1, &NUM2);
			break;

			default:
				xil_printf("Invalid DIP Switch Input, LEDs OFF");
				led = 0; // Turn off all LEDs
		} // end switch

		/* Setting output of LEDs */
		XGpio_DiscreteWrite(&Gpio, LED_PORT, led);

		/* Delay for observing LED flash */
		for (Delay = 0; Delay < LED_DELAY; Delay++);
	} // end while
}

/* Main function. */
int main(){
	outputLED();
	return 0;
}

