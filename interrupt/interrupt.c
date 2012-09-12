# include <msp430g2231.h>
# include <legacymsp430.h>

int main(void)
{
	// Set clock to 1 MHz
	DCOCTL= 0;
	BCSCTL1= CALBC1_1MHZ;
	DCOCTL= CALDCO_1MHZ;

	// SMCLK = 1 MHz / 8 = 125 KHz (SLAU144E p.5-15)
	//BCSCTL2 |= DIVS_3;

	P1DIR |= BIT6; 
	//48ms per tick
	// Source Timer A from SMCLK (TASSEL_2), up mode (MC_1).
	// Up mode counts up to TACCR0. SLAU144E p.12-20
	TACTL = TASSEL_2 | MC_1; 

	// CCIE = Interrupt when timer counts to TACCR1
	TACCTL1 = CCIE;

	// count up to
	TACCR0 = 64 - 1; //one interrupt per quarter second 125000/4

	// Initial CCR1
	TACCR1 = 0;

	// LPM0 (shut down the CPU) with interrupts enabled
	__bis_SR_register(CPUOFF | GIE);

	return 0;
}

// Timer A0 interrupt service routine
uint8_t ticks= 0
interrupt(TIMERA1_VECTOR) ta1_isr(void) {
	//ticks ++ if 4th, add a second to the stack
	ticks++;
	if (ticks % 16 == 0) {
		P1OUT ^= BIT6;
		ticks = 0;
	}
}