#ifndef LIB_GPIO_H
#define LIB_GPIO_H

#include <stdint.h>

// Function selection values
#define INPUT 	0
#define OUTPUT 	1
#define ALT0 	4
#define ALT1 	5
#define ALT2 	6
#define ALT3 	7
#define ALT4 	3
#define ALT5 	2

#define PWM 	8

// Define LOW and HIGH for convenience
#define HIGH 	1
#define LOW 	0

// Define other constants
#define MAX_PINS 54
#define PINS_PER_REGISTER 10

#define GPIO_PERI_BASE  		0x3F000000
#define PWM_CLK_PASSWORD		0x5a000000
#define ANALOG_ONE 				0xFFFF
#define STOP_PWM 				1
#define OSCILLATOR_FREQ 		19200000
#define PLLD_FREQ 				500000000
#define DEBUG 					0

// Initialize pointers
void gpio_init();

typedef struct{
	float current_frequency;
	unsigned int channel1_range;
	unsigned int channel2_range;
	unsigned int channel1_data;
	unsigned int channel2_data;
	uint8_t forced_range1;
	uint8_t forced_range2;
}pwm_status_t;

void gpio_set_mode();

// Set modes functions
void gpio_pin_set_mode(uint8_t pin, uint8_t mode);
void gpio_pin_set_all_modes(uint8_t mode);
void gpio_pin_set_list_modes(uint8_t* pins, uint8_t npins, uint8_t mode);

// Get modes functions
uint8_t gpio_pin_get_mode(uint8_t pin);
uint8_t* gpio_pin_get_all_modes(uint8_t* modes);
uint8_t* gpio_pin_get_list_modes(uint8_t* pins, uint8_t npins, uint8_t* modes);

// Write functions
void gpio_write(uint8_t pin, uint16_t value);
void gpio_write_all(uint16_t value);
void gpio_write_list(uint8_t* pins, uint8_t npins, uint16_t value);

// Read functions
uint8_t gpio_read(uint8_t pin);
uint8_t* gpio_read_all(uint8_t* values);
uint8_t* gpio_read_list(uint8_t* pins, uint8_t npins, uint8_t* values);


// PWM functions
uint8_t gpio_is_pwm_pin(uint8_t mode, uint8_t pin);
uint8_t gpio_pwm_channel(uint8_t pin);
void gpio_pwm_stop();
void gpio_pwm_start();
void set_pwm_mode(uint8_t channel, uint8_t ms_mode);
void gpio_pwm_clock(uint8_t source, uint16_t divisor);
uint8_t gpio_clock_busy();
pwm_status_t* gpio_pwm_get_status();
void gpio_pwm_clock_freq(uint8_t channel, float frequency);
void gpio_pwm_range(uint8_t channel, unsigned int range);
void gpio_pwm_data(uint8_t channel, unsigned int data);
void gpio_pwm_duty_cycle(uint8_t channel, float cycle);
#endif