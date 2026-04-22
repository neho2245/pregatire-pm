#include "log_display.h"
#include "lcd.h"
#include <stdio.h>
#include "pff.h"

void SD_display_logs() {
    char buffer[32];
    UINT br;
    
    if (pf_open("log.csv") != FR_OK) {
        LCD_clear();
        LCD_printAt(0, "No log data!");
        return;
    }

    while (1) {
        if (BUTTON_PRESSED(PB2)) {
            if (pf_read(buffer, sizeof(buffer) - 1, &br) == FR_OK && br > 0) {
                buffer[br] = '\0';
                LCD_clear();
                LCD_printAt(0, buffer);
            } else {
                pf_open("log.csv");
            }
            _delay_ms(300);
        }
    }
}
