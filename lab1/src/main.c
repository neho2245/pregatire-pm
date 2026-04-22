#include <util/delay.h>
#include <usart.h>
#include <stdio.h>
#include <string.h>

#define CLOCK_SPEED 12000000
#define BAUD 38400
#define MYUBRR CLOCK_SPEED/16/BAUD-1

#define MORSE_LUNG  3
#define MORSE_SCURT 1

uint16_t ticks = 0;
uint16_t lastBlinkTicks = 0;  // timestamp-ul (în ticks) ultimei schimbări de stare a LED-ului

/* Literele alfabetului in cod Morse */
char* morse_alphabet[] = {
    (char*)"13",    // a
    (char*)"3111",  // b
    (char*)"3131",  // c
    (char*)"311",   // d
    (char*)"1",     // e
    (char*)"1131",  // f
    (char*)"331",	// g
    (char*)"1111",	// h
    (char*)"11",	// i
    (char*)"1333",	// j
    (char*)"313",	// k
    (char*)"1311",	// l
    (char*)"33",	// m
    (char*)"31",	// n
    (char*)"333",	// o
    (char*)"1331",	// p
    (char*)"3313",	// q
    (char*)"131",	// r
    (char*)"111",	// s
    (char*)"3",	    // t
    (char*)"113",	// u
    (char*)"1113",	// v
    (char*)"133",	// w
    (char*)"3113",	// x
    (char*)"3133",	// y
    (char*)"3311"	// z
};

/* apelati functia pentru a obtine un ton lung/scurt,
 * folosind macro-urile MORSE_LUNG si MORSE_SCURT
 */
void speaker_morse(int tip)
{
    int delay = 1;
    int i;

    DDRD |= (1 << PD4);     // PD4 trebuie sa fie output

    for (i = 0; i < tip * 50 / delay; i++)
    {
        PIND = (1 << PD4);  // alternam starea iesirii
        _delay_ms(1);
    }
}

char write_buffer[30];
char read_buffer[30];
uint8_t recived_data = 0;

typedef enum {
	on = 0,
	off,
	red,
	green,
	blue,
    blink,
} LEDState_t;

LEDState_t LEDState  = off;

/* functie care verifica daca exista date disponibile in buffer-ul de receptie */
char USART0_data_available() {
    return ((UCSR0A & (1<<RXC0)) >> RXC0);
}

void USART0_receive_until_newline(char* received_data)
{
    char received_char;
    int index = 0;
    
    // se verifica daca exista date disponibile in buffer-ul de receptie
    if (USART0_data_available()){ 
      
    /* se citesc datele caracter cu caracter pana la '\n'. daca nu este intalnit '\n', programul ramane blocat aici! */
    while ((received_char = USART0_receive()) != '\n') {
        received_data[index++] = received_char;
    }

    received_data[index] = '\n';  // adauga '\n'
    received_data[++index] = '\0'; // adauga terminatorul de string la sfarsit

    // se seteaza un flag care indica faptul ca s-au receptionat date cu scopul de a le prelucra
    recived_data = 1;
    // se afiseaza datele primite:
    sprintf(write_buffer, "Date primite: ");
    USART0_print (write_buffer);
    USART0_print (received_data);
    }
}

char USART_exec(char* command){
    /* se verifica daca sirul de caractere este: */
  
    if ((strcmp(command, "on\n") == 0) || (strcmp(command, "on\r\n") == 0)){
      sprintf(write_buffer, "s-a primit comanda \"on\"\r\n"); 
      USART0_print (write_buffer);
      LEDState = on;
  
    } else if ((strcmp(command, "off\n") == 0) || (strcmp(command, "off\r\n") == 0)){
      sprintf(write_buffer, "s-a primit comanda \"off\"\r\n"); 
      USART0_print (write_buffer);
      LEDState = off;
  
    } else if ((strcmp(command, "red\n") == 0) || (strcmp(command, "red\r\n") == 0)){
      sprintf(write_buffer, "s-a primit comanda \"red\"\r\n"); 
      USART0_print (write_buffer);
      LEDState = red;
  
    } else if ((strcmp(command, "green\n") == 0) || (strcmp(command, "green\r\n") == 0)){
      sprintf(write_buffer, "s-a primit comanda \"green\"\r\n"); 
      USART0_print (write_buffer);
      LEDState = green;
  
    } else if ((strcmp(command, "blue\n") == 0) || (strcmp(command, "blue\r\n") == 0)){
      sprintf(write_buffer, "s-a primit comanda \"blue\"\r\n"); 
      USART0_print (write_buffer);
      LEDState = blue;

    } else if ((strcmp(command, "blink\n") == 0) || (strcmp(command, "blink\r\n") == 0)){
      sprintf(write_buffer, "s-a primit comanda \"blink\"\r\n"); 
      USART0_print (write_buffer);
      LEDState = blink;
  
    /* se afiseaza un mesaj de eroare daca sirul de caractere nu verifica niciuna dintre conditiile de mai sus: */
    } else {
      sprintf(write_buffer, "comanda invalida!\r\n"); 
      USART0_print (write_buffer);
    }
    // se reseteaza flag-ul cu scopul de a indica ca se pot prelucra alte date
    recived_data = 0;
    return 0;
  }
  
void RGB_control(){
    switch (LEDState)
    {
        case 0:
        PORTD &= ~(1 << PORTD5); // red on
        PORTD &= ~(1 << PORTD7); // green on
        PORTB &= ~(1 << PORTB3); // blue on
        break;

        case 1:
        PORTD |= (1 << PORTD5); // red off
        PORTD |= (1 << PORTD7); // green off
        PORTB |= (1 << PORTB3); // blue off
        break;

        case 2:
        PORTD &= ~(1 << PORTD5); // red on
        PORTD |= (1 << PORTD7); // green off
        PORTB |= (1 << PORTB3); // blue off
        break;

        case 3:
        PORTD |= (1 << PORTD5); // red off
        PORTD &= ~(1 << PORTD7); // green on
        PORTB |= (1 << PORTB3); // blue off
        break;

        case 4:
        PORTD |= (1 << PORTD5); // red off
        PORTD |= (1 << PORTD7); // green off
        PORTB &= ~(1 << PORTB3); // blue on
        break;

        case 5:
        if ((ticks - lastBlinkTicks) > 50){ // ATENȚIE: 50 ticks == 500ms!
        lastBlinkTicks = ticks;  // salvăm timestampul evenimentului
        PIND |= (1 << PD5);  // inversăm starea LED-ului
        }
        _delay_ms(10);
        ticks ++ ;
        break;
    }
}

int main() {
	
  USART0_init(MYUBRR);

  DDRD |= (1<<DDD5) | (1<<DDD7); //PD5 si PD7 (red si green) setati ca iesire
  DDRB |= (1<<DDB3); //PB3 (blue) setat ca  iesire

  PORTD |= (1 << PORTD5); // red off
  PORTD |= (1 << PORTD7); // green off
  PORTB |= (1 << PORTB3); // blue off

  USART0_init(MYUBRR);  //initializare USART
  USART0_print("Hello World!\r\n"); // afisez mesaj de test

  while (1){
    USART0_receive_until_newline(read_buffer);
    if(recived_data){
        USART_exec(read_buffer);
        }

        RGB_control();
    }
	return 0;
}
