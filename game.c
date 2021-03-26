#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <bcm_host.h>

#define BLOCK_SIZE 4096

#define TIMER_OFFSET 0x00003000

#define GPIO_OFFSET 0x200000

volatile unsigned int *gpio;
volatile unsigned int *timer;
void *gpioMap;
int fdGPIO;
void *timerMap;
int fdTimer;


#define TX 9 //MISO -- output
#define RX 10 //MOSI --input 


#define GPFSEL0 0 
#define GPFSEL1 1 

#define GPSET0 7
#define GPSET1 8 

#define GPCLR0 10
#define GPCLR1 11 

#define GPLEV0 13
#define GPLEV1 14

#define GPPUD 37
#define GPPUDCLK0 38

#define CS      0
#define CLO     1
#define CHI     2
#define C0      3
#define C1      4
#define C2      5
#define C3      6

 
//From tutorial 05 about us100 and GPIO

void initSensor() {
    unsigned peripheralBase = bcm_host_get_peripheral_address();
    
    fdGPIO = open("/dev/mem", O_RDWR|O_SYNC);
    gpioMap = (unsigned int *)mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fdGPIO,
        peripheralBase + GPIO_OFFSET
    );
    
    if ( gpioMap == MAP_FAILED ) {
        fprintf( stderr, "The memory map initialization failed.\n");
        perror( "mmap" );
        return;
    }
    
    gpio = (volatile unsigned int *) gpioMap;

    register unsigned int r;
    
    //Configures GPIO pin 9 to output
    r = gpio[GPFSEL0];
    r &=  ~(0x7 << (9*3));
    r |= (0x1 << (9*3));
    gpio[GPFSEL0] = r;
    
    //Configure GPIO 10 to input 
    r = gpio[GPFSEL1];
    r &= ~(0x7); //Clear the least significant bit in GPFSEL1
    gpio[GPFSEL1] = r; 
    
    // Disable pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register 
    gpio[GPPUD] = 0x0;
    r = 150;
    while (r--) {
      asm volatile("nop");
    }
    gpio[GPPUDCLK0] = (0x1 << TX);
    r = 150;
    while (r--) {
      asm volatile("nop");
    }
    gpio[GPPUDCLK0] = 0;
    
    // clearing the output line
    gpio[GPCLR0] = 1 << TX;
}

//From tutorial 05 about us100 and GPIO
void freeSensor() {
    munmap( gpioMap, BLOCK_SIZE );
    close( fdGPIO );
}
//From tutorial 05 about us100 and GPIO
void setTX() {
    register unsigned int r; 
    // Put a 1 into SET9 field of the GPOI Pin output set register to 0 
    r = (0x1 << 9); 
    gpio[GPSET0] = r;
    
}
//From tutorial 05 about us100 and GPIO
void clearTX() {
    register unsigned int r; 
    
    r = (0x1 << 9);
    gpio[GPCLR0] = r;
}

//From tutorial 05 about us100 and GPIO
int readRX() {
    register unsigned int r  = gpio[GPLEV0]; 
    r = ((r >> RX) & 0x1); 
    return r;
    
}

//Code is take from lecture about RPi Microsecond timer
void initTimer() {
    unsigned peripheralBase = bcm_host_get_peripheral_address();
    
    fdTimer = open("/dev/mem", O_RDWR|O_SYNC);
    timerMap = (unsigned int *)mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        fdTimer,
        peripheralBase + TIMER_OFFSET
    );
    
    if ( timerMap == MAP_FAILED ) {
        perror( "mmap" );
        return;
    }
    
    timer = (volatile unsigned int *) timerMap;

    fprintf( stderr, "mmap successful: %p\n", timer );
}
//Code is take from lecture about RPi Microsecond timer
void freeTimer() {
    munmap( timerMap, BLOCK_SIZE );
    close( fdTimer );
}

unsigned long long getSystemTimerCounter() {
 // from  https://embedded-xinu.readthedocs.io/en/latest/arm/rpi/BCM2835-System-Timer.html

    unsigned int h=-1, l;
    
    // we must read MMIO area as two separate 32 bit reads
    h = timer[CHI];
    l = timer[CLO];

    // we have to repeat it if high word changed during read
    //   - low low counter rolled over
    if ( h != timer[CHI] ) {
        h = timer[CHI];
        l = timer[CLO];
     }
    // compose long long int value
    return ((unsigned long long) h << 32) | (unsigned long long)l;
}

