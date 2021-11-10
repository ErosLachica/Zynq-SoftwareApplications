#include <stdio.h>
#include "xil_printf.h"
#include "xil_io.h"
#include "xparameters.h"

#define BRAM_BASEADDR XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR

// Parameters for Waveform Generator
// Can be changed according to user specifications
int finalValue = 200;
int dropValue = 150;
int stepHorizontal = 25;
int stepVertical = 25;
int period = 200;

// Function that will generate the waveform
uint32_t generateWaveform(uint16_t finalVal, uint16_t dropVal, uint16_t horizontalVal, uint16_t verticalVal, uint16_t per) {
	// the variables will be written into memory cells of BRAM
	// addresses are accessed via an 8 bit line
	Xil_Out16(BRAM_BASEADDR, per);
	Xil_Out16(BRAM_BASEADDR + 2, finalVal);
	Xil_Out16(BRAM_BASEADDR + 4, dropVal);
	Xil_Out16(BRAM_BASEADDR + 6, horizontalVal);
	Xil_Out16(BRAM_BASEADDR + 8, verticalVal);

	int y = finalVal - dropVal;
	int x = 0;

	Xil_Out16(BRAM_BASEADDR + 10, x);
	Xil_Out16(BRAM_BASEADDR + 12, y);

	// Incrementing the addresses for the x and y axis values
	for (int i = 0; i < (per/horizontalVal)*4; i = i+4) {
		x = x + horizontalVal;
		y = y + verticalVal;

		// change address for x and y if value greater than finalVal
		if(y > finalVal){
			y = finalVal;
		}

		Xil_Out16(BRAM_BASEADDR + 14 + i, x);
		Xil_Out16(BRAM_BASEADDR + 16 + i, y);

	}

	return BRAM_BASEADDR;
}

// Parsing the BRAM_BASEADDR
// Reads the values written into the memory cells of the BRAM
void displayData(uint32_t addr) {
	for(int i = 0; i <= (Xil_In16(addr+0)/Xil_In16(addr+6))*4; i = i+4) {
		xil_printf("%d, %d\n\r", Xil_In16(addr+10+i), Xil_In16(addr+12+i));
	}
}

int main() {
	// Generates the waveform using Parameter Values
	// and display the plot values in terminal

	displayData(generateWaveform(finalValue, dropValue, stepHorizontal, stepVertical, period));

	return 0;
}
