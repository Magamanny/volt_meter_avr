/*
 * volt_meter.c
 *
 * Created: 5/19/2017 3:22:14 AM
 * Author : Manny
 */ 
#define D4 eS_PORTC5
#define D5 eS_PORTC4
#define D6 eS_PORTC3
#define D7 eS_PORTC2
#define RS eS_PORTB2
#define EN eS_PORTB1
#define BU PB0
#define BD PD6
#define ENT PD7
#define F_CPU 12000000
#include <avr/io.h>
#include"adc.h"
#include"lcd.h"
#include <stdlib.h>
#include <avr/interrupt.h>
//#include <avr/eeprom.h>
int getmax(int *,int);
double getClock();
int rise(int,int);
void ftoi(double,int*,int);
void Lcd4_Write_Float(int*,int);
void shift(int*,int);
int mean(int*,int);
void setVar();
int zero(int*,int);
int adc=0;
int sample[200];
int digit[10];
int avg[100];
double avgflo=0;
int temp;	// temporary int
double flo; // temporary float
double test;
float err=0;
char buffer[20];
static int x=0,y=0;
float a=2.3,b=4.6; // x1=a;x2=b
float m;
int s=0;
int z[100];
int w;
int main(void)
{
	DDRC|=(1<<PC5)|(1<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1);
	DDRB|=(1<<PB1)|(1<<PB2);
	DDRD|=(1<<PD0);
	m=150/(b-a);
	//setVar();
	//err=(float)eeprom_read_byte((uint8_t*)33);
	//PORTD|=(1<<BD);
	//PORTB|=(1<<BU);
	adjADC(RIGHT);
    initADC();
    setChannel(0);
	setInt();
	Lcd4_Init();
	Lcd4_Clear();
	// ADC clock
	flo=getClock();
	ftoi(flo,digit,2);
	Lcd4_Set_Cursor(1,7);
	Lcd4_Write_Float(digit,2);
	Lcd4_Write_String("khz");
	
	
	// ADC sample rate data
	flo=flo*(1000/30);
	ftoi(flo,digit,0);
	Lcd4_Set_Cursor(2,7);
	//itoa(digit[0],buffer,10);
	//Lcd4_Write_String(buffer);
	Lcd4_Write_Float(digit,0);
	Lcd4_Write_Char(' ');
	flo=flo*(0.02);
	itoa((int)flo,buffer,10);
	Lcd4_Write_String(buffer);
	
	/*
	ftoi(a,digit,2);
	Lcd4_Set_Cursor(2,7);
	Lcd4_Write_Float(digit,2);
	
	ftoi(b,digit,2);
	Lcd4_Set_Cursor(2,12);
	Lcd4_Write_Float(digit,2);
	*/
	sei();
	readADC();
    while (1) 
    {
		//flo=(float)adc;
		//flo=(double)adc;
		flo=(adc*5.0)/1024.0; // adc to volt
		test=flo; // Zero-error correction
		ftoi(test,digit,3);
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("    ");
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_Float(digit,3);
		/*
		if(flo<2.3)
			PORTC|=(1<<PC1);
		else if(flo>2.4)
			PORTC&=~(1<<PC1);
		*/
		//flo=(300*test-450)/3.5; // to voltage linear mapping
		//flo=(test-b+err)*100/(a); // vp
		//flo=(flo/1.414); // Vrms
		//test=4.3;
		//flo=a*test-b; //1st order
		//flo=10*flo;
		flo=m*(flo-b)+300;
		//flo=(flo*300)/(4.7+a);
		//flo=flo/1.411;	detail not required
		//flo=441.5-222.89*test+38.32*test*test; // 2nd order
		//flo=348.73*test-103.8*test*test+11.92*test*test*test-317.49; //3rd order
		//flo=9.68*test*test*test*test-117.38*test*test*test+516.1*test*test-891.53*test+521.5; //4th order
		temp=(int)(flo+0.5);// round up
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("    ");
		itoa(adc,buffer,10);
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String(buffer);
		/*
		// Moving Window
		//temp=getmax(avg,200);
		//avgflo=(temp*5.0)/1024.0;
		ftoi(avgflo,digit,3);
		Lcd4_Set_Cursor(1,7);
		Lcd4_Write_String("    ");
		Lcd4_Set_Cursor(1,7);
		Lcd4_Write_Float(digit,3);
		
		flo=m*(avgflo-b)+300;
		temp=(int)(flo+0.5);// round up
		Lcd4_Set_Cursor(2,7);
		Lcd4_Write_String("    ");
		itoa(temp,buffer,10);
		Lcd4_Set_Cursor(2,7);
		Lcd4_Write_String(buffer);
		*/
		/*
		ftoi(a,digit,2);
		Lcd4_Set_Cursor(2,7);
		Lcd4_Write_Float(digit,2);
		
		ftoi(b,digit,2);
		Lcd4_Set_Cursor(2,12);
		Lcd4_Write_Float(digit,2);
		*/
		
		//int w=zero(z,50);
		itoa(w,buffer,10);
		Lcd4_Set_Cursor(2,12);
		Lcd4_Write_String("   ");
		Lcd4_Set_Cursor(2,12);
		Lcd4_Write_String(buffer);
		
		_delay_ms(10);
		if(!(PIND&(1<<BD)))
		{
			while(!(PIND&(1<<BD)))
			;
			a-=0.1;
			//_delay_ms(500);
		}
		else if(!(PINB&(1<<BU)))
		{
			while(!(PINB&(1<<BU)))
			;
			a+=0.1;
			//_delay_ms(500);
		}
    }
}

