
/**
 * main.c
 */



// UART0 base: 0x4000.C000 (bits [7:0])

// UARTIBRD (integer baud rate divisor)

// define UART Clock Control Register
# define RCGCUART (* (volatile unsigned long *) (0x400FE618))
// bit 0 controls clock for UART0(UART0-UART7)

// define UART Control Register
# define UARTCTL (* (volatile unsigned long *) (0x4000C030))
// Set UARTEN (bit 0) high to enable UART (set to 0 while configuring UART)
// Set TXE (bit 8) high to enable transmitting data
// Set RXE (bit 9) high to enable receiving data
// During Configuration: FFFF.FFFF.FFF0
// After Configuration:
//      Only Transmit: FF01.FFFF.FFF1 --> 0001.0000.0001 --> 0x101
//      Only Receive: FF10.FFFF.FFF1 --> 0010.0000.0001 --> 0x201
//      Both: FF11.FFFF.FFF1 --> 0011.0000.0001 --> 0x301

// define UART Line Control Register
# define UARTLCRH (* (volatile unsigned long *) (0x4000C02C))
// BRK (bit 0) sends 0 to break communication
// PEN (bit 1) parity enable
// STP2 (bit 3) stop bits, *0 = 1 stop bit, 1 = 2 stop bits
// WLEN (bits 5 and 6) length of data frame (0x0 = 5, 0x1 = 6, 0x2 = 7, *0x3 = 8 bits)
// for bits 5 and 6, 0x3 is F110.0F11 --> 0110.0011 --> 0x63

// define UART0 Data Register: 0x4000.C000
# define UARTDR (* (volatile unsigned long *) (0x4000C000)) // transmitted data

// define UART0 Flag Register:
/* contains transmit/receive status information
   Indicates whether the UART is in the middle of a
   transmit/receive */
# define UARTFR (* (volatile unsigned long *) (0x4000C018))

// define UART0 Integer/Fractional Baud Rate Registers
// BRD = 1MHz / Baud Rate (which is 9600) == 104 2/3
# define UARTIBRD (* (volatile unsigned long *) (0x4000C024)) // 8 bits long, 0 - 255
# define UARTFBRD (* (volatile unsigned long *) (0x4000C028)) // 6 bits long, 0 – 0.984375



// (* (volatile unsigned long *) ())
// set corresponding afsel/pdr bits to 1 to "commit" changes to these registers
# define GPIOCR_A (* (volatile unsigned long *) (0x40004524))
# define GPIOCR_B (* (volatile unsigned long *) (0x40005524))

// Define Clock
# define RCGCGPIO (* (volatile unsigned long *) (0x400FE608)) // READ DATA SHEET
// Define GPIO Data A
# define GPIODATA_A (* (volatile unsigned long *) (0x40004000))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0000 0001 0000 --> 0x010 (0s remain unchanged)
// Define GPIO Data A for UART
# define GPIODATA_B (* (volatile unsigned long *) (0x40005000))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0000 0011 0000 --> 0x008 (0s remain unchanged)
// Define GPIO Data B for pull-up/down
# define GPIODATA_C (* (volatile unsigned long *) (0x40006040))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0000 0100 0000 --> 0x040 (0s remain unchanged)

# define GPIOPUR_A (* (volatile unsigned long *) (0x40004510))
// pin's pull up register is enabled at 1, disabled at 0
# define GPIOPDR_A (* (volatile unsigned long *) (0x40004514))
// pin's pull down register is enabled at 1, disabled at 0
# define GPIOPUR_B (* (volatile unsigned long *) (0x40005510))
// pin's pull up register is enabled at 1, disabled at 0
# define GPIOPDR_B (* (volatile unsigned long *) (0x40005514))
// pin's pull down register is enabled at 1, disabled at 0

// Define GPIO Direction A (UART0 in this port)
# define GPIODIR_A (* (volatile unsigned long *) (0x40004400))
// Define GPIO Direction B (input? should this even be set?)
# define GPIODIR_B (* (volatile unsigned long *) (0x40005400))
# define GPIODIR_C (* (volatile unsigned long *) (0x40006400))

// Define UART0 GPIO Digital Enable
# define GPIODEN_A (* (volatile unsigned long *) (0x4000451C))
// Set bit 1
// PA0 pin 17 - U0Rx
// PA1 pin 18 - U0Tx
# define GPIODEN_B (* (volatile unsigned long *) (0x4000551C))
# define GPIODEN_C (* (volatile unsigned long *) (0x4000651C))

// Define UART0 GPIO Alternate Select
# define GPIOAFSEL_A (* (volatile unsigned long *) (0x40004420))
// PA0 and PA1 set to 1
# define GPIOPCTL_A (* (volatile unsigned long *) (0x4000452C))
// 0x1

