#  include <msp430g2231.h>

unsigned int delay ( unsigned int x)
{
  unsigned int i,j;
  for (i = 0; i<= x*100; i++)
  {
    for(j=0;j<=10000; j++);
  }
  return 0;
}


int main(void)
{
  // Stop watchdog timer to prevent time out reset 
  WDTCTL = WDTPW + WDTHOLD;

  P1DIR |=  (BIT0); //1.0 output (red)
  P1DIR |=  (BIT6); //1.6 output (green)
  P1DIR &= ~(BIT3); //1.3 input

  while (1) {
    if ((P1IN & BIT3) == BIT3) {
      P1OUT &= ~(BIT0); //P1.0 low
    }
    else {
      P1OUT |=  (BIT0); //P1.0 high
    }

    delay(1000);
    P1OUT &= ~(BIT6); // P6.0 Low
    delay(1000);
    P1OUT |=  (BIT6) ; // P6.0 High
  }
  
  return 0;
}
