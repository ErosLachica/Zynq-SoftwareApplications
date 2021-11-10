#include "xparameters.h"
#include "xgpio.h"
#include "xtmrctr.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

// Parameter definitions
#define INTC_DEVICE_ID 		XPAR_PS7_SCUGIC_0_DEVICE_ID
#define TMR_DEVICE_ID		XPAR_TMRCTR_0_DEVICE_ID
#define BTNS_DEVICE_ID		XPAR_AXI_GPIO_0_DEVICE_ID
#define LEDS_DEVICE_ID		XPAR_AXI_GPIO_1_DEVICE_ID
#define SWS_DEVICE_ID		XPAR_AXI_GPIO_2_DEVICE_ID
#define INTC_GPIO_0_INTERRUPT_ID XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define INTC_GPIO_2_INTERRUPT_ID XPAR_FABRIC_AXI_GPIO_2_IP2INTC_IRPT_INTR
#define INTC_TMR_INTERRUPT_ID XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR

#define BTN_INT 			XGPIO_IR_CH1_MASK
#define SWS_INT				XGPIO_IR_CH1_MASK
#define D_TMR_LOAD			0xF8000000 // Default value for TMR_LOAD
#define C_TMR_LOAD			0xFF000000 // custom TMR_LOAD
#define D_tmr_count			3		   // Default value for tmr_count
#define C_tmr_count			10000000   // custom tmr_count

XGpio LEDInst, BTNInst, SWSInst;
XScuGic INTCInst;
XTmrCtr TMRInst;
static int led_data;
static int btn_value;
static int sws_value;
static int tmr_count;
static u32 TMR_LOAD;
static int tmr_count_const; // constraint value used in
							// if statement of timer handler
static int upDown;			// up = 0, down = 1

//----------------------------------------------------
// PROTOTYPE FUNCTIONS
//----------------------------------------------------
static void BTN_Intr_Handler(void *baseaddr_p);
static void TMR_Intr_Handler(void *baseaddr_p);
static void SWS_Intr_Handler(void *baseaddr_p);
static int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int IntcInitFunction(u16 DeviceId, XTmrCtr *TmrInstancePtr, XGpio *GpioInstancePtr, XGpio *Gpio2InstancePtr);

// Functions for switches
void pauseTimer() {
	XTmrCtr_Stop(&TMRInst,0);
}

void resumeTimer() {
	XTmrCtr_Start(&TMRInst,0);
}

void LED_UP() {
	upDown = 0;
}

void LED_DOWN(){
	upDown = 1;
}


//----------------------------------------------------
// INTERRUPT HANDLER FUNCTIONS
// - called by the timer, button interrupt, performs
// - LED flashing
//----------------------------------------------------


void BTN_Intr_Handler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&BTNInst, BTN_INT);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&BTNInst) & BTN_INT) !=
			BTN_INT) {
			return;
		}
	btn_value = XGpio_DiscreteRead(&BTNInst, 1);
	// Increment counter by 1 if BTN 0 pressed
	// Stop timer if BTN3 pressed
	// Reset timer if BTN1 pressed
	// Reset LEDs if BTN 2 pressed
	switch (btn_value) {
		case 8: XTmrCtr_Stop(&TMRInst,0); // BTN3
			break;
		case 4: led_data = 0; // BTN2
			break;
		case 2: XTmrCtr_Reset(&TMRInst,0); // BTN1
			XTmrCtr_Start(&TMRInst,0);
			break;
		case 1: led_data = led_data + btn_value; // BTN0
			break;
	}

    XGpio_DiscreteWrite(&LEDInst, 1, led_data);
    (void)XGpio_InterruptClear(&BTNInst, BTN_INT);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(&BTNInst, BTN_INT);
}

void TMR_Intr_Handler(void *data)
{
	if (XTmrCtr_IsExpired(&TMRInst,0)){
		// Once timer has expired 3 times, stop, increment counter
		// reset timer and start running again
		if(tmr_count == tmr_count_const){
			XTmrCtr_Stop(&TMRInst,0);
			tmr_count = 0;
			if (upDown == 0) {
				led_data++;
			}
			else if ((upDown == 1) && (led_data > 0)) {
				led_data--;
			}
			XGpio_DiscreteWrite(&LEDInst, 1, led_data);
			XTmrCtr_Reset(&TMRInst,0);
			XTmrCtr_Start(&TMRInst,0);
		}
		else tmr_count++;
	}
}

