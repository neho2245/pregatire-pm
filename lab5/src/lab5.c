#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <string.h>

#include "usart.h"
#include "lcd.h"
#include "pff.h"
#include "log_display.h"
#include "sd.h"

#define PM_BAUD 9600

FATFS fs;  // sistemul de fisiere

static DWORD logWritePos = 0;
static DWORD logReadPos = 0;

static char pendingLogs[128];
static uint8_t pendingLen = 0;

/*---------------------------------------------------------------------------*/
/* Citirea temperaturii si logarea pe SD si USART                           */
/*---------------------------------------------------------------------------*/

volatile uint16_t temperature = 0;  // Variabila pentru temperatura curenta

void adc_init() {
    /* TODO3: Configurati ADC pentru citirea senzorului de temperatura */
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN);

}

uint16_t adc_read() {
    /* TODO3: Cititi valoarea ADC pentru senzorul de temperatura */
    ADMUX &= 0b11100000;
    // 2. Select the new channel in ADMUX
    ADMUX |= PA0;
    // 3. Start single conversion
    ADCSRA |= (1 << ADSC);
    // 4. Busy wait for conversion to complete
    while (ADCSRA & (1 << ADSC));

    // Return ADC value
    return (ADC);
}

void SD_log_data(int temp_c) {
    /* TODO4 Scrieti temp_c in log.csv */
    char line[16];
    uint8_t len;

    sprintf(line, "%d\r\n", temp_c);
    len = (uint8_t)strlen(line);

    if ((uint16_t)pendingLen + len < sizeof(pendingLogs)) {
        memcpy(&pendingLogs[pendingLen], line, len);
        pendingLen += len;
    }
}

void log_temperature() {
    char log_entry[32];
    uint16_t temp = adc_read();

    // Convertire valoare ADC in tensiune si temperatura
    float temperature_c = (temp / 1024.0) * 100.0; // Conversie ADC -> Celsius (pentru LM35D)

    // Afisare pe LCD
    LCD_clear_top_line();
    sprintf(log_entry, "Temp: %.2f C", temperature_c);
    LCD_printAt(0, log_entry);

    // Logare in fisierul CSV
    SD_log_data((int)temperature_c);

    // Logare prin USART
    char usart_message[50];
    sprintf(usart_message, "Logged Temperature: %.2f C\r\n", temperature_c);
    USART0_print(usart_message);
}

/*---------------------------------------------------------------------------*/
/* Navigare prin logurile salvate                                           */
/*---------------------------------------------------------------------------*/

void init_log_display() {
    /* TODO4: Deschideti log.csv si pregatiti afisarea */
    pf_open("log.csv");
    logReadPos = 0;
    logWritePos = 0;
    pendingLen = 0;

    LCD_clear_top_line();
    LCD_printAt(0x00, "logs ready");
    USART0_print("Log display initialized\r\n");
}

void next_log_entry() {
    /* TODO5: Navigati prin loguri la apasarea butonului PB2 */
    char line[32];
    WORD bw;
    WORD br;
    char c;
    uint8_t i = 0;

    if (pendingLen > 0) {
        pf_open("log.csv");
        pf_lseek(logWritePos);
        pf_write(pendingLogs, pendingLen, &bw);
        pf_write(0, 0, &bw);

        logWritePos += pendingLen;
        pendingLen = 0;
    }

    pf_open("log.csv");

    if (logReadPos >= logWritePos) {
        LCD_clear_top_line();
        LCD_printAt(0x00, "log empty");
        USART0_print("Next log entry: <empty>\r\n");
        return;
    }

    pf_lseek(logReadPos);

    while (i < sizeof(line) - 1) {
        pf_read(&c, 1, &br);

        if (br == 0 || c == '\0') {
            break;
        }

        logReadPos++;

        if (c == '\r') {
            continue;
        }

        if (c == '\n') {
            break;
        }

        line[i++] = c;
    }

    line[i] = '\0';

    LCD_clear_top_line();
    LCD_printAt(0x00, line);

    USART0_print("Next log entry: ");
    USART0_print(line);
    USART0_print("\r\n");
}

void buttons_init() {
    /* TODO4: Configurati butonul PB2 pentru navigare prin loguri */
    DDRB &= ~(1 << PB2);  // Setare PB2 ca input
    PORTB |= (1 << PB2);  // Activare pull-up resistor

    /* TODO4: Configurati butonul PD6 pentru logarea temperaturii */
    DDRD &= ~(1 << PD6);  // Setare PD6 ca input
    PORTD |= (1 << PD6);  // Activare pull-up resistor
}

int main(void) {
    buttons_init();
    LCD_init();
    USART0_init(CALC_USART_UBRR(PM_BAUD));
    USART0_use_stdio();
    adc_init();  // Initializare ADC pentru citirea temperaturii

    sei(); // Activare intreruperi

    USART0_print("System Initialized\r\n");

    for (;;) {
        // Montare filesystem
        LCD_printAt(0x00, "mounting...");
        USART0_print("Mounting SD Card...\r\n");

        int mount = pf_mount(&fs);
        if (mount != FR_OK) {
            // Asteptati un timp si reincercati
            USART0_print("SD Card Mount Failed. Retrying...\r\n");
            _delay_ms(1000);
            continue;
        }

        LCD_clear_top_line();
        LCD_printAt(0x00, "mounted!");
        USART0_print("SD Card Mounted Successfully.\r\n");
        _delay_ms(2000);
        LCD_clear_top_line();

        // Initializare afisare loguri
        init_log_display();

        for (;;) {
            /* TODO4: La apasarea butonului PB2, afisati urmatorul log din log.csv */
            if (!(PINB & (1 << PB2))) {
                next_log_entry();
                _delay_ms(300);  // Debounce
            }

            /* TODO4: La apasarea butonului PD6, cititi si logati temperatura */
            if (!(PIND & (1 << PD6))) {
                log_temperature();
                _delay_ms(300);  // Debounce
            }
        }
    }

    return 0;
}
