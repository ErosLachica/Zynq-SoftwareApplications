#include <stdio.h>
#include "platform.h"
#include "xbasic_types.h"
#include "xparameters.h"
#include "xil_printf.h"
#include <stdlib.h>
#include <math.h>

Xuint32 *baseaddr_p = (Xuint32 *)XPAR_MY_COMPLEX_MULTIPLIER_0_S00_AXI_BASEADDR;

int main()
{
	init_platform();

	xil_printf("Complex Multiplier Test\n\r");

	u32 num_Array[100]; // 100 pairs of numbers for input a & b of multiplier
	u32 num_Array2[100]; // 100 pairs of numbers for input c & d of multiplier

	// Process of populating a and b values with 16-bit data (size of 100)
	size_t num_Array_Size = sizeof(num_Array)/sizeof(u32);
	// Populate within upper bound of 32 bit max value
	for (size_t i = 0; i < num_Array_Size; i++) {
		num_Array[i] = rand() % (int) pow(2, 32);
	}

	// Process of populating c and d values with 16-bit data (size of 100)
	size_t num_Array2_Size = sizeof(num_Array2)/sizeof(u32);
	// Populate within upper bound of 32 bit max value
	for (size_t k = 0; k < num_Array2_Size; k++) {
		num_Array2[k] = rand() % (int) pow(2, 32);
	}

	// Printing the products passing 100 pairs of numbers into multiplier
	for (size_t j = 0; j < num_Array_Size; j++) {
		*(baseaddr_p+0) = num_Array[j]; // input of a and b values
		*(baseaddr_p+1) = num_Array2[j]; // input of c and d values

		// Print Input Pairs (a & b populated values)
		xil_printf("Input Pair (a&b) #%d => a&b (HEX) = 0x%08x 0x%08x \n", j+1, *(baseaddr_p+0) >> 16, *(baseaddr_p+0) & 0x0000FFFF);
		xil_printf("Input Pair (c&d) #%d => c&d (HEX) = 0x%08x 0x%08x \n", j+1, *(baseaddr_p+1) >> 16, *(baseaddr_p+1) & 0x0000FFFF);

		xil_printf("Read Complex Real Result : 0x%08x (HEX) \n", *(baseaddr_p+2));
		xil_printf("Read Complex Imaginary Result : 0x%08x (HEX) \n", *(baseaddr_p+3));
	}
	xil_printf("End of test\n\n\r");

	return 0;
}
