/*
 * PWM_Thermostat.c
 *
 * Created: 23-Nov-18 1:09:31 PM
 * Author : Mads A. Skov
 *
 * Connected pins
 *    PB0: SER
 *    PB1: SCK
 *    PB2: RCK
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "characters.h"

#define SER PB0
#define SCK PB1
#define RCK PB2
#define Btn0 PD2
#define Btn1 PD3
#define Btn2 PD4

void initADC();
void shiftBit();
void updateDisplay(uint8_t digit, uint8_t pattern);
void timerSetup();

uint8_t curBtn;
uint8_t lBtn;

uint8_t curDigit = 0;
int curNr[] = {1,0,0,0};
uint8_t count = 0;

int delay = 0;

int main(void) {
    //Set up IO for display
	DDRB |= (1<<SER) | (1<<SCK) | (1<<RCK);
	PORTB &= ~(1<<0);
	
	//Set up IO for buttons
	DDRD &= ~((1<<Btn0) | (1<<Btn1) |  (1<<Btn2));
	PORTD |= (1<<Btn0) | (1<<Btn1) | (1<<Btn2);
	
	curBtn = PIND;
	lBtn = curBtn;
	
	initADC();
	timerSetup();
	updateDisplay(digit_0, curDigit);
	
    while (1) {
    }
}

ISR(TIMER0_COMPA_vect) {
	//Check buttons
	curBtn = PIND;
	if (!(curBtn & (1<<Btn0)) && (lBtn & (1<<Btn0))){
		curDigit++;
	}
	if (!(curBtn & (1<<Btn1)) && (lBtn & (1<<Btn1))){
		curDigit = 1;
	}
	if (!(curBtn & (1<<Btn2)) && (lBtn & (1<<Btn2))){
		curDigit--;
	}
	lBtn = curBtn;
	
	if (delay < 250){
		delay++;
	} 
	else {
		delay = 0;
		count++;
	}
	
	if (curDigit < 2){
		curDigit++;
	} 
	else {
		curDigit = 0;
	}
	
	int buffer = count;
	curNr[2] = buffer%10;
	curNr[1] = (buffer/10)%10;
	curNr[0] = (buffer/100)%10;
	
	switch(curNr[curDigit]) {
		case 0:
			updateDisplay(curDigit, digit_0);
			break;
		case 1:
			updateDisplay(curDigit, digit_1);
			break;
		case 2:
			updateDisplay(curDigit, digit_2);
			break;
		case 3:
			updateDisplay(curDigit, digit_3);
			break;
		case 4:
			updateDisplay(curDigit, digit_4);
			break;
		case 5:
			updateDisplay(curDigit, digit_5);
			break;
		case 6:
			updateDisplay(curDigit, digit_6);
			break;
		case 7:
			updateDisplay(curDigit, digit_7);
			break;
		case 8:
			updateDisplay(curDigit, digit_8);
			break;
		case 9:
			updateDisplay(curDigit, digit_9);
			break;
		default:
			updateDisplay(curDigit, digit_0);
			break;
	}
}

void initADC() {
	ADMUX |= (1<<REFS0) | (1<<REFS1);
	ADCSRA |= (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1);
}

void analogRead(){
	
}

void shiftBit(){
	PORTB |= (1<<SCK);
	PORTB &= ~(1<<SCK);
}

void timerSetup() {
	//Set timer to clear on compare
	TCCR0A = (1<<WGM01);
	//Setup timer interrupt to 2ms - 8bit timer  (timer 0) - 256 prescaler - 125 ticks
	TCCR0B = (1<<CS02);
	OCR0A = 125;
	TIMSK0 |= (1<<OCIE0A);
	sei();
}

void updateDisplay(uint8_t digit, uint8_t pattern) {
	//Turn off icons
	PORTB |= (1<<SER);
	shiftBit();
	shiftBit();
	
	//Turn on all segments
	for (uint8_t i = 0; i < 7; i++){
		if ((pattern>>i)%2){
			PORTB &= ~(1<<SER);
		} 
		else{
			PORTB |= (1<<SER);
		}
		shiftBit();
	}
	
	//Enable all digits
	for (uint8_t j = 0; j < 3; j++){
		if (j == digit){
			PORTB |= (1<<SER);
			shiftBit();
		} 
		else {
			PORTB &= ~(1<<SER);
			shiftBit();
		}
	}
	
	PORTB |= (1<<RCK);
	PORTB &= ~(1<<RCK);
}