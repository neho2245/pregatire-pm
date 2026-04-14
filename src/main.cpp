#include <avr/io.h>
#include <util/delay.h>
 
int main() {

  // make it so it toggles on button press

  // set the button as input
  DDRB &= ~(1 << PB2);

  // activate the pull up resistance
  PORTB |= (1 << PB2);

	// green LED
	DDRD |= (1 << PD7);

  // activate red LED
  DDRB |= (1 << PB3);
  PORTB |= (1 << PB3);
 
  int toggle_semaphore = 0;
	while(1) {

    // check that the button was pressed
    // which means that the value on PB2 is 0 (the button connects the pin to the ground)
    if (! (PINB & (1 << PB2))) {
      // activate the semaphore
      toggle_semaphore = 1;
      _delay_ms(200);
    }

    if (toggle_semaphore) {
      // activate green LED to get yellow
      PORTD = (1 << PD7);
      _delay_ms(1000);
      
      // deactivate red LED
      PORTB &= ~(1 << PB3);
      _delay_ms(5000);

      // deactivate green LED
      PORTD &= ~(1 << PD7);
      _delay_ms(1000);

      // reactivate red LED
      PORTB |= (1 << PB3);
      
      // deactivate semaphore
      toggle_semaphore = 0;
    }

	}
 
	return 0;
}