void SWS_Intr_Handler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&SWSInst, SWS_INT);
	// Ignore additional switches
	if ((XGpio_InterruptGetStatus(&SWSInst) & SWS_INT) !=
			SWS_INT) {
			return;
		}
	sws_value = XGpio_DiscreteRead(&SWSInst, 1);

	switch (sws_value) {
		case 0: tmr_count_const = D_tmr_count;
				TMR_LOAD = D_TMR_LOAD;
				LED_UP();
				resumeTimer();
				break;
		case 1: tmr_count_const = D_tmr_count;
				TMR_LOAD = D_TMR_LOAD;
				LED_UP();
				pauseTimer(); 			// change
				break;
		case 2: tmr_count_const = D_tmr_count;
				TMR_LOAD = D_TMR_LOAD;
				LED_DOWN();	   			//change
				resumeTimer(); 			// change
				break;
		case 3: tmr_count_const = D_tmr_count;
				TMR_LOAD = D_TMR_LOAD;
				LED_DOWN();
				pauseTimer(); // change
				break;
		case 4: tmr_count_const = D_tmr_count;
				TMR_LOAD = C_TMR_LOAD;  // change
				LED_UP();	   		    // change
				resumeTimer(); 		    // change
				break;
		case 5: tmr_count_const = D_tmr_count;
				TMR_LOAD = C_TMR_LOAD;
				LED_UP();
				pauseTimer(); 		    // change
				break;
		case 6: tmr_count_const = D_tmr_count;
				TMR_LOAD = C_TMR_LOAD;
				LED_DOWN();	   		    //change
				resumeTimer(); 		    // change
				break;
		case 7: tmr_count_const = D_tmr_count;
				TMR_LOAD = C_TMR_LOAD;
				LED_DOWN();
				pauseTimer(); 		    // change
				break;
		case 8: tmr_count_const = C_tmr_count; // change
				TMR_LOAD = D_TMR_LOAD;  //change
				LED_UP();	   		    // change
				resumeTimer(); 		    // change
				break;
		case 9: tmr_count_const = C_tmr_count;
				TMR_LOAD = D_TMR_LOAD;
				LED_UP();
				pauseTimer(); 		    // change
				break;
		case 10: tmr_count_const = C_tmr_count;
				TMR_LOAD = D_TMR_LOAD;
				LED_DOWN();	   		    //change
				resumeTimer(); 		    // change
				break;
		case 11: tmr_count_const = C_tmr_count;
				TMR_LOAD = D_TMR_LOAD;
				LED_DOWN();
				pauseTimer(); 		    // change
				break;
		case 12: tmr_count_const = C_tmr_count;
				TMR_LOAD = C_TMR_LOAD;  // change
				LED_UP();	   		    // change
				resumeTimer(); 		    // change
				break;
		case 13: tmr_count_const = C_tmr_count;
				TMR_LOAD = C_TMR_LOAD;
				LED_UP();
				pauseTimer(); 		    // change
				break;
		case 14: tmr_count_const = C_tmr_count;
				TMR_LOAD = C_TMR_LOAD;
				LED_DOWN();	   		    //change
				resumeTimer(); 		    // change
				break;
		case 15: tmr_count_const = C_tmr_count;
				TMR_LOAD = C_TMR_LOAD;
				LED_DOWN();
				pauseTimer(); 		    // change
				break;
	}

	XTmrCtr_SetResetValue(&TMRInst, 0, TMR_LOAD);
    (void)XGpio_InterruptClear(&SWSInst, SWS_INT);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(&SWSInst, SWS_INT);
}



