/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_from/to_user */
#include <linux/ioport.h>
#include <asm/io.h>


/* Bitfields in CS */
#define BCM2835_SPI_CS_LEN_LONG     0x02000000
#define BCM2835_SPI_CS_DMA_LEN      0x01000000
#define BCM2835_SPI_CS_CSPOL2       0x00800000
#define BCM2835_SPI_CS_CSPOL1       0x00400000
#define BCM2835_SPI_CS_CSPOL0       0x00200000
#define BCM2835_SPI_CS_RXF      0x00100000
#define BCM2835_SPI_CS_RXR      0x00080000
#define BCM2835_SPI_CS_TXD      0x00040000
#define BCM2835_SPI_CS_RXD      0x00020000
#define BCM2835_SPI_CS_DONE     0x00010000
#define BCM2835_SPI_CS_LEN      0x00002000
#define BCM2835_SPI_CS_REN      0x00001000
#define BCM2835_SPI_CS_ADCS     0x00000800
#define BCM2835_SPI_CS_INTR     0x00000400
#define BCM2835_SPI_CS_INTD     0x00000200
#define BCM2835_SPI_CS_DMAEN        0x00000100
#define BCM2835_SPI_CS_TA       0x00000080
#define BCM2835_SPI_CS_CSPOL        0x00000040
#define BCM2835_SPI_CS_CLEAR_RX     0x00000020
#define BCM2835_SPI_CS_CLEAR_TX     0x00000010
#define BCM2835_SPI_CS_CPOL     0x00000008
#define BCM2835_SPI_CS_CPHA     0x00000004
#define BCM2835_SPI_CS_CS_10        0x00000002
#define BCM2835_SPI_CS_CS_01        0x00000001


/*Local defined macros*/
#define BUF_LEN 32
#define DEVICE_NAME "spi"
#define SUCCESS 0
static unsigned RANGE = 0x18;
static unsigned SPI_BASE = 0x3f204000;

static char msg[BUF_LEN]; /* The msg the device will give when asked */
static char *msg_Ptr;
static int Device_Open = 0; /* Is device open? */
u8 *addr;
MODULE_LICENSE("GPL v2");
/* Declaration of spi.c functions */
int spi_open(struct inode *inode, struct file *filp);
int spi_release(struct inode *inode, struct file *file);
ssize_t spi_read(struct file *filp, char *buffer, size_t length, loff_t *offset);
ssize_t spi_write(struct file *filp, const char *buffer, size_t length, loff_t *offset);
void spi_exit(void);
int spi_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations timer_fops = {
read:
    spi_read,
write:
    spi_write,
open:
    spi_open,
release:
    spi_release
};
/* Declaration of the init and exit functions */
module_init(spi_init);
module_exit(spi_exit);
/* Global variables of the driver */
/* Major number */
int spi_major = 60;

/*********Functions*********/
/* Device init*/
int spi_init(void) {
    int result;
    /* Registering device */
    result = register_chrdev(spi_major, DEVICE_NAME, &timer_fops);
    if (result < 0) {
        printk("<1>spi: cannot obtain major number %d\n", spi_major);
        return result;
    }
    /*Reserve spi memory region */
    if(request_mem_region(SPI_BASE, RANGE, DEVICE_NAME) == NULL) {
        printk("<1>spi: cannot reserve memory \n");
        unregister_chrdev(spi_major, DEVICE_NAME);
        return -ENODEV;
    }
    printk("<1>Inserting spi module... :)\n");

    /*Generate new address required for iowrite and ioread methods*/
    addr = ioremap(SPI_BASE, RANGE);

    /* Set clock frequency to 1MHz */
    u32 cmd = 0x000004B; //600
    
    /*Write to spi CLK Register*/
    iowrite32(cmd, (addr + 0x8));

    cmd = ioread32(addr);
    cmd = cmd | BCM2835_SPI_CS_CPOL | BCM2835_SPI_CS_CPHA | BCM2835_SPI_CS_CLEAR_TX | BCM2835_SPI_CS_CLEAR_RX;
    
    iowrite32(cmd, addr);

    /*Check it has been configured*/
    printk("spi: SPI control set to 0x%08x\n", ioread32(addr));
    u32 cdiv = ioread32(addr + 0x8);
    printk("spi: SPI CLK set to %dMHz\n", 600 / cdiv);
}

