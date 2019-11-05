#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops

//Static base
static unsigned GPIO_BASE = 0x3f200000;

//Regs pointers
volatile unsigned int * gpfsel0;
volatile unsigned int * gpset0;
volatile unsigned int * gpclr0;
volatile unsigned int * gplev0;

//Initialize pointers: performs memory mapping, exits if mapping fails
void gpioInitPtrs(){
	int fd = -1;
	//Loading /dev/mem into a file
	if ((fd = open("/dev/mem", O_RDWR, 0)) == -1)
		err(1, "Error opening /dev/mem");
	//Mapping GPIO base physical address
	gpfsel0 = (unsigned int*)mmap(0, getpagesize(),
	PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
	//Check for mapping errors
	if (gpfsel0 == MAP_FAILED)
		errx(1, "Error during mapping GPIO");
	//Set regs pointers
	gpset0 = gpfsel0 + 0x7; // offset 0x1C / 4 = 0x7
	gpclr0 = gpfsel0 + 0xA; // offset 0x28 / 4 = 0xA
	gplev0 = gpfsel0 + 0xD; // offset 0x34
}

//Sets GPIO<pin> as <mode>
void gpioSetMode(int pin, int mode){
	int pinRegIdx = pin/10;
	*(gpfsel0+pinRegIdx) = *(gpfsel0+pinRegIdx) | (mode << (pin%10)*3);
}

//Sets all GPIOs as <mode>
void gpioSetAllMode(int mode){
	for (int i=0; i<=53; ++i)gpioSetMode(i, mode);
}

//Sets GPIOs in a list to <mode>
//Requires number of gpios in the list as arg.
void gpioListSetMode(int *gpios, int ngpios, int mode){
	for (int i=0; i<ngpios; ++i)gpioSetMode(gpios[i], mode);
}

//Gets GPIO<pin>'s mode
int gpioGetMode(int pin){
	int pinRegIdx = pin/10;
	int indxWithInReg = (pin%10)*3;
	return (*(gpfsel0+pinRegIdx) & (7 << indxWithInReg)) >> indxWithInReg;
}

//Gets all GPIO's mode
void gpioGetAllMode(int *modes){
	for (int i=0; i<=53; ++i)modes[i]=gpioGetMode(i);
}

//Gets the mode from all GPIO's in a list
//Requires number of gpios in the list as arg.
void gpioListGetMode(int *gpios, int ngpios, int *modes){
	for (int i=0; i<ngpios; ++i)modes[i]=gpioGetMode(i);
}

//Writes to GPIO<pin>
void gpioWrite(int pin, unsigned char bit){
	int pinRegIdx = pin/32;
	if (bit) *(gpset0+pinRegIdx) = *(gpset0+pinRegIdx) | (1 << (pin%32));
	else *(gpclr0+pinRegIdx) = *(gpclr0+pinRegIdx) | (1 << (pin%32)); //clears bit
}

//Writes to <bit> to all gpios.
void gpioWriteAll(unsigned char bit){
	for (int i=0; i<54; ++i)gpioWrite(i, bit);
}

//Writes to <bit> to all gpios in list
//Requires number of gpios in the list as arg.
void gpioListWrite(int *gpios, unsigned char bit, int ngpios){
	for (int i=0; i<ngpios; ++i)gpioWrite(gpios[i], bit);
}

//Reads from GPIO<pin>
int gpioRead(int pin){
	int pinRegIdx = pin/32;
	return *(gplev0+pinRegIdx) & (1 << (pin%32));
}

//Reads from all gipios
void gpioReadAll(int *values){
	for (int i=0; i<54; ++i)values[i]=gpioRead(i);
}

//Reads from all gipios in a list
//Requires the number of gpios as arg.
void gpioListRead(int *gpios, int ngpios, int *values){
	for (int i=0; i<ngpios; ++i)values[i]=gpioRead(gpios[i]);
}