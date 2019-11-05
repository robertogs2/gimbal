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

// Define LOW and HIGH for convenience
#define HIGH 	1
#define LOW 	0

// Define other constants
#define MAX_PINS 54
#define PINS_PER_REGISTER 10

// Initialize pointers
void gpio_init();

// Set modes functions
void gpio_pin_set_mode(uint8_t pin, uint8_t mode);
void gpio_pin_set_all_modes(uint8_t mode);
void gpio_pin_set_list_modes(uint8_t* pins, uint8_t npins, uint8_t mode);

// Get modes functions
uint8_t gpio_pin_get_mode(uint8_t pin);
uint8_t* gpio_pin_get_all_modes(uint8_t* modes);
uint8_t* gpio_pin_get_list_modes(uint8_t* pins, uint8_t npins, uint8_t* modes);

// Write functions
void gpio_write(uint8_t pin, uint8_t value);
void gpio_write_all(uint8_t value);
void gpio_write_list(uint8_t* pins, uint8_t npins, uint8_t value);

// Read functions
uint8_t gpio_read(uint8_t pin);
uint8_t* gpio_read_all(uint8_t* values);
uint8_t* gpio_read_list(uint8_t* pins, uint8_t npins, uint8_t* values);

#endif