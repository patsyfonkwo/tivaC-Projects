
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
// Define UART0 GPIO Alternate Select
# define GPIOAFSEL_A (* (volatile unsigned long *) (0x40004420))
// PA0 and PA1 set to 1
# define GPIOPCTL_A (* (volatile unsigned long *) (0x4000452C))
// 0x1
// Define UART0 GPIO Digital Enable
# define GPIODEN_A (* (volatile unsigned long *) (0x4000451C))
// Set bit 1
// PA0 pin 17 - U0Rx
// PA1 pin 18 - U0Tx
// Define GPIO Direction A (UART0 in this port)
# define GPIODIR_A (* (volatile unsigned long *) (0x40004400))
# define GPIOPUR_A (* (volatile unsigned long *) (0x40004510))
// pin's pull up register is enabled at 1, disabled at 0
# define GPIOPDR_A (* (volatile unsigned long *) (0x40004514))
// pin's pull down register is enabled at 1, disabled at 0


# define RCGCGPIO (* (volatile unsigned long *) (0x400FE608))
// Port B is bit 1 (set to 1)
# define GPIODATA_B (* (volatile unsigned long *) ( 0x40005300))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0011 0000 0000 --> 0x100 (0s remain unchanged)
# define GPIOAFSEL_B (* (volatile unsigned long *) ( 0x40005420))
# define GPIOPCTL_B (* (volatile unsigned long *) ( 0x4000552C)) // 27, 26, 24, 25 for port 6 (0x4)
# define GPIODIR_B (* (volatile unsigned long *) (0x40005400))
# define GPIODEN_B (* (volatile unsigned long *) (0x4000551C))
# define GPIOCR_B (* (volatile unsigned long *) (0x40005524))

# define GPIODATA_E (* (volatile unsigned long *) (0x40024100))
// Bit Masking [9:2] : 0011 1111 1100 --> 0x3FC (0s remain unchanged)
// Bit Masking [9:2] : 0001 0000 0000 --> 0x100 (0s remain unchanged)
# define GPIOAFSEL_E (* (volatile unsigned long *) (0x40024420))
# define GPIOAMSEL_E (* (volatile unsigned long *) (0x40024528))
# define GPIODEN_E (* (volatile unsigned long *) (0x4002451C))

# define RCGCPWM (* (volatile unsigned long *) (0x400FE640))
// Module 0 is bit 0 (set to 1)
# define RCC (* (volatile unsigned long *) (0x400FE060))
//RCC register
// USESYSDIV, bit 20 – use PWM clock divider
// PWMDIV, bits 19:17 – determine clock division
//o 0x0 /2, 0x1 /4, 0x2 /8, ... 0x6 /64
# define PWM0CTL (* (volatile unsigned long *) (0x40028040))
# define PWM0LOAD (* (volatile unsigned long *) (0x40028050))
//# define PWM0COUNT (* (volatile unsigned long *) (0x40028054)) // not necessary
# define PWM0CMPA (* (volatile unsigned long *) (0x40028058))
//# define PWM0CMPB (* (volatile unsigned long *) (0x4002805C)) // not necessary
# define PWM0GENA (* (volatile unsigned long *) (0x40028060))
//# define PWM0GENB (* (volatile unsigned long *) (0x40028064)) // not necessary
# define PWMENABLE (* (volatile unsigned long *) (0x40028008))

# define RCGCADC (* (volatile unsigned long *) (0x400FE638))
# define ADCACTSS (* (volatile unsigned long *) (0x40038000))
# define ADCSSMUX0 (* (volatile unsigned long *) (0x400380A0)) // – 8 sample sources
# define ADCEMUX (* (volatile unsigned long *) (0x40038014))
# define ADCPSSI (* (volatile unsigned long *) (0x40038028)) // register • Bit 3 triggers SS3
# define ADCRIS (* (volatile unsigned long *) (0x40038004))
# define ADCISC (* (volatile unsigned long *) (0x4003800C)) // - “This bit is cleared by writing a 1 to the IN3 bit in the ADCISC register.”
# define ADCSSFIFO3 (* (volatile unsigned long *) (0x400380A8))
# define ADCSSCTL3 (* (volatile unsigned long *) (0x400380A4))


