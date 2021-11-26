#include <avr/interrupt.h>
#include "./nokia5110.h"
#include <avr/io.h>
#include <util/delay.h>

void start();
void sortSecret();
void nextDigit();
void nextValue();
void prevValue();
void verifyCode();

//uint8_t heart[5] = {
//    0b00000000,
//    0b00011000,
//    0b00100100,
//    0b01111110,
//    0b00000000,
//};

int time = 9;

// Sorted digits
int secret[4];

// Displayed digits
int code[4];

int leds[4];
int currentDigit = 0;

//Botoes
#define BOTAO1 PD0
#define BOTAO2 PD1
#define BOTAO3 PD2
#define BOTAO4 PD3
#define BOTAO5 PD4

ISR(TIMER1_COMPA_vect) {
    nokia_lcd_set_cursor(0, 0);

    char buf[2] = " \0";
    buf[0] = '0' + time;

    nokia_lcd_write_string(buf, 1);
    nokia_lcd_render();

    time--;
}

//Tratamento interrupcao PCINT2
ISR(PCINT2_vect){
    if (!(PIND & (1 << BOTAO1))){   // lê PD0
        nokia_lcd_write_string("botao 1!", 1);
        //debounce
        while (!(PIND & (1 << BOTAO1))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO2))){   // lê PD1
        nokia_lcd_write_string("botao 2!", 1);
        //debounce
        while (!(PIND & (1 << BOTAO2))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO3))){   // lê PD0
        nokia_lcd_write_string("botao 3!", 1);
        //debounce
        while (!(PIND & (1 << BOTAO1))){
            _delay_ms(1);
        }
    }
    _delay_ms(1);
}


int main() {
    // Botão: seta PD como entrada
    DDRD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD4));
    //Ativa pull-up da PD
    PORTD |= ((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD4)); 

    //Habilita vetor de interrupcao para PD0...PD7
    PCICR |= (1 << PCIE2);
    //Habilita interrupcao para PD0..PD4
    PCMSK2 |= (1 << PCINT16) | (1 << PCINT17) | (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20);

    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);

    OCR1AH = 15624 >> 8;
    OCR1AL = 15624 & 0x0f;

    TIMSK1 |= (1 << OCIE1A);

    sei();



    nokia_lcd_init();
    nokia_lcd_power(1);
    nokia_lcd_clear();
    nokia_lcd_set_cursor(0, 12);
    nokia_lcd_write_string("me mato ta", 2);

    while(1) {}

    return 0;
}

void start() {
    sortSecret();
    // start timer
    // clear code and leds list
    // display stuf
}

void sortSecret() {
    // sort 4 digit code (random library)
    // store in "secrret" variable
}

void nextDigit() {
    currentDigit++;
    if (currentDigit > 3) {
        currentDigit = 0;
    }
}

void nextValue() {
    code[currentDigit]++;
    if (code[currentDigit] > 9) {
        code[currentDigit] = 0;
    }
}

void prevValue() {
    code[currentDigit]--;
    if (code[currentDigit] < 0) {
        code[currentDigit] = 9;
    }
}

void verifyCode() {
    for (int i = 0; i < 4; i++) {
        if (code[i] == secret[i]) {
            leds[i] = 1;
        }
        else {
            leds[i] = 0;
        }
    }