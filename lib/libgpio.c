#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgpio.h>

//Static base

static unsigned GPIO_PADS     	= GPIO_PERI_BASE + 0x00100000;
static unsigned GPIO_CLOCK_BASE = GPIO_PERI_BASE + 0x00101000;
static unsigned GPIO_BASE   	= GPIO_PERI_BASE + 0x00200000;
static unsigned GPIO_TIMER    	= GPIO_PERI_BASE + 0x0000B000;
static unsigned GPIO_PWM    	= GPIO_PERI_BASE + 0x0020C000;

volatile unsigned int *gpfsel0;
volatile unsigned int *gpset0;
volatile unsigned int *gpclr0;
volatile unsigned int *gplev0;
volatile unsigned int *gpeds0;
volatile unsigned int *gpren0;
volatile unsigned int *gpfen0;
volatile unsigned int *gphen0;
volatile unsigned int *gplen0;
volatile unsigned int *gparen0;
volatile unsigned int *gpafen0;
volatile unsigned int *gppud;
volatile unsigned int *gppudclk0;

volatile unsigned int *pwmctl;
volatile unsigned int *pwmsta;
volatile unsigned int *pwmdmac;
volatile unsigned int *pwmrng1;
volatile unsigned int *pwmdat1;
volatile unsigned int *pwmfif1;
volatile unsigned int *pwmrng2;
volatile unsigned int *pwmdat2;

volatile unsigned int *clkbase;
volatile unsigned int *pwmclkcntl;
volatile unsigned int *pwmclkdiv;