//----------------------------------------------------
// MAIN FUNCTION
//----------------------------------------------------
int main (void)
{
  int status;
  //----------------------------------------------------
  // INITIALIZE THE PERIPHERALS & SET DIRECTIONS OF GPIO
  //----------------------------------------------------
  // Initialise LEDs
  status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);
  if(status != XST_SUCCESS) return XST_FAILURE;

  // Initialise Push Buttons
  status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
  if(status != XST_SUCCESS) return XST_FAILURE;

  // Initialize Switches
  status = XGpio_Initialize(&SWSInst, SWS_DEVICE_ID);
  if(status != XST_SUCCESS)	return XST_FAILURE;

  // Set LEDs direction to outputs
  XGpio_SetDataDirection(&LEDInst, 1, 0x00);
  // Set all buttons direction to inputs
  XGpio_SetDataDirection(&BTNInst, 1, 0xFF);
  // Set all switches direction to inputs
  XGpio_SetDataDirection(&SWSInst, 1, 0xF);

  // Initialize for default behavior
  TMR_LOAD = D_TMR_LOAD;
  tmr_count_const = D_tmr_count;
  upDown = 0;

  //----------------------------------------------------
  // SETUP THE TIMER
  //----------------------------------------------------
  status = XTmrCtr_Initialize(&TMRInst, TMR_DEVICE_ID);
  if(status != XST_SUCCESS) return XST_FAILURE;
  XTmrCtr_SetHandler(&TMRInst, TMR_Intr_Handler, &TMRInst);
  XTmrCtr_SetResetValue(&TMRInst, 0, TMR_LOAD);
  XTmrCtr_SetOptions(&TMRInst, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
 

  // Initialize interrupt controller
  status = IntcInitFunction(INTC_DEVICE_ID, &TMRInst, &BTNInst, &SWSInst);
  if(status != XST_SUCCESS) return XST_FAILURE;

  XTmrCtr_Start(&TMRInst, 0);




  while(1);

  return 0;
}

//----------------------------------------------------
// INITIAL SETUP FUNCTIONS
//----------------------------------------------------

int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
	// Enable interrupt
	XGpio_InterruptEnable(&BTNInst, BTN_INT);
	XGpio_InterruptGlobalEnable(&BTNInst);

	XGpio_InterruptEnable(&SWSInst, SWS_INT);
	XGpio_InterruptGlobalEnable(&SWSInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 	 	 	 	 	 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
			 	 	 	 	 	 XScuGicInstancePtr);
	Xil_ExceptionEnable();


	return XST_SUCCESS;

}

int IntcInitFunction(u16 DeviceId, XTmrCtr *TmrInstancePtr, XGpio *GpioInstancePtr, XGpio *Gpio2InstancePtr)
{
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialisation
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Call to interrupt setup
	status = InterruptSystemSetup(&INTCInst);
	if(status != XST_SUCCESS) return XST_FAILURE;
	
	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&INTCInst,
					  	  	 INTC_GPIO_0_INTERRUPT_ID,
					  	  	 (Xil_ExceptionHandler)BTN_Intr_Handler,
					  	  	 (void *)GpioInstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;

	status = XScuGic_Connect(&INTCInst,
					  	  	 INTC_GPIO_2_INTERRUPT_ID,
					  	  	 (Xil_ExceptionHandler)SWS_Intr_Handler,
					  	  	 (void *)Gpio2InstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;


	// Connect timer interrupt to handler
	status = XScuGic_Connect(&INTCInst,
							 INTC_TMR_INTERRUPT_ID,
							 (Xil_ExceptionHandler)TMR_Intr_Handler,
							 (void *)TmrInstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	// For buttons
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// For switches
	XGpio_InterruptEnable(Gpio2InstancePtr, 1);
	XGpio_InterruptGlobalEnable(Gpio2InstancePtr);

	// Enable GPIO and timer interrupts in the controller
	XScuGic_Enable(&INTCInst, INTC_GPIO_0_INTERRUPT_ID); // for button
	XScuGic_Enable(&INTCInst, INTC_GPIO_2_INTERRUPT_ID); // for switch
	
	XScuGic_Enable(&INTCInst, INTC_TMR_INTERRUPT_ID);
	

	return XST_SUCCESS;
}

