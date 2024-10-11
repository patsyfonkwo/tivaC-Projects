
/**
 * main.c
 */


// ------------------ UART STUFF ------------------
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

# define BRDF 1/6


// ------------------ GPIO STUFF ------------------
// Define GPIO Clock Control Register
# define RCGCGPIO (* (volatile unsigned long *) (0x400FE608)) // READ DATA SHEET
// Define GPIO Data A
# define GPIODATA_A (* (volatile unsigned long *) (0x40004000))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0000 0001 0000 --> 0x010 (0s remain unchanged)
// Define GPIO Data A for UART
# define GPIODATA_B (* (volatile unsigned long *) (0x40005030))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0000 0011 0000 --> 0x030 (0s remain unchanged)
// Define GPIO Data B for pull-up/down
# define GPIODATA_C (* (volatile unsigned long *) (0x400063C0))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0011 1100 0000 --> 0x3C0 (0s remain unchanged)
// Columns
# define GPIODATA_D (* (volatile unsigned long *) (0x4000703C))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0000 0011 1100 --> 0x03C (0s remain unchanged)
// Rows
# define GPIODATA_F (* (volatile unsigned long *) (0x400253FC))
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
# define GPIOPUR_F (* (volatile unsigned long *) (0x40025510))
// pin's pull up register is enabled at 1, disabled at 0
# define GPIOPDR_F (* (volatile unsigned long *) (0x40025514))
// pin's pull down register is enabled at 1, disabled at 0

// Define GPIO Direction A (UART0 in this port)
# define GPIODIR_A (* (volatile unsigned long *) (0x40004400))
// Define GPIO Direction B (input? should this even be set?)
# define GPIODIR_B (* (volatile unsigned long *) (0x40005400))
# define GPIODIR_C (* (volatile unsigned long *) (0x40006400))
# define GPIODIR_D (* (volatile unsigned long *) (0x40007400))
# define GPIODIR_F (* (volatile unsigned long *) (0x40025400)) // TEST

// Define UART0 GPIO Digital Enable
# define GPIODEN_A (* (volatile unsigned long *) (0x4000451C))
// Set bit 1
// PA0 pin 17 - U0Rx
// PA1 pin 18 - U0Tx
# define GPIODEN_B (* (volatile unsigned long *) (0x4000551C))
# define GPIODEN_C (* (volatile unsigned long *) (0x4000651C))
# define GPIODEN_D (* (volatile unsigned long *) (0x4000751C))
# define GPIODEN_F (* (volatile unsigned long *) (0x4002551C)) //TEST

// Define UART0 GPIO Alternate Select
# define GPIOAFSEL_A (* (volatile unsigned long *) (0x40004420))
// PA0 and PA1 set to 1
# define GPIOPCTL_A (* (volatile unsigned long *) (0x4000452C))
// 0x1

// (* (volatile unsigned long *) ())
// set corresponding afsel/pdr bits to 1 to "commit" changes to these registers
# define GPIOCR_B (* (volatile unsigned long *) (0x40005524))


// ------------------ INTERRUPT STUFF ------------------
# define GPIOIS_B (* (volatile unsigned long *) (0x40005404)) // interrupt sense
// 1=level sensitive, 0=edge sensitive
# define GPIOIEV_B (* (volatile unsigned long *) (0x4000540C)) // interrupt event
// 1=sense high/rising, 0=sense low/falling
# define GPIOIM_B (* (volatile unsigned long *) (0x40005410)) // interrupt mask
// corresponding bit set to 1 unmasks pin's interrupt
# define GPIOICR_B (* (volatile unsigned long *) (0x4000541C)) // clear interrupt
// corresponding bit set to 1 unmasks pin's interrupt

# define GPIOIS_C (* (volatile unsigned long *) ( 0x40006404)) // interrupt sense
// 1=level sensitive, 0=edge sensitive
# define GPIOIEV_C (* (volatile unsigned long *) (0x4000640C)) // interrupt event
// 1=sense high/rising, 0=sense low/falling
# define GPIOIM_C (* (volatile unsigned long *) (0x40006410)) // interrupt mask
// corresponding bit set to 1 unmasks pin's interrupt
# define GPIOICR_C (* (volatile unsigned long *) (0x4000641C)) // clear interrupt
// corresponding bit set to 1 unmasks pin's interrupt

# define GPIOIS_D (* (volatile unsigned long *) (0x40007404)) // interrupt sense
// 1=level sensitive, 0=edge sensitive
# define GPIOIEV_D (* (volatile unsigned long *) (0x4000740C)) // interrupt event
// 1=sense high/rising, 0=sense low/falling
# define GPIOIM_D (* (volatile unsigned long *) (0x40007410)) // interrupt mask
// corresponding bit set to 1 unmasks pin's interrupt
# define GPIOICR_D (* (volatile unsigned long *) (0x4000741C)) // clear interrupt
// corresponding bit set to 1 unmasks pin's interrupt


