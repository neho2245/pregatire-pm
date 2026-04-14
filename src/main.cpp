#include <avr/io.h>
#include <util/delay.h>
 
int main() {

  // make it so it toggles on button press

  // set the button as input
  DDRB &= ~(1 << PB2);

  // activate the pull up resistance
  PORTB |= (1 << PB2);

	/* Setăm pinul 7 al portului D ca pin de ieșire. */
	DDRD |= (1 << PD7);
 
	while(1) {

    // check that the button was pressed
    // which means that the value on PB2 is 0 (the button connects the pin to the ground)
    if (! (PINB & (1 << PB2))) {
      PORTD ^= (1 << PD7);
      _delay_ms(500);    
    }

	}
 
	return 0;
}