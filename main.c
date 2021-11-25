#include <avr/interrupt.h>
#include "./nokia5110.h"

uint8_t heart[5] = {
    0b00000000,
    0b00011000,
    0b00100100,
    0b01111110,
    0b00000000,
};

int time = 9;

ISR(TIMER1_COMPA_vect) {
    nokia_lcd_set_cursor(0, 0);

    char buf[2] = " \0";
    buf[0] = '0' + time;

    nokia_lcd_write_string(buf, 1);
    nokia_lcd_render();

    time--;
}

int main() {

    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);

    OCR1AH = 15624 >> 8;
    OCR1AL = 15624 & 0x0f;

    TIMSK1 |= (1 << OCIE1A);

    sei();



    nokia_lcd_init();
    nokia_lcd_power(1);
    nokia_lcd_clear();

    while(1) {}

    return 0;
}