// ------------------ TIMER STUFF ------------------
# define RCGCTIMER (* (volatile unsigned long *) (0x400FE604))
// Bit 0 enables Timer 0, Bit 1 enables Timer 1, ..., Bit 5 enables Timer 5
# define GPTMCTL (* (volatile unsigned long *) (0x4003000C)) // 16/32 bit Timer 0 Address
// Bit 0 enables Timer A, Bit 8 enables Timer B 16/32 Bit
// Bits in this register should only be changed when the TnEN bit for the respective timer is cleared.
# define GPTMCFG (* (volatile unsigned long *) (0x40030000)) // 16/32 bit Timer 0 Address
// controls the bitwidth of a timer; 0x0: 32(16/32) or 64(32/64), 0x4: 16(16/32) or 32(32/64)
# define GPTMTAMR (* (volatile unsigned long *) (0x40030004)) // 16/32 bit Timer 0 Address
// controls the “mode” of a timer (0x2 is periodic, bit 4 is 0 for down counting (1 for up))
// Delay D depends on clock period T and max count M
// 16MHz clock, T = 62.5 nanoseconds
// M = 2^16 = 65536
// D = T * M = 4.096 milliseconds (max delay)
# define GPTMTAPR (* (volatile unsigned long *) (0x40030038)) // where Prescalar is
# define GPTMTAILR (* (volatile unsigned long *) (0x40030028)) // where InitCnt is (this is where end count is in count up mode)
// Prescalar (divide the clock): 1s/4.096ms = 244.14 -> 245
// Counter Period: Prescalar*T = 62.5ns*245 = 15.3125 micros.
// Counter Periods Needed to Reach 1s: 1s/15.3125 micros. = 65306.1224
// InitCnt: 65306
# define GPTMRIS (* (volatile unsigned long *) (0x4003001C)) // Timer flag (interrupt status)
// TATORIS (bit 0) is Timer A Raw Output flag
// TBTORIS (bit 8) is Timer B Raw Output flag
# define EN0 (* (volatile unsigned long *) (0xE000E100)) // EN0-EN4 correspond to interrupts
// Table 2-9, 16/32-Bit Timer 0A, bit 19 (in EN0)
# define GPTMIMR (* (volatile unsigned long *) (0x40030018))
// Set TATOIM, bit 0, Timer A Time-Out Interrupt Mask
# define GPTMICR (* (volatile unsigned long *) (0x40030024)) //clear flags in GPTMRIS


// ------------------ FUNCTION DECLARATIONS ------------------
void initialize(); // initializes all values
void UART_wait(); // waits for UART to establish good connection
void UART_Tx(char data); // prints to console
char UART_Rx(); // reads user input from controller
void blueLEDOn(); // turns on blue LED
void blueLEDOff(); // turns off blue LED
void LEDPinOn(); // turns on LED pin
void LEDPinOff(); // turns off LED pin
void sendSign(); // prints '$' to console

void checkTimer();
void checkButton();
void checkKeypad();

void delay(){
    int i = 0;
    while (i < 700000){
                ++i;
    } // wait
}


// ------------------ MAIN FUNCTION ------------------
int main(void)
{
    initialize();
    UART_wait();

    while (1) {
//        GPIOPUR_B |= (1 << 2); // enable pull up register
//        GPIOCR_B |= (1 << 2);
//        delay();
//        GPIOPDR_B |= (1 << 2);
//        GPIOCR_B |= (1 << 2);
//        delay();
        checkKeypad();
    }

    return 0;
}


// ------------------ FUNCTION DEFINITIONS ------------------
void initialize(){
    // initialize register values

    // - UART STUFF -----------
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

    // - GPIO STUFF -----------
    RCGCGPIO |= (1 << 1)|(1 << 2)|(1 << 3)|(1 << 5); // B|C|D|F;
    GPIOPUR_B |= (1 << 3); // for LED pin init
    GPIOPDR_B |= (1 << 2); // for LED pin init
    // pin's pull down register is enabled at 1, disabled at 0
    GPIODIR_B &= ~(1 << 2); // input for LED pull down/up
    GPIODIR_B &= ~(1 << 3); // input for button read pull up
    GPIODEN_B |= (1 << 2)|(1 << 3)|(1 << 7);
    GPIODIR_F |= (1 << 2); // Blue LED
    GPIODEN_F |= (1 << 2); // Blue LED
    GPIOCR_A |= (1 << 0)|(1 << 1); // UART
    GPIOCR_B |= (1 << 2)|(1 << 3); // Pull up/down

    // - KEYBOARD STUFF -----------
    GPIODIR_C &= ~(1 << 4)& ~(1 << 5)& ~(1 << 6)& ~(1 << 7); // input for LED pull down/up
    GPIODEN_C |= (1 << 4)|(1 << 4)|(1 << 6)|(1 << 7);
//    GPIOIS_C &= ~(1 << 0); // edge sensitive
//    GPIOIEV_C &= ~(1 << 0); // falling edge
//    delay();
//    GPIOIM_C |= (1 << 4)|(1 << 5)|(1 << 6)|(1 << 7);
//    EN0 |= (1 << 2); // enable port C interrupts
//    GPIOICR_C |= (1 << 4)|(1 << 5)|(1 << 6)|(1 << 7);

    GPIODIR_D |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3); // input for LED pull down/up
    GPIODEN_D |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3);