void delay(long d);

void initUART(){
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
}

void initPWM() {
    PWM0CTL &= ~(1 << 0); // Disable PWM0 channel 0 output
    PWM0CTL &= ~(1 << 1); // set bit 1 to 0
    PWM0LOAD = 1600; // Set Load value for 1kHz
    PWM0CMPA = 160; // 640; // 960; // Set initial duty cycle to 40%
    PWM0GENA |= (0x3 << 0); // Drive high on load
    PWM0GENA |= (0x2 << 6); // Drive low on cmpa
    PWM0CTL |= (1 << 0); // Enable PWM0 channel 0 outputs
    PWMENABLE |= (1 << 0);
}

void initGPIO() {
    GPIOPCTL_B &= ~(0xF << 24); // Configure PB6
    GPIOAFSEL_B |= (1 << 6); // Set PB6 as alternative function
    GPIODIR_B |= (1 << 6); // Make PB6 output
    GPIODEN_B |= (1 << 6); // Enable digital function on PB6
    GPIOPCTL_B |= (0x4 << 24); // Configure PB6

    GPIOAFSEL_E |= (1 << 3); // Set PB6 as alternative function
    GPIODEN_E &= ~(1 << 3); // Enable digital function on PB6
    GPIOAMSEL_E |= (1 << 3);
}

void initADC () {
    ADCACTSS &= ~(1 << 3);
    ADCEMUX &= ~(0xF000);
    ADCSSMUX0 = 0;
    ADCSSCTL3 |= (1 << 1)|(1 << 2);
    ADCACTSS |= (1 << 3);
}



void initialize(){
    // Clock Setting for PWM and GPIO Port
    RCGCGPIO |= (1 << 1)|(1 << 4); // Enable system clock to PORTB and PORTE
    RCGCPWM |= (1 << 0); // Enable clock to PWM0 module
    RCGCADC |= (1 << 0);
//    RCC &= (~(1 << 20)); // Directly feed clock to PWM0 module without pre-divider

    initPWM();

    initGPIO();

    initADC();

    RCC |= (1 << 20)|(1 << 18)|(1 << 17);
//    delay(10000);
//    GPIOPCTL_B |= (0x4 << 24); // This sets bits 27-24 to 0x4 for PB6
//    GPIOCR_B |= (1 << 6);
//    initUART();



}



void delay(long d){
    while (--d);
}

//One suggestion is to make sure that the duty cycle ranges between 60% and 100%.
//If the duty cycle is below ~60% then the LED might not light at all.

void setDutyCycle(unsigned int percent) {
    if (percent < 10) percent = 10; // Ensure minimum 40%
    if (percent > 99) percent = 99; // Cap at 100%

//    PWM0CTL &= ~(1 << 0); // Disable PWM0 channel 0 output
    unsigned long loadValue = 1600; // PWM Load Value
    unsigned long dutyValue = loadValue - ((loadValue * percent) / 100);

    PWM0CMPA = dutyValue; // Update the PWM Compare Value
//    PWM0CTL |= (1 << 0); // Enable PWM0 channel 0 outputs
}


void UART_wait(){
    int i = 0;
    while (i < 100000)
        ++i;
}

int main(void)
{
    initialize();

    while(1){
        ADCPSSI |= (1 << 3); // Trigger ADC conversion
        while ((ADCRIS & (1 << 3)) == 0); // Wait for conversion completion
        unsigned long adc = ADCSSFIFO3;
        ADCISC |= (1 << 3);
        float voltage = (adc * 0.0008);
        unsigned int percentage = (voltage / 3.3) * 100;

        setDutyCycle(percentage);



    }
	return 0;
}
