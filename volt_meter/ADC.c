/*
 * ADC.c
 *
 * Created: 4/19/2015 10:19:41 AM
 *  Author: DUDE
 */ 


#include <avr/io.h>
#ifndef F_CPU
#define F_CPU 1000000 
#endif
#include <util/delay.h>
void setChannel(int8_t i)
{
	//if(i>8)
	ADMUX|=(0x0F)&i;
}
void adjADC(int adj)
{
	if(adj==1)
	ADMUX|=(1<<ADLAR);
}
void initADC()
{
	setChannel(0);
	ADMUX|=(1<<REFS0); // select AVcc as reference
	//ADCSRA=0b10000111;
	ADCSRA|=(1<<ADEN)|/*(1<<ADPS0)|*/(1<<ADPS1)|(1<<ADPS2);		//ADC enable/ set clk f_cpu/128
	//ADCSRA|=(1<<ADIE); //set to use special function register
	//SFIOR|=(1<<ADTS0)|(1<<ADTS1); // special function register, start convention at every TIMER0 COMP match
}
void setInt()
{
	ADCSRA|=(1<<ADIE); // complete interrupt
}

int16_t getADC()
{
	if(!(ADMUX&(1<<ADLAR)))
	return ADC;
	else
	return ADCH;
}

int8_t getADCH()
{
	return ADCH;
}

int8_t getADCL()
{
	return ADCL;
}
int16_t readADC()
{
	ADCSRA|=(1<<ADSC);
	//PORTC|=2;
	if(!(ADCSRA&(1<<ADIE)))
	{
		while(ADCSRA&(1<<ADSC))
		;
		//PORTC&=~2;
		return ADC;
	}
	else
		return 0;
}
void setADC()
{
	ADCSRA|=(1<<ADSC);
}
/*
int main(void)
{
	int adc;
	DDRD=0xff;
	DDRB=0x03;
	InitADC();
	setChannel(0);
    while(1)
    {
		adc=readADC();
		PORTD=(adc&0xFF);
		PORTB=(adc>>8)&0x03;
		
		if(a>=0 && a<32)
		PORTB=0x00;
		if(a>=32 && a<64)
		PORTB=0x80;
		if(a>=64 && a<96)
		PORTB=0xC0;
		if(a>=96 && a<128)
		PORTB=0xE0;
		if(a>=128 && a<160)
		PORTB=0xF0;
		if(a>=160 && a<192)
		PORTB=0xF8;
		if(a>=192 && a<220)
		PORTB=0xFC;
		if(a>=220 && a<255)
		PORTB=0xFE;
		if(a>=255)
		PORTB=0xFF;
    }
}
*/