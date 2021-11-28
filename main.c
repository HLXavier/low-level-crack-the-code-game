#include <avr/interrupt.h>
#include "./nokia5110.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
 
void start();
void sortSecret();
void displayCode();
void win();
void lose();
void nextDigit();
void nextValue();
void prevValue();
void verifyCode();

int time;
char stringTime[3];

int isTimeRunning = 0;

int randomParam = 0;

// Sorted digits
int secret[4];

// Displayed digits
int code[4];

int leds[4];
int currentDigit = 0;

// Botoes
#define BOTAO1 PD0
#define BOTAO2 PD1
#define BOTAO3 PD2
#define BOTAO4 PD3
#define BOTAO5 PD4

ISR(TIMER1_COMPA_vect) {
    nokia_lcd_set_cursor(0, 20);

    if (time < 10) {
        stringTime[0] = '0';    
        stringTime[1] = '0' + time;
    } else {
        itoa(time, stringTime, 10);
    }
    stringTime[2] = '\0';
    
    if (isTimeRunning) {
        nokia_lcd_write_string(stringTime, 1);
        displayCode();
        nokia_lcd_render();
        time--;
        if (time == -1) {
            lose();
        }
    }
}

// Tratamento interrupcao PCINT2
ISR(PCINT2_vect) {
    if (!(PIND & (1 << BOTAO1))){   // lê PD0
        nextValue();
        while (!(PIND & (1 << BOTAO1))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO2))){   // lê PD1
        prevValue();
        while (!(PIND & (1 << BOTAO2))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO3))){   // lê PD2
        nextDigit();
        while (!(PIND & (1 << BOTAO3))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO4))){   // lê PD3
        verifyCode();
        while (!(PIND & (1 << BOTAO4))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO5))){   // lê PD4
        start();
        while (!(PIND & (1 << BOTAO5))){
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

    // Seta PC0..PC4 como saida (leds)
    DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3);
    // Ligar os leds
    PORTC |= (leds[0] << PC0) | (leds[1] << PC1) | (leds[2] << PC2) | (leds[3] << PC3);
    

    // Timer
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);

    OCR1AH = 15624 >> 8;
    OCR1AL = 15624 & 0x0f;

    TIMSK1 |= (1 << OCIE1A);
    //

    sei();

    nokia_lcd_init();
    nokia_lcd_power(1);
    nokia_lcd_clear();

    while(1) {
        randomParam++;
    }

    return 0;
}

void start() {
    sortSecret();

    // start timer
    time = 60;
    isTimeRunning = 1;

    // clear code and leds list
    for (int i = 0; i < 4; i++) {
        code[i] = 0;
        leds[i] = 0;
    }
    currentDigit = 0;
}

void sortSecret() {
    srand(randomParam);
    for (int i = 0; i < 4; i++) {
        secret[i] = rand() % 10; // numero aleatorio de 0 a 9
    }
}

void displayCode() {
    
    // Render 4 digit code
    nokia_lcd_set_cursor(0, 0);
    char stringCode[8];
    for (int i = 0; i < 4; i++) {
        stringCode[i*2] = '0' + code[i]; // 0 2 4 6
        stringCode[i*2+1] = ' ';   // 1 3 5 7
    }
    stringCode[7] = '\0';
    nokia_lcd_write_string(stringCode, 1);

    // Render secret (test)
    nokia_lcd_set_cursor(0, 10);
    char stringCode2[8];
    for (int i = 0; i < 4; i++) {
        stringCode2[i*2] = '0' + secret[i]; // 0 2 4 6
        stringCode2[i*2+1] = ' ';   // 1 3 5 7
    }
    stringCode2[7] = '\0';
    nokia_lcd_write_string(stringCode2, 1);

    // Render digit position
    // nokia_lcd_set_cursor(0, 10);
    // char stringPosition[8];
    // for (int i = 0; i < 4; i++) {
    //     stringPosition[i] = ' ';
    // }
    // stringPosition[currentDigit*2] = '*';
    // stringPosition[7] = '\0';
    // nokia_lcd_write_string(stringPosition, 1);

    nokia_lcd_render();
}

void win() {
    isTimeRunning = 0;
    nokia_lcd_clear();
    nokia_lcd_write_string("Acertou :)", 1);
    nokia_lcd_render();
}

void lose() {
    isTimeRunning = 0;
    nokia_lcd_clear();
    nokia_lcd_write_string("Perdeu :(", 1);
    nokia_lcd_render();
}

void nextDigit() {
    currentDigit++;
    if (currentDigit > 3) {
        currentDigit = 0;
    };
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
    int correctNumbers = 0;
    for (int i = 0; i < 4; i++) {
        if (code[i] == secret[i]) {
            leds[i] = 1;
            correctNumbers++;
        }
        else {
            leds[i] = 0;
        }
    }
    PORTC |= (leds[0] << PC0) | (leds[1] << PC1) | (leds[2] << PC2) | (leds[3] << PC3);
    if (correctNumbers == 4) {
        win();
    }
}
