/*
 * main.c
 *
 * Created: 5/11/2022 9:51:29 AM
 *  Author: Xiangsheng
 */ 

#include <xc.h>

#include <stdio.h>
#include <stdlib.h>

#include "avr.h"
#include "lcd.h"
#include "avr.c"
#include "lcd.c"
/**************************************** Project 4: Volt-meter ****************************************/

// Display all 4 readouts
typedef struct
{
	short InstantV;
	short MaxV;
	short MinV;
	double AvgV;
} readouts;

// Sampling rate is at least 2 samples/second -> every 500ms
const int samplingRate = 500; //ms

// Computing Average: Sum and totalCount
unsigned long Sum = 0;
unsigned long totalCount = 0;


int is_pressed(int r, int c)
{
	// Set all 8 GPIOs to N/C
	DDRC = 0;
	PORTC = 0;
	
	// Set r to "0"
	SET_BIT(DDRC, r);
	CLR_BIT(PORTC, r);
	
	// Set c to "w1"
	CLR_BIT(DDRC, c + 4);
	SET_BIT(PORTC, c + 4);
	avr_wait(1);
	
	if (GET_BIT(PINC, c + 4) == 0) //value of c == 0
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int get_key()
{
	int i, j;
	for (i=0; i < 4; i++)
	{
		for (j=0; j < 4; j++)
		{
			if (is_pressed(i, j))
			{
				return 4 * i + j + 1;
			}
		}
	}
	return 0;
}

char int_to_char(int key)
{
	// Transform integer to actual character of the key for the functionality purpose.
	switch (key)
	{
		case 1:
		return '0' + key;
		case 2:
		return '0' + key;
		case 3:
		return '0' + key;
		case 4:
		return 'A';
		case 5:
		return '0' + key - 1;
		case 6:
		return '0' + key - 1;
		case 7:
		return '0' + key - 1;
		case 8:
		return 'B';
		case 9:
		return '0' + key - 2;
		case 10:
		return '0' + key - 2;
		case 11:
		return '0' + key - 2;
		case 12:
		return 'C';
		case 13:
		return '*';
		case 14:
		return '0';
		case 15:
		return '#';
		case 16:
		return 'D';
	}
	return '$';
}

void print_Welcome()
{
	for (int i=0; i < 10; i++)
	{
		avr_wait(200);
		CLR_BIT(PORTB, 4);
		avr_wait(200);
		SET_BIT(PORTB, 4);
	}
	
	// LCD display
	char topLine[17];
	char bottomLine[17];
	
	lcd_pos(0, 0);
	sprintf(topLine, "---Welcome---");
	lcd_puts2(topLine);
	lcd_pos(1, 0);
	sprintf(bottomLine, "Sys is loading");
	lcd_puts2(bottomLine);
	
	avr_wait(2000);
	lcd_clr();
}


int get_sample()
{
	// configure the ADC -> read from PA0
	ADMUX = 0b01000000;
	// start conversion -> single conversion
	ADCSRA = 0b11000000;
	// wait for conversion result: while(bit 6 of ADCSRA)
	while (GET_BIT(ADCSRA, 6) == 1);
	return ADC;
}


int main ()
{
	SET_BIT(DDRB, 4);
	SET_BIT(PORTB, 4);
	
	avr_init();
	lcd_init();
	lcd_clr();
	print_Welcome();
	
	readouts showVolts;
	// ADCSRA: ADC Control and Status Register
	// SET_BIT(ADCSRA, 6);
	// Initialize all 4 readouts
	showVolts.InstantV = 0;
	showVolts.MaxV = 0;
	showVolts.MinV = 1024;
	showVolts.AvgV = 0;
	Sum = 0;
	totalCount = 0;
	
	while (1)
	{
		int key = get_key();
		char actual_key = int_to_char(key);

		switch(actual_key)
		{
			case 'B':
			// Resets max\min\avg to blank -> how about instant volt?
			lcd_clr();
			showVolts.InstantV = 0;
			showVolts.MaxV = 0;
			showVolts.MinV = 1024;
			Sum = 0;
			totalCount = 0;
			break;

		}
		
		//sampling here
		avr_wait(samplingRate);
		// Get Intsant
		showVolts.InstantV = get_sample();

		// Get Max
		if(showVolts.InstantV > showVolts.MaxV)
		{
			showVolts.MaxV = showVolts.InstantV;
		}
		// Get Min
		if(showVolts.InstantV < showVolts.MinV)
		{
			showVolts.MinV = showVolts.InstantV;
		}

		// For Calculating Avg
		Sum += (showVolts.InstantV / 1023.0) * 5;
		totalCount += 1;
        showVolts.AvgV = (double)Sum / totalCount;
		// Display Volts
	// LCD display
	char topLine[17];
	char bottomLine[17];
	
	// Converting from normalized to volts?
	lcd_pos(0, 0);
	sprintf(topLine, "%.2f V  %.2f V", (showVolts.MaxV / 1023.0) * 5, (showVolts.MinV / 1023.0) * 5);
	lcd_puts2(topLine);

	lcd_pos(1, 0);
	// Display the avg volt:
	sprintf(bottomLine, "%.2f V  %.2f V", (showVolts.InstantV / 1023.0) * 5, showVolts.AvgV);
	lcd_puts2(bottomLine);
	}
	return 0;
}

/**************************************** Project 4 ****************************************/