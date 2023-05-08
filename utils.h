/* 
 * File:   utils.h
 * Author: tj
 *
 * Created on April 13, 2023, 1:45 PM
 */

#ifndef UTILS_H
#define	UTILS_H

struct cRGB led[256];
const float maxCurrent = 350;

void clearAll(int a, int b)
{
	for(int x = a; x < b; x ++)
	{
		led[x].r=0;led[x].g=0;led[x].b=0;		// sets every led's vals to zero.
	}
	ws2812_setleds(led,256);					// applies vals
}

void setColour(int led0, char R, char G, char B)
{
	led[led0].r=R;
	led[led0].g=G;
	led[led0].b=B;
}


void SafeSetLeds(int count)											//Sets leds with powerlimit applied
{															//not perfect, works recursively on previously limited leds.
	float currMax = maxCurrent * 12;						//converts 20 mA per led to 255 values, real number is 12.75, but I truncated it as a buffer
	float currActual = 0;
	for (int i = 0; i < 256; i++)
	{
		currActual += led[i].r; currActual += led[i].g; currActual += led[i].b;
	}
	if (currMax < currActual)								//checks if power target is exceeded
	{
		float frac = currMax / currActual;					//creates a ratio to multiply all leds, that use over 1.9 mA, that burn too much current, with
		for (int i = 0; i < 256; i++)
		{
			if(led[i].r > 24) led[i].r*=frac;				//all led's can run simultaneously if they're under 1.9 mA, so that's the cutoff.
			if(led[i].g > 24) led[i].g*=frac;
			if(led[i].b > 24) led[i].b*=frac;
		}
	}
	ws2812_setleds(led,count);								//sets leds
}



void singleColour(int x, int y, uint8_t R, uint8_t G, uint8_t B)
{
	int z = 0;
	
	//error handling
	if (x > 15) x = 15;
	if (y > 15) y = 15;
	if (x < 0)	y =  0;
	if (x < 0)	y =  0;
	
	//the biz
	z += y*16;
	if (y % 2 == 0)		z += x;				//checks for even and determines if x axis goes up or down
	else				z += 15-x;
	
	setColour(z, R, G, B);
	

}


/*void singleSetter(int x, int y, uint8_t R, uint8_t G, uint8_t B, int count)
{
	int z = 0;
	
	//error handling
	if (x > 15) x = 15;
	if (y > 15) y = 15;
	if (x < 0)	y =  0;
	if (x < 0)	y =  0;
	
	//the biz
	z += y*16;
	if (y % 2 == 0)		z += x;				//checks for even and determines if x axis goes up or down
	else				z += 15-x;
	
	
	
	setColour(z, R, G, B);
	SafeSetLeds(count);

}*/

void arrayPlot(bool a, char smiley[256], char colour)

{ 
	int red = 255;
	int green = 0;
	int blue = 0;
		
	if ( colour == 'r')
	{
		red = 255;
		green = 0;
		blue = 0;
	}
	if(colour == 'y')
	{
		red = 150;
		green = 150;
		blue = 0;
	}
	for(int x = 80; x < 255; x++)
	{	
		/*
		if(smiley[x] == 0)
		{
			led[x].r=0;
			led[x].g=0;
			led[x].b=0;
		}*/
		if(smiley[x] == 1)
		{
			led[x].r=red;
			led[x].g=green;
			led[x].b=blue;
		}
		
		
		if(a)			//used to set leds on in a flow
		{
		_delay_ms(10);
		SafeSetLeds(x);
		}
		
		
	}
	SafeSetLeds(255);
	_delay_ms(1000);
}



#endif	/* UTILS_H */



