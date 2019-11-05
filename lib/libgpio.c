#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops
#include <stdint.h>
#include <unistd.h>
#include <libgpio.h>

//Static base
static unsigned GPIO_BASE = 0x3f200000;

volatile unsigned int* gpfsel0;
volatile unsigned int* gpset0;
volatile unsigned int* gpclr0;
volatile unsigned int* gplev0;
volatile unsigned int* gpeds0;
volatile unsigned int* gpren0;
volatile unsigned int* gpfen0;
volatile unsigned int* gphen0;
volatile unsigned int* gplen0;
volatile unsigned int* gparen0;
volatile unsigned int* gpafen0;
volatile unsigned int* gppud;
volatile unsigned int* gppudclk0;

// Initialize pointers
void gpio_init(){
    int fd = -1;
    //Loading /dev/mem into a file
    if ((fd = open("/dev/mem", O_RDWR, 0)) == -1) err(1, "Error opening /dev/mem");
    //Mapping GPIO base physical address
    gpfsel0 = (unsigned int*) mmap(0, getpagesize(), PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
    //Check for mapping errors
    if (gpfsel0 == MAP_FAILED) errx(1, "Error during mapping GPIO");
    //Set regs pointers
    gpset0 = gpfsel0 + 0x07; 		// offset 0x1C / 4 = 0x07
    gpclr0 = gpfsel0 + 0x0A; 		// offset 0x28 / 4 = 0x0A
    gplev0 = gpfsel0 + 0x0D; 		// offset 0x34 / 4 = 0x0D
	gpeds0 = gpfsel0 + 0x10; 		// offset 0x40 / 4 = 0x10
	gpren0 = gpfsel0 + 0x13; 		// offset 0x4C / 4 = 0x13
	gpfen0 = gpfsel0 + 0x16;		// offset 0x58 / 4 = 0x16
	gphen0 = gpfsel0 + 0x19;		// offset 0x64 / 4 = 0x19
	gplen0 = gpfsel0 + 0x1C;		// offset 0x70 / 4 = 0x1C
	gparen0 = gpfsel0 + 0x1F;		// offset 0x7C / 4 = 0x1F
	gpafen0 = gpfsel0 + 0x22;		// offset 0x88 / 4 = 0x22
	gppud = gpfsel0 + 0x25;			// offset 0x94 / 4 = 0x25
	gppudclk0 = gpfsel0 + 0x26;		// offset 0x98 / 4 = 0x98
}

// Set modes functions
void gpio_pin_set_mode(uint8_t pin, uint8_t mode){
	uint8_t register_index = pin/PINS_PER_REGISTER;
	volatile unsigned int* gpfseln = gpfsel0 + register_index;
	uint8_t lower3 = mode & 0x7; // Gets lower 3 bits from mode
	uint8_t value = 0xf8 | lower3; // Sets higher 5 bits in 1
	*gpfseln = *gpfseln & (value << 3*(pin%PINS_PER_REGISTER)); //Ands to set to low
}
void gpio_pin_set_all_modes(uint8_t mode){
	for(uint8_t i = 0; i < MAX_PINS; ++i){
		gpio_pin_set_mode(i, mode);
	}
}
void gpio_pin_set_list_modes(uint8_t* pins, uint8_t npins, uint8_t mode){
	for(uint8_t i = 0; i < npins; ++i){
		uint8_t pin = pins[i];
		if(pin < MAX_PINS) gpio_pin_set_mode(pin, mode);
	}
}

// Get modes functions
uint8_t gpio_pin_get_mode(uint8_t pin){
	uint8_t register_index = pin/PINS_PER_REGISTER;
	volatile unsigned int* gpfseln = gpfsel0 + register_index;

	uint8_t shift_position = 3*(pin%PINS_PER_REGISTER);
	uint8_t mode = (*gpfseln) >> shift_position;
	return mode & 0x7;
}
uint8_t* gpio_pin_get_all_modes(uint8_t* modes){
	for(uint8_t i = 0; i < MAX_PINS; ++i){
		modes[i] = gpio_pin_get_mode(i); 
	}
	return modes;
}
uint8_t* gpio_pin_get_list_modes(uint8_t* pins, uint8_t npins, uint8_t* modes){
	for(uint8_t i = 0; i < npins; ++i){
		uint8_t pin = pins[i];
		if(pin < MAX_PINS) modes[i] = gpio_pin_get_mode(pin); ;
	}
	return modes;
}

// Write functions
void gpio_write(uint8_t pin, uint8_t value){
	uint8_t mode = gpio_pin_get_mode(pin);
	if(mode == OUTPUT){
		uint8_t register_index = pin/32;
		uint8_t index_in_register = pin%32;
		volatile unsigned int* gpreg = value ? gpset0 : gpclr0;
		*(gpreg+register_index) = *(gpreg+register_index) | (1 << index_in_register);
	}
}
void gpio_write_all(uint8_t value){
	for(uint8_t i = 0; i < MAX_PINS; ++i){
		gpio_write(i, value);
	}
}
void gpio_write_list(uint8_t* pins, uint8_t npins, uint8_t value){
	for(uint8_t i = 0; i < npins; ++i){
		uint8_t pin = pins[i];
		if(pin < MAX_PINS) gpio_write(pin, value);
	}
}

// Read functions
uint8_t gpio_read(uint8_t pin){
	uint8_t register_index = pin/32;
	uint8_t index_in_register = pin%32;
	volatile unsigned int* gplevn = gplev0 + register_index;
	return (*(gplevn) >> index_in_register) & 1;
}
uint8_t* gpio_read_all(uint8_t* values){
	for(uint8_t i = 0; i < MAX_PINS; ++i){
		values[i] = gpio_pin_get_mode(i); 
	}
	return values;
}
uint8_t* gpio_read_list(uint8_t* pins, uint8_t npins, uint8_t* values){
	for(uint8_t i = 0; i < npins; ++i){
		uint8_t pin = pins[i];
		if(pin < MAX_PINS) values[i] = gpio_pin_get_mode(pin); ;
	}
	return values;
}