#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <io.h>
#include <iostream.h>

struct VIDEO
{
	unsigned char symb;
	unsigned char attr;
};

VIDEO  far* screen = (VIDEO far*)MK_FP(0xB800, 0);

void interrupt newInt9(...); 		
void interrupt(*oldInt9)(...); 		 
void indicator(unsigned char mask);	 
void blinking(void);				
void calc(int value);

int quitFlag = 0; 				
int blinkingON = 0;					


void main() 
{
	oldInt9 = getvect(0x09);		
	setvect(0x09, newInt9); 	

	while (!quitFlag) {		
		if (blinkingON) 			
			blinking(); 		   
	}
	setvect(0x09, oldInt9);			
	return;
}

void interrupt newInt9(...) 		
{
	unsigned char value = 0;
	oldInt9();
	value = inp(0x60); 				

	if (value == 0x01)				
		quitFlag = 1; 		

	if (value == 0x1C && blinkingON == 0) 
		blinkingON = 1; 				  
	calc(value);
	screen->symb = ' ';
	screen->attr = 0x5E;
	screen++;


	outp(0x20, 0x20); 				
}

void calc(int value)
{
	int base = 16;
	if (value == 0)
		return;
	int rem = value % base;
	if (rem <= 9)
	{
		calc(value / base);
		screen->symb = rem + '0';
		screen->attr = 0x5E;
		screen++;
	}
	else
	{
		calc(value / base);
		screen->symb = rem - 10 + 'A';
		screen->attr = 0x5E;
		screen++;
	}
}


void indicator(unsigned char mask) 
{
	int flg = 0;			
	while ((inp(0x64) & 0x02) != 0x00); 
	outp(0x60, 0xED);				

	for (int i = 9; i > 0; i--)
	{
		if ((inp(0x60) == 0xFA))	
		{
			flg = 1;
			break;
		}
		delay(600);
	}
	if (flg) 
	{
		while ((inp(0x64) & 0x02) != 0x00);
		outp(0x60, mask);
	}
	delay(100);
}

void blinking()						// LED flashing function
{
	indicator(0x02);                // Turn on Num Lock
	delay(300);

	indicator(0x04);				// Turn on Caps Lock
	delay(300);

	indicator(0x6);					// Turn on Num Lock и Caps Lock
	delay(300);

	indicator(0x00);				// Turn off all indicators
	delay(100);

	indicator(0x06);				// Turn on all indicators
	delay(200);

	indicator(0x00);				// Turn off all indicators
	blinkingON = 0;
}