#include <libgpio.h>
#include <time.h> // clock
#include <stdio.h> // printf

#define NGPIOS 4
 
int main(){

	// Create list with gpio pins
	int gpioPins[NGPIOS] = {2,3,17,27};

	// Init pointers to gpio regs.
	printf("Initialising gpio pins: 2, 3, 17, 27\n");
	gpioInitPtrs();

	// Set mode for 5 different gpio pins
	printf("Seting mode to gpio pins: 2, 3, 17, 27 as OUTPUT\n");
	gpioListSetMode(gpioPins, NGPIOS, OUTPUT);

	// Set all gpio pins in list to low
	printf("Seting to LOW gpio pins: 2, 3, 17, 27\n");
	gpioListWrite(gpioPins, LOW, NGPIOS);
	unsigned char values[NGPIOS] = {LOW};

	printf("Now gpio0 will have  2 sec period\n");
	printf("Now gpio1 will have  3   sec period\n");
	printf("Now gpio2 will have  17   sec period\n");
	printf("Now gpio3 will have  27   sec period\n");

	clock_t currTime0, currTime1, currTime2, currTime3 = clock();
	// Wirte to gpios
	while(1){
		// gpio0 will have  1/2 sec period
		if((float)(clock()-currTime0)/(float)CLOCKS_PER_SEC >= 0.25f){
			printf("gpio0\n");
			values[0] ^= 1;
			gpioWrite(gpioPins[0], values[0]);
			currTime0 = clock();
		}
		// gpio1 will have a 1 sec period
		if((float)(clock()-currTime1)/(float)CLOCKS_PER_SEC >= 0.5f){
			printf("gpio1\n");
			values[1] ^= 1;
			gpioWrite(gpioPins[1], values[1]);
			currTime1 = clock();
		}
		// gpio2 will have a 2 sec period
		if((float)(clock()-currTime2)/(float)CLOCKS_PER_SEC >= 0.5f){
			printf("gpio2\n");
			values[2] ^= 1;
			gpioWrite(gpioPins[2], values[2]);
			currTime2 = clock();
		}
		// gpio3 will have a 4 sec period
		if((float)(clock()-currTime3)/(float)CLOCKS_PER_SEC >= 2.0f){
			printf("gpio3\n");
			values[3] ^= 1;
			gpioWrite(gpioPins[3], values[3]);
			currTime3 = clock();
		}

	}
	
	return 0;
}