# define POSITIVE 1
# define NEGATIVE 0
# define BRDF 1/6

int mode = POSITIVE; // set default to positive

void initialize(){
    // initialize register values

    RCGCUART = (1 << 0);
    while ((* (volatile unsigned long *) (0x400FE000)) & 0x01 == 0);
    RCGCGPIO |= (1 << 0); // A
    UARTCTL &= ~(1 << 0);
    GPIODEN_A |= (1 << 0)|(1 << 1);
    GPIOAFSEL_A |= (1 << 0)|(1 << 1);
    GPIOPUR_A &= ~(0x03); // disable pull up
    GPIOPDR_A &= ~(0x03); // disable pull down
    GPIODIR_A &= ~(0 << 0); GPIODIR_A |= (1 << 1);
    GPIOPCTL_A = 0x11;
    UARTIBRD = 0x68; // (104); //hopefully this works: BRD = BRDI + BRDF = UARTSysClk / (ClkDiv * Baud Rate)
    UARTFBRD = 0xB; // (int)(BRDF * 64.0 + 0.5); // hopefully this works: UARTFBRD[DIVFRAC] = integer(BRDF * 64 + 0.5)
    // 0110 0000 or 0110 0(Even Parity)(Parity Enable)0
    UARTLCRH |= 0x60;
    UARTCTL |= (1 << 8)|(1 << 9)|(1 << 0); // TXE|RXE|Enable

    RCGCGPIO |= (1 << 1)|(1 << 2); // B|C;



//    GPIODATA_B

    GPIOPUR_B |= (1 << 2);
    GPIOPDR_B |= (1 << 3);
    // pin's pull down register is enabled at 1, disabled at 0


    GPIODIR_B &= ~(1 << 2); // input
//    GPIODIR_B |= (1 << 2); // output


    GPIODEN_B |= (1 << 2);


    GPIODIR_C |= (1 << 4);
    GPIODEN_C |= (1 << 4);

    GPIOCR_A |= (1 << 0)|(1 << 1);
    GPIOCR_B |= (1 << 2);

}


void UART_Tx (char data) {
    // PRINT CHAR
    while((UARTFR & 0x20) != 0); // while bit 5 of UARTFR is high (transmit/FIFO full)
    UARTDR = data;
    //
}

char UART_Rx () {
    // READ CHAR
    char data;
    // this is a wait loop (acts like bitwise AND)
    while((UARTFR & 0x10) != 0); // while bit 4 of UARTFR is high (receive/FIFO empty)
    // ^ waiting for user to enter something into the data register
    data = UARTDR;
    return ((unsigned char) data);
}



void positive(){
    // run pull up logic version
    // I believe this switch off happens automatically but it's cool
    // GPIOPDR_B &= (0 << 2); // disable pull down register
//    GPIODIR_B |= (1 << 2); // output
//    GPIODIR_B &= ~(1 << 2); // input
    GPIOPUR_B |= (1 << 2); // enable pull up register
    GPIOPDR_B |= (1 << 3); // this happens automatically
    GPIOCR_B |= (1 << 2)|(1 << 3);
}

void negative(){
    // run pull down logic version
    // I believe this switch off happens automatically but it's cool
//    GPIOPUR_B &= ~(1 << 2);// disable pull up register
//    GPIOCR_B |= (1 << 2);
//    GPIODIR_B |= (1 << 2); // output
//    GPIODIR_B &= ~(1 << 2); // input
    GPIOPDR_B |= (1 << 2); // enable pull down register
    GPIOPUR_B |= (1 << 3); // this happens automatically
    GPIOCR_B |= (1 << 2)|(1 << 3);
}



int main(void)
{
    initialize();
    int i = 0;
    char read_val;
    while (i < 417)
        ++i;
//    unsigned char Rx;
//    UART_Tx('p');

    while (1){


//        UART_Tx('P'); // sends "P" to data reg and prints in console
//        UART_Tx('a');
//        UART_Tx('t');
//        UART_Tx('s');
//        UART_Tx('y');
//        UART_Tx('\n');
//        GPIODATA_C |= (1 << 4);

        switch(mode){
            case POSITIVE:
                positive(); // positive case
                read_val = UART_Rx();
                UART_Tx(read_val); // echo
                if (read_val == 'n'){
                    mode = NEGATIVE;
                    break;
                } else if (read_val == 'p'){
                    mode = POSITIVE;
                    break;
                }
            case NEGATIVE:
                negative(); // negative case
                read_val = UART_Rx();
                UART_Tx(read_val); // echo
                if (read_val == 'p'){
                    mode = POSITIVE;
                    break;
                } else if (read_val == 'n'){
                    mode = NEGATIVE;
                    break;
                }
        }
    }


    return 0;
}
