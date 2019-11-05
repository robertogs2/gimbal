#ifndef LIB_GPIO_H
#define LIB_GPIO_H

// Definition of function sel values
#define INPUT 		0
#define OUTPUT 		1
#define ALTERNATE_0	4
#define ALTERNATE_1 5
#define ALTERNATE_2 6
#define ALTERNATE_3 7
#define ALTERNATE_4 3
#define ALTERNATE_5 2

// Define LOW and HIGH for convenience
#define HIGH 	1
#define LOW 	0

//Initialize pointers: performs memory mapping, exits if mapping fails
void gpioInitPtrs();
//Sets GPIO<pin> as <mode>
void gpioSetMode(int pin, int mode);
//Sets all GPIOs as <mode>
void gpioSetAllMode(int mode);
//Sets GPIOs in a list to <mode>
//Requires number of gpios in the list as arg.
void gpioListSetMode(int *gpios, int ngpios, int mode);
//Gets GPIO<pin>'s mode
int gpioGetMode(int pin);
//Gets all GPIO's mode
void gpioGetAllMode(int *modes);
//Gets the mode from all GPIO's in a list
//Requires number of gpios in the list as arg.
void gpioListGetMode(int *gpios, int ngpios, int *modes);
//Writes to GPIO<pin>
void gpioWrite(int pin, unsigned char bit);
//Writes to <bit> to all gpios.
void gpioWriteAll(unsigned char bit);
//Writes to <bit> to all gpios in list
//Requires number of gpios in the list as arg.
void gpioListWrite(int *gpios, unsigned char bit, int ngpios);
//Reads from GPIO<pin>
int gpioRead(int pin);
//Reads from all gipios
void gpioReadAll(int *values);
//Reads from all gipios in a list
//Requires the number of gpios as arg.
void gpioListRead(int *gpios, int ngpios, int *values);

#endif