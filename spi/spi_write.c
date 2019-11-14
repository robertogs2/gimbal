#include <stdio.h>
#include <stdint.h>
#include <time.h>

char read_buff1[4];
char read_buff2[4];
char read_buff3[4];
char read_buff4[4];

#define BUFFSIZE 50

char buffstr[BUFFSIZE];

void writeDriver(uint8_t data, FILE *fp){
	fp = fopen("/dev/spi", "r+");
	fputc(data, fp);
	fclose(fp);
}

void readDriver(char* buff, FILE *fp){
	fp = fopen("/dev/spi", "r+");
	fgets(buff, 4, fp);
	fclose(fp);
}

void readData(uint8_t data, char* buff, FILE *fp) {
	fp = fopen("/dev/spi", "r+");
	fputc(data, fp);
	fgets(buff, 4, fp);
	fclose(fp);
}

void printBuff(){
	printf("0x%02x%02x%02x%02x\n", read_buff1[3], read_buff1[2], read_buff1[1], read_buff1[0]);
}

int main(int argc, char const *argv[]){

	struct timespec ts;

	ts.tv_sec = 0.1;
	ts.tv_nsec = 100000000;

	uint8_t MPU_Init_Data[17][2] = {
        {0x80, 0x6b},        // Reset Device
        {0x01, 0x6b},               // Clock Source
        {0x00, 0x6c},               // Enable Acc & Gyro
        {0x01, 0x1a},     // Use DLPF set Gyroscope bandwidth 184Hz, temperature bandwidth 188Hz
        {0x00, 0x1b},    // +-250dps
        {0x00, 0x1c},       // +-2G
        {0x01, 0x1d}, // Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz
        {0x12, 0x37},      //
        {0x30, 0x6a},        // I2C Master mode and set I2C_IF_DIS to disable slave mode I2C bus
        {0x0D, 0x24},     // I2C configuration multi-master  IIC 400KHz
        {0x0c, 0x25},  // Set the I2C slave addres of AK8963 and set for write.
        {0x0b, 0x26}, // I2C slave 0 register address from where to begin data transfer
        {0x01, 0x63},   // Reset AK8963
        {0x81, 0x27}, // Enable I2C and set 1 byte
        {0x04, 0x26}, // I2C slave 0 register address from where to begin data transfer
        {0x12, 0x63},   // Register value to 8Hz continuous measurement in 16bit
        {0x81, 0x27}  //Enable I2C and set 1 byte
        
    };

	FILE *fp;
	//if (fp != NULL) {

		/*for(int i = 0; i < 17; ++i) {
			writeDriver(MPU_Init_Data[i][1], fp);
			writeDriver(MPU_Init_Data[i][0], fp);
			readDriver(read_buff1, fp);
			printBuff();
			readDriver(read_buff1, fp);
			printBuff();
			nanosleep(&ts, &ts);
	        //fputc(MPU_Init_Data[i][0], fp);
	        //fgets(read_buff, 4, fp);
	    }*/
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
		}

		printf("%s\n", buffstr);
	}

		//printf("0x%02x%02x%02x%02x\n", read_buff2[3], read_buff2[2], read_buff2[1], read_buff2[0]);
		
		//printf("0x%02x%02x%02x%02x\n", read_buff3[3], read_buff3[2], read_buff3[1], read_buff3[0]);
		
		//printf("0x%02x%02x%02x%02x\n", read_buff4[3], read_buff4[2], read_buff4[1], read_buff4[0]);
	//}
	return 0;
}