ISR(ADC_vect)
{
	int temp;
	//adc=getADC();
	//PORTD^=(1<<PD0);
	sample[x]=getADC();
	z[y]=getADC();
	//Moving window
	//shift(avg,200);
	//avg[199]=getADC();
	//
	if(y>=40-1)
	{
		w=zero(z,40);
		y=0; //reset
	}
	else
		y++;
	if(x>=200-1){
	adc=getmax(sample,200); // change
	x=0; // reset x
	//PORTC^=(1<<PC1);
	avg[s++]=adc;
	if(s>10){
		temp=mean(avg,20);
		avgflo=(temp*5.0)/1024.0;
		if(s>20)
		s=0;
	}
	}
	else
	x++;
	//if(x==100)
	//zero(sample);
	
	setADC();
}
int getmax(int *arr,int l)
{
	int max=0;
	for (int i=0;i<l;i++)
	{
		if(arr[i]>max)
		{
			max=arr[i]; //new max
		}
	}
	return max;
}
double getClock()
{	double x; 
	x=F_CPU/1000.0;
	x=x/64.0;
	return x;
}
int rise(int b,int e)
{
	int result=b;
	if(e<=0)
	return 1;
	for(int i=1;i<e;i++)
	{
		result*=b;
	}
	return result;
}

void ftoi(double num,int* arr,int prec)
{
	arr[0]=(int)(num);
	//printf("%d %d\n",arr[0],rise(10,0));
	for(int i=1;i<prec+1;i++)
	{
		arr[i]=(int)((num-arr[0])*rise(10,i));
		arr[i]=arr[i]%10;
		//printf("%d %d\n",arr[i],rise(10,i));
	}
}

void setVar()
{
	int8_t flag;
	//eeprom_busy_wait();
	//eeprom_write_byte((uint8_t*)33,1);
}

void Lcd4_Write_Float(int *temp,int n)
{
	itoa(temp[0],buffer,10);
	Lcd4_Write_String(buffer);
	if(n!=0)
	Lcd4_Write_Char('.');
	for(int i=1;i<=n;i++)
	{
	itoa(temp[i],buffer,10);
	Lcd4_Write_String(buffer);
	}
}

void shift(int* arr,int l)
{
	for(int i=1;i<l;i++)
	{
		arr[i-1]=arr[i];
	}
}
int mean(int* arr,int l)
{
	int sum=0;
	for(int i=0;i<l;i++)
	{
		sum+=arr[i];
	}
	return sum/l;
}
int zero(int* arr,int l)
{
	int w=0;
	for(int i=0;i<l;i++)
	{
		if(arr[i]<180)
		w++;
	}
	if(w>30)
	{
		PORTC|=(1<<PC1);
	}
	else
	{
		PORTC&=~(1<<PC1);
	}
	return w;
}