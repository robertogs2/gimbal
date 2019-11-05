#include <sys/mman.h> //mmap
#include <err.h> //error handling
#include <fcntl.h> //file ops
#include <unistd.h> //usleep
#include <libgpio.h>
int main(int argc, char const *argv[]) {
    gpio_init();
    
    gpio_pin_set_mode(2, OUTPUT);

    return 0;
}