#include <avr/interrupt.h>
#include "./nokia5110.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
 
void start();
void sortSecret();
void win();
void lose();
void nextDigit();
void nextValue();
void prevValue();
void verifyCode();
void render();
void renderCode();
void renderTime();
void renderLives();

int time;
char stringTime[3];
int isTimeRunning = 0;

int lives;
char stringLives[3];

int randomParam = 0;

// Sorted digits
int secret[4] = {3,1,0,0};

// Displayed digits
int code[4];

int greenLeds[4];
int purpleLeds[4];
int currentDigit = 0;

// Buttons
#define BOTAO1 PD3
#define BOTAO2 PD4
#define BOTAO3 PD5
#define BOTAO4 PD6
#define BOTAO5 PD7

ISR(TIMER1_COMPA_vect) {
    if (isTimeRunning) {
        render();
        time--;
        if (time < 0) {
            lose();
        }
    }
}

// Tratamento interrupcao PCINT2
ISR(PCINT2_vect) {
    if (!(PIND & (1 << BOTAO1))){   // lê PD3
        nextValue();
        render();
        while (!(PIND & (1 << BOTAO1))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO2))){   // lê PD4
        prevValue();
        render();
        while (!(PIND & (1 << BOTAO2))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO3))){   // lê PD5
        nextDigit();
        render();
        while (!(PIND & (1 << BOTAO3))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO4))){   // lê PD6
        verifyCode();
        render();
        while (!(PIND & (1 << BOTAO4))){
            _delay_ms(1);
        }
    }
    if (!(PIND & (1 << BOTAO5))){   // lê PD7
        start();
        while (!(PIND & (1 << BOTAO5))){
            _delay_ms(1);
        }
    }
    _delay_ms(1);
}


int main() {
    // Botão: seta PD como entrada(botoes)
    DDRD &= ~( (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7) );
    //Ativa pull-up da PD
    PORTD |= ((1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7)); 

    //Habilita vetor de interrupcao para PD0...PD7
    PCICR |= (1 << PCIE2) | (1 << PCIE1);
    //Habilita interrupcao para os pinos dos botoes (PD3, PD4, PD5, PD6, PD7)
    PCMSK2 |= (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);
    

    //Seta PD0, PD1 e PD2 como saida (leds)
    DDRD |= (1 << PD0) | (1 << PD1) | (1 << PD2);
    // Seta PC0..PC4 e PC6 como saida (leds)
    DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC6);
    // Ligar os leds
    //PORTC |= ( (greenLeds[0] << PC0) | (greenLeds[1] << PC1) | (greenLeds[2] << PC2) | (greenLeds[3] << PC3) | (purpleLeds[0] << PC6) );
    //PORTD |= ( (purpleLeds[1] << PD0) | (purpleLeds[2] << PD1) | (purpleLeds[3] << PD2) );


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
    //sortSecret();

    // start timer
    time = 30;
    isTimeRunning = 1;

    // clear code and leds list
    lives = 10;
    for (int i = 0; i < 4; i++) {
        code[i] = 0;
        greenLeds[i] = 0;
        purpleLeds[i] = 0;
    }
    currentDigit = 0;
}

void sortSecret() {
    srand(randomParam);
    for (int i = 0; i < 4; i++) {
        // numero aleatorio de 0 a 9
        secret[i] = rand() % 10;
    }
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
    // Ligar os leds
    PORTC |= ( (greenLeds[0] << PC0) | (greenLeds[1] << PC1) | (greenLeds[2] << PC2) | (greenLeds[3] << PC3) | (purpleLeds[0] << PC6) );
    PORTD |= ( (purpleLeds[1] << PD0) | (purpleLeds[2] << PD1) | (purpleLeds[3] << PD2) );
    
    int correctNumbers = 0;
    for (int i = 0; i < 4; i++) {
        if (code[i] == secret[i]) {
            greenLeds[i] = 1;
            correctNumbers++;
        }
        else {
            greenLeds[i] = 0;
            purpleLeds[i] = 0;
        }
    }
    if (correctNumbers == 4) {
        win();
    } else {
        lives--;
        if (lives == 0) {
            lose();
        }
        time = 30;
    }
}

void render() {
    if (isTimeRunning) {
        nokia_lcd_clear();

        renderCode();
        renderTime();
        renderLives();

        nokia_lcd_render();
    }
}

void renderCode() {
    
    // Render 4 digit code
    nokia_lcd_set_cursor(0, 0);
    char stringCode[8];
    for (int i = 0; i < 4; i++) {
        stringCode[i*2] = '0' + code[i]; // 0 2 4 6
        stringCode[i*2+1] = ' ';   // 1 3 5 7
    }
    stringCode[7] = '\0';
    nokia_lcd_write_string(stringCode, 1);

    // Render digit position
    nokia_lcd_set_cursor(0, 10);
    char stringPosition[8];
    for (int i = 0; i < 4; i++) {
        stringPosition[i] = ' ';
    }
    stringPosition[currentDigit*2] = '*';
    stringPosition[7] = '\0';
    nokia_lcd_write_string(stringPosition, 1);
}

void renderTime() {
    nokia_lcd_set_cursor(0, 20);

    if (time < 10) {
        stringTime[0] = '0';    
        stringTime[1] = '0' + time;
    } else {
        itoa(time, stringTime, 10);
    }
    stringTime[2] = '\0';

    nokia_lcd_write_string(stringTime, 1);
}

void renderLives() {
    nokia_lcd_set_cursor(0, 30);

    if (lives < 10) {
        stringLives[0] = '0';    
        stringLives[1] = '0' + lives;
    } else {
        itoa(lives, stringLives, 10);
    }
    stringLives[2] = '\0';

    nokia_lcd_write_string(stringLives, 1);
}
