#ifndef LIB_SERVO_H
#define LIB_SERVO_H

#include <libgpio.h>
#include <unistd.h> //usleep

#define max_ms 2400
#define min_ms 544
#define servo_frequency 50

float angle_to_microseconds(float angle){
	if(angle < 0) angle = 0;
	if(angle > 180) angle = 180;
	return angle*(max_ms-min_ms)/180 + min_ms;
}

float microseconds_to_duty_cycle(float frequency, float microseconds){
	float period_micros = 1000*1000/frequency;
	return microseconds/period_micros;
}

float angle_to_duty_cycle(float angle, float frequency){
	float micros = angle_to_microseconds(angle);
	return microseconds_to_duty_cycle(frequency, micros);
}

uint8_t init_servo(uint8_t pin){
	gpio_init(); // Doesnt matter if it started already
	uint8_t channel = gpio_pwm_channel(pin);
	if(channel){
		gpio_pwm_clock_freq(channel, servo_frequency); 	// 50Hz clock
		gpio_pwm_duty_cycle(channel, 0.01);		// 1% duty cycle on channel, disabling it
		gpio_pin_set_mode(pin, PWM); 		// 12 PIN in mode PWM
		usleep(10000);
		return 0;
	}
	return 1;
}

uint8_t write_servo(uint8_t pin, float angle){
	uint8_t channel = gpio_pwm_channel(pin);
	if(channel){
		float duty_cycle = angle_to_duty_cycle(angle, servo_frequency);
		gpio_pwm_duty_cycle(channel, duty_cycle);
		return 0;
	}
	return 1;	
}

#endif