//    GPIOIS_D &= ~(1 << 0); // edge sensitive
//    GPIOIEV_D &= ~(1 << 0); // falling edge
//    delay();
//    GPIOIM_D |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3);
//    EN0 |= (1 << 3); // enable port D interrupts
//    GPIOICR_D |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3);

    // - INTERRUPT STUFF -----------
    GPIOIS_B &= ~(1 << 0); // edge sensitive
    GPIOIEV_B &= ~(1 << 0); // falling edge
    delay();
    GPIOIM_B |= (1 << 3); // bits 2 and 3
    EN0 |= (1 << 1); // enable port B interrupts
    GPIOICR_B |= (1 << 3);

    // - TIMER STUFF -----------
    RCGCTIMER |= (1 << 0); // enable Timer 0
    GPTMCTL &= ~0x1; // clear entire register or &= ~(0x1F)
    GPTMCFG = 0x4;
    GPTMTAMR = 0x2;
    GPTMTAPR = 0xF5;
    GPTMTAILR = 0xFF1A;
    GPTMIMR |= (1 << 0);
    EN0 |= (1 << 19);
    GPTMCTL |= (1 << 0); // enable after configuring timer
    GPTMICR |= (1 << 0);

}

void UART_wait(){
    int i = 0;
    while (i < 100000)
        ++i;
}


void UART_Tx (char data) {
    // PRINT CHAR to console
    while((UARTFR & 0x20) != 0); // while bit 5 of UARTFR is high (transmit/FIFO full)
    UARTDR = data;
}

char UART_Rx () {
    // READ CHAR from input
    char data;
    // this is a wait loop (acts like bitwise AND)
    while((UARTFR & 0x10) != 0); // while bit 4 of UARTFR is high (receive/FIFO empty)
    // ^ waiting for user to enter something into the data register
    data = UARTDR;
    return ((unsigned char) data);
}

// Pointer to the function needs to be inserted into the vector table
void blueLEDOn(){
    GPIODATA_F |= (1 << 2);
}

void blueLEDOff(){
    GPIODATA_F &= ~(1 << 2);
}

void LEDPinOn(){
    GPIOPUR_B |= (1 << 2); // enable pull up register
    GPIOCR_B |= (1 << 2);
}

void LEDPinOff(){
    GPIOPDR_B |= (1 << 2); // disable pull up register
    GPIOCR_B |= (1 << 2);
}

void sendSign(){
    UART_Tx('$');
}

void checkTimer() {
    if ((GPTMRIS & (0x1)) != 0) {
        LEDPinOn();
//        blueLEDOn();
        delay();
        LEDPinOff();
//        blueLEDOff();
    }
    GPTMICR |= (1 << 0);
}

void checkButton() {
    if ((GPIODATA_B & (0x8)) == 0) {
        // pin pulled down to Gnd by button
        sendSign();
//        blueLEDOn();
//        delay();
//        blueLEDOff();
    } else {
        // pin pulled up to Vcc by PUR
//        blueLEDOff();
    }
    GPIOICR_B |= 0x08;
}

char keypad[4][4] = {{'1', '2', '3', 'A'},
                     {'4', '5', '6', 'B'},
                     {'7', '8', '9', 'C'},
                     {'*', '0', '#', 'D'}};

void checkKeypad(){
    char send;
    int is_char = 0;
    int i = 0;
    while (i < 4) {
        // send row 1 high
        GPIODATA_D = 0x0;
        GPIODATA_D |= (1 << i); // send the i'th bit high
        int j = 4;
        while (j < 8) {
            // check if each column is high
            // check against 0 with 1 shifted into respective places
            if (GPIODATA_C & (0x00 + (1 << j))){
                send = keypad[i][j-4];
                is_char = 1;
                break;
            }
            ++j;
        }
        if (is_char){
            UART_Tx(send);
            break;
        }
        ++i;
    }


//    GPIOICR_C |= (1 << 4)|(1 << 5)|(1 << 6)|(1 << 7);
//    GPIOICR_D |= (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3);
}
