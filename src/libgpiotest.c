#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops
#include <unistd.h> //usleep
#include <libgpio.h>
#include <stdio.h>
#include <libservo.h>
// To use pwm, set a clock to a channel, set a pin from that channel as PWM 
// Set whatever duty cycle you want as a fraction from 0 to 1, example to blink 5s  on PIN 12:
// gpio_init();
// gpio_pwm_clock_freq(1, 0.1) 		// 0.1Hz clock on channel 1
// gpio_pwm_duty_cycle(1, 0.5);		// 50% duty cycle on channel 1
// gpio_pin_set_mode(12, PWM); 		// 12 PIN in mode PWM

// The servo library is a wrapper for servos that uses gpio library, here is an example
int main(int argc, char const *argv[]) {
    gpio_init();
    init_servo(12);
    init_servo(13);
    float angle = 0;
    while(1){
    	printf("Enter angle: ");
		scanf("%f", &angle);
		write_servo(12, angle);
		write_servo(13, angle);
    }


    return 0;
}
