#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops
#include <unistd.h> //usleep
#include <libgpio.h>
#include <stdio.h>
#include <libservo.h>
#include <stdlib.h>     /* atof */
// To use pwm, set a clock to a channel, set a pin from that channel as PWM 
// Set whatever duty cycle you want as a fraction from 0 to 1, example to blink 5s  on PIN 12:
// gpio_init();
// gpio_pwm_clock_freq(1, 0.1) 		// 0.1Hz clock on channel 1
// gpio_pwm_duty_cycle(1, 0.5);		// 50% duty cycle on channel 1
// gpio_pin_set_mode(12, PWM); 		// 12 PIN in mode PWM

// The servo library is a wrapper for servos that uses gpio library, here is an example

char read_buff1[4];
char buffer[128];
#define BUFFSIZE 50

#define FILTER_BUFFER_SIZE 25

char buffstr[BUFFSIZE];

float filter_pitch[FILTER_BUFFER_SIZE];
float filter_roll[FILTER_BUFFER_SIZE];

float pitch_mpu = 0;
float roll_mpu = 0;

float pitch_servo = 120;
float roll_servo = 90;

int count_fpitch = 0;
int count_froll = 0;

void readData(uint8_t data, char* buff, FILE *fp) {
    fp = fopen("/dev/spi", "r+");
    fputc(data, fp);
    fgets(buff, 4, fp);
    fclose(fp);
}

float filter(float val, float* filter_buff, int *count) {
    float sum = 0;
    filter_buff[*count] = val;
    *count = (*count + 1)%FILTER_BUFFER_SIZE;
    for (int i = 0; i < FILTER_BUFFER_SIZE; ++i) {
        sum += filter_buff[i];
    }
    return sum/FILTER_BUFFER_SIZE;
}

void process_string(char* input){
    int i = 0;
    float f1 = 0;
    float f2 = 0;
    while(input[i] && input[i++] != '*');// Fake parsing
    int ii = 0;
    // First number parsing
    while(input[i] && input[i] != ':'){
        buffer[ii++] = input[i];
        i++;
    }
    i++;
    buffer[ii] = 0;
    f1 = atof(buffer);
    ii=0;
    // Second number parsing
    while(input[i]){
        buffer[ii++] = input[i];
        i++;
    }
    buffer[ii] = 0;
    f2 = atof(buffer);

    pitch_mpu = filter(f1, filter_pitch, &count_fpitch);
    roll_mpu = filter(f2, filter_roll, &count_froll);

    //printf("%f : %f\n", pitch_mpu, roll_mpu);
}

int main(int argc, char const *argv[]) {
    gpio_init();
    gpio_set_mode();

    init_servo(13);
    init_servo(12);
    write_servo(12, pitch_servo); //Pitch Servo
    write_servo(13, roll_servo); //Roll Servo
  //   float angle = 0;
  //   while(1){
  //   	printf("Enter angle: ");
		// scanf("%f", &angle);
		// write_servo(12, angle);
		// write_servo(13, angle);
  //   }
  //   return 0;
    FILE *fp;
    while(1) {

        readData(0x31, read_buff1, fp);
        //printBuff();

        int count = 0;

        while(1) {
            //writeDriver(0x30, fp);
            readData(0x30, read_buff1, fp);
            if (read_buff1[0] == 0x0A | count == BUFFSIZE-1){
                buffstr[count] = '\0';
                break;
            } else {
                buffstr[count++] = read_buff1[0];
            }
            usleep(100);
        }

        //printf("%s\n", buffstr);
        // Process that string
        // Process string
        process_string(buffstr);

        write_servo(12, pitch_servo-1.25*pitch_mpu);
        write_servo(13, roll_servo-1.25*roll_mpu);
    }

    return 0;
}