static pwm_status_t *pwm_status;
static uint8_t started = 0;
// Initialize pointers
void gpio_init() {
	if(started != 1){
		int fd = -1;
	    //Loading /dev/mem into a file
	    if ((fd = open("/dev/mem", O_RDWR, 0)) == -1) err(1, "Error opening /dev/mem");
	    //Mapping GPIO base physical address and PWM
	    gpfsel0 = (volatile unsigned int *) mmap(0, getpagesize(), PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
	    pwmctl = (volatile unsigned int *) mmap(0, getpagesize(), PROT_WRITE, MAP_SHARED, fd, GPIO_PWM);
	    clkbase = (volatile unsigned int *) mmap(0, getpagesize(), PROT_WRITE, MAP_SHARED, fd, GPIO_CLOCK_BASE);
	    //Check for mapping errors
	    if (gpfsel0 == MAP_FAILED) errx(1, "Error during mapping GPIO");
	    if (pwmctl == MAP_FAILED) errx(1, "Error during mapping PWM");
	    if (clkbase == MAP_FAILED) errx(1, "Error during mapping CLOCK");
	    //Set regs pointers for gpio
	    gpset0 = gpfsel0 + 0x07; 	// offset 0x1C / 4 = 0x07
	    gpclr0 = gpfsel0 + 0x0A; 	// offset 0x28 / 4 = 0x0A
	    gplev0 = gpfsel0 + 0x0D; 	// offset 0x34 / 4 = 0x0D
	    gpeds0 = gpfsel0 + 0x10; 	// offset 0x40 / 4 = 0x10
	    gpren0 = gpfsel0 + 0x13; 	// offset 0x4C / 4 = 0x13
	    gpfen0 = gpfsel0 + 0x16;	// offset 0x58 / 4 = 0x16
	    gphen0 = gpfsel0 + 0x19;	// offset 0x64 / 4 = 0x19
	    gplen0 = gpfsel0 + 0x1C;	// offset 0x70 / 4 = 0x1C
	    gparen0 = gpfsel0 + 0x1F;	// offset 0x7C / 4 = 0x1F
	    gpafen0 = gpfsel0 + 0x22;	// offset 0x88 / 4 = 0x22
	    gppud = gpfsel0 + 0x25;		// offset 0x94 / 4 = 0x25
	    gppudclk0 = gpfsel0 + 0x26;	// offset 0x98 / 4 = 0x26

	    // Set regs pointers for pwm
	    pwmsta = pwmctl + 0x01; 	// offset 0x04 / 4 = 0x01
	    pwmdmac = pwmctl + 0x02; 	// offset 0x08 / 4 = 0x02
	    pwmrng1 = pwmctl + 0x04; 	// offset 0x10 / 4 = 0x04
	    pwmdat1 = pwmctl + 0x05; 	// offset 0x14 / 4 = 0x05
	    pwmfif1 = pwmctl + 0x06; 	// offset 0x18 / 4 = 0x06
	    pwmrng2 = pwmctl + 0x08; 	// offset 0x20 / 4 = 0x08
	    pwmdat2 = pwmctl + 0x09; 	// offset 0x24 / 4 = 0x09

	    // Sets pointers for pwm clock
	    pwmclkcntl = clkbase + 0x28;// offset 0xa0 / 4 = 0x28
	    pwmclkdiv = clkbase + 0x29;	// offset 0xa4 / 4 = 0x29

	    pwm_status = (pwm_status_t*) malloc(sizeof(pwm_status_t));
	    started = 1;
	}


}

// Set modes functions
void gpio_pin_set_mode(uint8_t pin, uint8_t mode) {
    if(mode == PWM) {
        if(pin == 12 || pin == 13 || pin == 40 || pin == 41 || pin == 45) mode = ALT0;
        else if(pin == 18 || pin == 19) mode = ALT5;
        else if(pin == 52 || pin == 53) mode = ALT1;
        else printf("Pin is not able to output PWM\n");
        gpio_pin_set_mode(pin, OUTPUT);
        gpio_write(pin, HIGH);
    }
    uint8_t register_index = pin / PINS_PER_REGISTER;
    volatile unsigned int *gpfseln = gpfsel0 + register_index;
    uint8_t lower3 = mode & 0x7; // Gets lower 3 bits from mode
    unsigned int mask = ~(0x7 << (3 * (pin % PINS_PER_REGISTER))); // Mask to remove those 3 bits
    *gpfseln = *gpfseln & mask; 	//remove them
    *gpfseln = (*gpfseln) | (lower3 << (3 * (pin % PINS_PER_REGISTER))); //Ands to set to low
}

void gpio_pin_set_all_modes(uint8_t mode) {
    for(uint8_t i = 0; i < MAX_PINS; ++i) {
        gpio_pin_set_mode(i, mode);
    }
}
void gpio_pin_set_list_modes(uint8_t *pins, uint8_t npins, uint8_t mode) {
    for(uint8_t i = 0; i < npins; ++i) {
        uint8_t pin = pins[i];
        if(pin < MAX_PINS) gpio_pin_set_mode(pin, mode);
    }
}

// Get modes functions
uint8_t gpio_pin_get_mode(uint8_t pin) {
    uint8_t register_index = pin / PINS_PER_REGISTER;
    volatile unsigned int *gpfseln = gpfsel0 + register_index;
    uint8_t shift_position = 3 * (pin % PINS_PER_REGISTER);
    uint8_t mode = (*gpfseln) >> shift_position;
    return mode & 0x7;
}
uint8_t *gpio_pin_get_all_modes(uint8_t *modes) {
    for(uint8_t i = 0; i < MAX_PINS; ++i) {
        modes[i] = gpio_pin_get_mode(i);
    }
    return modes;
}
uint8_t *gpio_pin_get_list_modes(uint8_t *pins, uint8_t npins, uint8_t *modes) {
    for(uint8_t i = 0; i < npins; ++i) {
        uint8_t pin = pins[i];
        if(pin < MAX_PINS) modes[i] = gpio_pin_get_mode(pin); ;
    }
    return modes;
}

// Write functions
void gpio_write(uint8_t pin, uint16_t value) {
    uint8_t mode = gpio_pin_get_mode(pin);
    if(mode == OUTPUT) { // Value is 0 or 1
        uint8_t register_index = pin / 32;
        uint8_t index_in_register = pin % 32;
        volatile unsigned int *gpreg = (value & 0xFF) ? gpset0 : gpclr0;
        unsigned int mask = ~(1 << index_in_register);
        *(gpreg + register_index) = (*(gpreg + register_index)) & mask;
        *(gpreg + register_index) = *(gpreg + register_index) | (1 << index_in_register);
    } else if(gpio_is_pwm_pin(mode, pin)) { // PWM mode active
        uint8_t channel = gpio_pwm_channel(pin);
        if(channel) {
        	unsigned int pwmctl_;
            if(STOP_PWM){
            	pwmctl_ = *pwmctl; 
				gpio_pwm_stop();
            }
            gpio_pwm_range(channel, (unsigned int) ANALOG_ONE);
            gpio_pwm_data(channel, (unsigned int) value);
            if(STOP_PWM){
            	gpio_pwm_start();
    			*pwmctl = pwmctl_;
            }
        }
    }
}
void gpio_write_all(uint16_t value) {
    for(uint8_t i = 0; i < MAX_PINS; ++i) {
        gpio_write(i, value);
    }
}
void gpio_write_list(uint8_t *pins, uint8_t npins, uint16_t value) {
    for(uint8_t i = 0; i < npins; ++i) {
        uint8_t pin = pins[i];
        if(pin < MAX_PINS) gpio_write(pin, value);
    }
}

// Read functions
uint8_t gpio_read(uint8_t pin) {
    uint8_t mode = gpio_pin_get_mode(pin);
    uint8_t register_index = pin / 32;
    uint8_t index_in_register = pin % 32;
    volatile unsigned int *gplevn = gplev0 + register_index;
    return (*(gplevn) >> index_in_register) & 1;
}
uint8_t *gpio_read_all(uint8_t *values) {
    for(uint8_t i = 0; i < MAX_PINS; ++i) {
        values[i] = gpio_pin_get_mode(i);
    }
    return values;
}
uint8_t *gpio_read_list(uint8_t *pins, uint8_t npins, uint8_t *values) {
    for(uint8_t i = 0; i < npins; ++i) {
        uint8_t pin = pins[i];
        if(pin < MAX_PINS) values[i] = gpio_pin_get_mode(pin); ;
    }
    return values;
}

// PWM functions

uint8_t gpio_is_pwm_pin(uint8_t mode, uint8_t pin) {
    return (mode == ALT0 && (pin == 12 || pin == 13 || pin == 40 || pin == 41 || pin == 45))
           || (mode == ALT1 && (pin == 52 || pin == 53))
           || (mode == ALT5 && (pin == 18 || pin == 19));
}
uint8_t gpio_pwm_channel(uint8_t pin) {
    if(pin == 12 || pin == 18 || pin == 40 || pin == 52) {
        return 1;
    } else if(pin == 13 || pin == 19 || pin == 41 || pin == 45 || pin == 53) {
        return 2;
    } else {
        return 0;
    }
}

void gpio_pwm_stop() {
    if(DEBUG) printf("Stopping clock\n");
    *pwmctl = 0; // Turns off all pwm
    *pwmclkcntl = ((*pwmclkcntl) & ~0x10) | PWM_CLK_PASSWORD; // Sets enable bit to 0
    while(gpio_clock_busy()); // Waits for the busy flag
    if(DEBUG) printf("Clock stopped\n");
}
void gpio_pwm_start() {
    if(DEBUG) printf("Starting clock\n");
    *pwmclkcntl = (*pwmclkcntl | 0x10) | PWM_CLK_PASSWORD; // Sets enable bit to 0
    while(!gpio_clock_busy()); // Waits for the busy flag to be set
    if(DEBUG) printf("Clock started\n");
}
void set_pwm_mode(uint8_t channel, uint8_t ms_mode){ // 0 for PWM, 1 for MS
	if(channel == 1){
		if(ms_mode) *pwmctl |= 0x0081; // Channel 1 M/S mode, no FIFO, PWM mode, enabled
		else *pwmctl &= 0xFF01; // Channel 1 PWM algorithm mode, no FIFO, PWM mode, enabled
	}
	else if(channel == 2){
		if(ms_mode) *pwmctl |= 0x8100; // Channel 2 M/S mode, no FIFO, PWM mode, enabled
		else *pwmctl &= 0x01FF; // Channel 2 PWM algorithm mode, no FIFO, PWM mode, enabled
	}
	else{
		if(DEBUG) printf("Setting both channels modes\n");
		if(ms_mode) *pwmctl = 0x8181; // Both channels M/S mode, no FIFO, PWM mode, enabled
		else *pwmctl = 0x0101; // Both channels PWM algorithm mode, no FIFO, PWM mode, enabled
	}
	
}
/*Sources:
0 = GND, 1 = oscillator, 2 = testdebug0, 3 = testdebug1
4 = PLLA per, 5 = PLLC per, 6 = PLLD per, 7 = HDMI auxiliary, 8-15 = GND*/
void gpio_pwm_clock(uint8_t source, uint16_t divisor) { // Divisor goes from 0 to 2^12-1
    gpio_pwm_stop();
    if(DEBUG) printf("Setting the source to %d, divided by %d\n", source, divisor);
    // Source and division set, page 107
    divisor = divisor & 0xFFF; // gets last 12 bits
    unsigned int divisor_ = divisor << 12; // 12 bits from start are for floating division
    *pwmclkdiv = PWM_CLK_PASSWORD | (divisor_); // Sets the divisor to divisor_
    unsigned int source_ = source & 0xF; // 4 bits from source
    *pwmclkcntl = PWM_CLK_PASSWORD | (source_); // Sets source in control
    *pwmclkcntl = PWM_CLK_PASSWORD | (*pwmclkcntl) | (1 << 9); // Sets mash 1
    gpio_pwm_start();
    set_pwm_mode(0, 0); // Both channels in pwm
}

uint8_t gpio_clock_busy() {
    return (*pwmclkcntl) & 0x80;
}
pwm_status_t* gpio_pwm_get_status() {
    return pwm_status;
}

// Scale is not big enough, therefore is frequency is less than 500Khz, PWM range change is disabled and should use MS mode
void gpio_pwm_clock_freq(uint8_t channel, float frequency) {
    if(frequency <= 500 * 1000) { // If less than 500kHz, force MS
    	// Sets clock frequency for 500kHz
    	float real_frequency = 500*1000;
        uint16_t divisor = 1000;
        gpio_pwm_clock(6, divisor);
        pwm_status->current_frequency=real_frequency;
        unsigned int n = real_frequency/frequency; // Calculates T' = n*T, this is the range


        if(channel == 1){
        	pwm_status->forced_range1=1;
        	set_pwm_mode(1, 1); 					// Sets MS mode for channel 1
        	gpio_pwm_range(1, n); 					// Sets range for channel 1
        }
        else if(channel == 2){
        	pwm_status->forced_range2=1;
        	set_pwm_mode(2, 1);						// Sets MS mode for channel 2	
        	gpio_pwm_range(2, n); 					// Sets range for channel 2
        }
        else{
        	pwm_status->forced_range1=1;
        	pwm_status->forced_range2=1;
        	set_pwm_mode(0, 1); 					// Sets MS mode for both channels
        	gpio_pwm_range(1, n); 					// Sets range for channel 1
        	gpio_pwm_range(2, n); 					// Sets range for channel 2
        }

    } else {
        uint16_t divisor = PLLD_FREQ / frequency;
        gpio_pwm_clock(6, divisor);
        pwm_status->current_frequency=frequency;
        pwm_status->forced_range1=0;
		pwm_status->forced_range2=0;
    }
    
}
void gpio_pwm_range(uint8_t channel, unsigned int range) {
    if(DEBUG) printf("Setting range for channel %d as %u\n", channel, range);
    if(channel == 1) {
        *pwmrng1 = range;
        pwm_status->channel1_range=range;
    } else if(channel == 2) {
        *pwmrng2 = range;
        pwm_status->channel2_range=range;
    } else {
        printf("Range for wrong channel\n");
    }
}
void gpio_pwm_data(uint8_t channel, unsigned int data) {
    if(DEBUG) printf("Setting data for channel %d as %u\n", channel, data);
    if(channel == 1) {
        *pwmdat1 = data;
        pwm_status->channel1_data=data;
    } else if(channel == 2) {
        *pwmdat2 = data;
        pwm_status->channel2_data=data;
    } else {
        printf("Range for wrong channel\n");
    }
}
void gpio_pwm_duty_cycle(uint8_t channel, float cycle) {
	unsigned int pwmctl_ = *pwmctl; 
	gpio_pwm_stop();

    if(cycle > 1) cycle = 1;
    unsigned int s = 100000;
    if(channel == 1 && pwm_status->forced_range1) s = pwm_status->channel1_range;
    if(channel == 2 && pwm_status->forced_range2) s = pwm_status->channel2_range;
    unsigned int m = s * (cycle);
    if(s == 100000) gpio_pwm_range(channel, s);
    gpio_pwm_data(channel, m);

    gpio_pwm_start();
    *pwmctl = pwmctl_;
}