void spi_exit(void) {
    /* Freeing the major number and memory space */
    unregister_chrdev(spi_major, DEVICE_NAME);
    release_mem_region(SPI_BASE, RANGE);

    printk("<1>Removing spi module\n");
}

int spi_open(struct inode *inode, struct file *filp) {
    printk("<1>Opening spi module\n");
    /*Avoid reading conflicts*/
    if (Device_Open)
        return -EBUSY;
    Device_Open++;
    return SUCCESS;
}

int spi_release(struct inode *inode, struct file *file) {
    printk("<1>Releasing spi module...\n");
    Device_Open--; //make ready for the next caller*/
    return SUCCESS;
}

ssize_t spi_write(struct file *filp, const char *buffer, size_t length, loff_t *offset) {
    printk("<1>Writing to spi module...\n");
    u32 cmd;
    if (buffer) {
        u32 status = ioread32(addr);

        cmd = status | BCM2835_SPI_CS_TA; // | BCM2835_SPI_CS_INTD | BCM2835_SPI_CS_INTR;
	    /*Write to spi control Register*/
	    iowrite32(cmd, addr);

        status = ioread32(addr);

        //printk("spi: SPI control before write is 0x%08x\n", status);

        while (!(ioread32(addr) & BCM2835_SPI_CS_TXD)) {
        }

        //if ((status & BCM2835_SPI_CS_TXD) != 0) {
        	//printk("spi: Queue not full\n");
            //cmd = 0x48454848;
            if (buffer[0] == '-') {
                iowrite32(0x0A, (addr + 0x4));
            }
            iowrite32(buffer[0], (addr + 0x4));
        //}

        status = ioread32(addr);
        //printk("spi: SPI control after write is 0x%08x\n", status);

        /*if (status & BCM2835_SPI_CS_DONE != 0) {
            //Set TA = 0
            cmd = !(BCM2835_SPI_CS_TA) & status;
            iowrite32(cmd, addr);
        }*/

        //status = ioread32(addr);
        //printk("spi: SPI test 0x%08x\n", status & BCM2835_SPI_CS_DONE);
        //printk("spi: SPI control before RXD Poll is 0x%08x\n", status);
        u32 recieved;

        //Read Data if RXD == 1
        /*while ((status & BCM2835_SPI_CS_RXD) != 0) {
            recieved = ioread32(addr+0x4);
            printk("spi: SPI data recieved is 0x%08x\n", recieved);
            status = ioread32(addr);
        }*/

        status = ioread32(addr);
        //printk("spi: SPI control before DONE Poll is 0x%08x\n", status);

        while ((ioread32(addr) & BCM2835_SPI_CS_DONE) == 0) {
            /*while ((status & BCM2835_SPI_CS_RXD) != 0) {
                recieved = ioread32(addr+0x4);
                printk("spi: SPI data recieved is 0x%08x\n", recieved);
                status = ioread32(addr);
            }*/
        }
        //printk("spi: SPI control after while is 0x%08x\n", status);

        /*if ((status & BCM2835_SPI_CS_RXR) != 0) {
            printk("spi: RXR is set\n");
            
            ioread32(addr);
            ioread32(addr);
            ioread32(addr);
        }*/

        //Set TA = 0
        cmd = !(BCM2835_SPI_CS_TA) & status;
        iowrite32(cmd, addr);

    }
    /*Buffer contains only 1 char (0-1)
    returns "1 byte written"*/
    return 1;
}

ssize_t spi_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
    printk("spi: reading\n");
    u32 res;
    msg_Ptr = msg;
    int nbytes = 8;
    if (*offset == 0) {
        u32 status = ioread32(addr);
        if ((status & BCM2835_SPI_CS_RXD) != 0) {
            res = ioread32(addr + 0x4);
            /*Mask to get bytes of message*/
            msg[0] = res & 0xFF;
            msg[1] = (res >> 8) & 0xFF;
            msg[2] = (res >> 16) & 0xFF;
            msg[3] = (res >> 24) & 0xFF;
            /*Convert to Hex ASCII */
            //sprintf(msg, "%08hhx", res);
            /*Copy to user buffer*/
            copy_to_user(buffer, msg, nbytes);
            //*offset += 1;
            //printk("spi: RXD = %d \n", res);
            return nbytes;
        }

        //printk("spi: None data to read\n", res);
        return SUCCESS;
    }
    //printk("spi: offset is %d\n", *offset);
    return SUCCESS;
}