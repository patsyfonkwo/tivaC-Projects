

/**
 * main.c
 */

/*

 *  PF1 --> LED_RED
 *  PF2 --> LED_BLUE
 *  PF3 --> LED_GREEN

 *  GPIO Data Port F (APB) base           : 0x4002.5000
 *  GPIO Data Offset                      : 0x000 --> 0x4002.5000
 *  LED_BLUE                              : PF2 [GPIODATA]

 *  GPIO Direction Port F (APB) base      : 0x4002.5000
 *  GPIO Direction Offset                 : 0x400 --> 0x4002.5400
 *  Set to output                         : 1 [GPIODIR]

 *  GPIO Digital Enable Port F (APB) base : 0x4002.5000
 *  GPIO Digital Enable Offset            : 0x51C --> 0x4002.551C
 *  Set to digital                        : 1 [GPIODEN]

 *  GPIO Clock Enable Port F Base         : 0x400F.E000 (R5 bit)
 *  GPIO Clock Enable Offset              : 0x608 --> 0x400F.E608
 *  Enable and provide clock in Run mode  : 1 [RCGCGPIO]

*/


// Define Clock
# define RCGCGPIO (* (volatile unsigned long *) (0x400FE608))

// Define GPIO Data F
# define GPIODATA (* (volatile unsigned long *) (0x40025010))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0000 0001 0000 --> 0x010 (0s remain unchanged)

// Define GPIO Direction F
# define GPIODIR (* (volatile unsigned long *) (0x40025400))

// Define GPIO Digital Enable F
# define GPIODEN (* (volatile unsigned long *) (0x4002551C))



int main(void)
{
    RCGCGPIO = 0x20;
    GPIODIR |= 0x04; //PF2 as output
    GPIODEN = 0x04; // PF2 DEN
    GPIODATA = 0x04; // PF2 (LED_BLUE) high

    int i = 0;

    while (1){
        GPIODATA = 0x04; // PF2 (LED_BLUE) high
        i = 0;
        while (i < 700000){
            ++i;
        } // wait

        GPIODATA = 0x00; // PF2 (LED_BLUE) low
        i = 0;
        while (i < 700000){
            ++i;
        } // wait
    }

}
