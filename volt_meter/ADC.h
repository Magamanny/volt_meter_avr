/*
 * IncFile1.h
 *
 * Created: 9/22/2016 6:06:29 PM
 *  Author: Ali Harman
 */ 


#ifndef ADC_H_
#define ADC_H_
void setChannel(int8_t);
void adjADC(int);
void initADC();
void setInt();
void setADC();
int16_t getADC();
int16_t readADC();
#define LEFT 1
#define RIGHT 2
#endif /* ADC_H_ */