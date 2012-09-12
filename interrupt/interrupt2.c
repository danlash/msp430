# include <msp430g2231.h>
# include <legacymsp430.h>

typedef unsigned char	uint8_t;
typedef unsigned int	uint16_t;
typedef int  			int16_t;

volatile uint8_t rounds = 122-1;
volatile uint8_t ticks=0;
uint8_t tps = 0;
volatile uint8_t stacked=0;

int main(void)
{
//wdt= watch dog timer
//nmi=
	WDTCTL = WDTPW + WDTHOLD + WDTNMI + WDTNMIES;	// stop WDT, enable NMI hi/lo
//dco= digital oscillator instead of crystal
//vlo= very low frequency oscillator
//aclk= alternate clock (not main clock)
//mcu= main loop?
	BCSCTL3 |= LFXT1S_2;			// use VLO as ACLK, ~12Khz
	BCSCTL1 = CALBC1_1MHZ;		// Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;
//set the watch dog timer for 8ms, at 1mhz thats
//1000/8ms= 125 = maximum ticks wdt can get per second
//122 is rough start, incorporates wakeup time for mcu

	//_______ VLO calibration, 
	// WDT counts 122x8ms (on 1 Mhz MCLK) times and see how many ACLK counts per sec
//after 122x8ms we would go 1mhz and count true ticks per second on the low freq 12khz timer
//then we can get better time tracking on the 12khz

//at 1mhz watch dog divides by 64 (2^6)
//WDTIS0 is input to multiplexer to tell it to use 2^6
//1000000 / 2^6 (wdt divider) = 5^6, 15625hz
//watch dog checks in every 15khz
//125 ticks of 1mhz / 64 = 8ms

	// now we got tps as ACLK/64 counts in one sec
	// for 1/8 sec per interrupt we will need to set CCR0 to tps * 8
	CCR0  = 64-1;					// we want one count every 64 clock cycle
	CCTL0 = CCIE;					// CCR0 interrupt enabled
	TACTL = TASSEL_1 + MC_1;	// ACLK, upmode


    WDTCTL = WDT_MDLY_8 + WDTNMI + WDTNMIES;	// WDT at 8ms, NMI hi/lo
	IE1 |= WDTIE;					// Enable WDT interrupt
	_BIS_SR(GIE);					// enable interrupt

	// wait for VLO calibration be done
	// timer interrupt to reduce rounds from 122 to 0
	while (rounds);

	// save ticks per second value for later setting
	unsigned char use_tps = tps;
	CCR0 = (use_tps-0) * 8;

	P1DIR |= BIT6; 



	// LPM0 (shut down the CPU) with interrupts enabled
	__bis_SR_register(CPUOFF | GIE);

	return 0;
}

interrupt(TIMERA0_VECTOR) TimerA0_ISR(void) {
	ticks++;
	if (!(ticks&0x07)) { //every 8th interrupt, assume 1 sec
		stacked++;
		P1OUT ^= BIT6;

	}
}

interrupt(WDT_VECTOR) wdt_ISR(void) {
	rounds--;
	if (!rounds) {
		tps = ticks;
		IE1 &= ~WDTIE;				// Disable WDT interrupt
		//WDTCTL = WDTPW + WDTHOLD + WDTNMI + WDTNMIES;
	}//if
}