#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops
#include <unistd.h> //usleep

//Static base
static unsigned GPIO_BASE = 0x3f200000;

//Regs pointers
volatile unsigned int * gpfsel0;
volatile unsigned int * gpfsel1;

/*Function prototypes*/
void gpioInitPtrs();
void gpioSetMode();

//Initialize pointers: performs memory mapping, exits if mapping fails
void gpioInitPtrs(){
	int fd = -1;
	//Loading /dev/mem into a file
	if ((fd = open("/dev/mem", O_RDWR, 0)) == -1)
		err(1, "Error opening /dev/mem");
	//Mapping GPIO base physical address
	gpfsel0 = (unsigned int*)mmap(0, getpagesize(), PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
	//Check for mapping errors
	if (gpfsel0 == MAP_FAILED)
		errx(1, "Error during mapping GPIO");
	//Set regs pointers
    gpfsel1 = gpfsel0 + 0x1;
}

//Sets spi pin modes
void gpioSetMode(){
	*gpfsel0 = (*gpfsel0 & ~(0x7 << 7*3)) | (0x4 << 7*3); //GPIO7 (CS1) takes alternate fun 0
    *gpfsel0 = (*gpfsel0 & ~(0x7 << 8*3)) | (0x4 << 8*3); //GPIO8 (CS0) takes alternate fun 0
    *gpfsel0 = (*gpfsel0 & ~(0x7 << 9*3)) | (0x4 << 9*3); //GPIO9 (MISO) takes alternate fun 0
    *gpfsel1 = (*gpfsel1 & ~(0x7 << 0*3)) | (0x4 << 0*3); //GPIO10 (MOSI) takes alternate fun 0
    *gpfsel1 = (*gpfsel1 & ~(0x7 << 1*3)) | (0x4 << 1*3); //GPIO11 (SCLK) takes alternate fun 0
    printf("GPFSEL0 is %x\n", *gpfsel0);
    printf("GPFSEL1 is %x\n", *gpfsel1);
}

int main(int argc, char const *argv[]){
	gpioInitPtrs();
	gpioSetMode();
	return 0;
}
