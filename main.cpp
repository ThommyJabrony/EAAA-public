#define F_CPU 16000000
#define BUT (1<<3)		//defining button

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "light_ws2812.c"
#include "light_ws2812.h"

#include "utils.h"
#include "classy.h"

unsigned long OvF = 0;
char keepJumping = 0;



char death[256]   // array that spells out "you died" in dark souls fashion
{

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	
	
	0,1,1,0,1,1,1,0,1,1,1,0,0,1,1,0,
	0,1,0,1,0,0,1,0,0,1,0,0,0,1,0,1,

	1,0,1,0,0,1,1,0,0,1,0,0,0,1,1,0,
	0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,1,

	1,0,1,0,1,1,1,0,0,0,0,0,0,0,1,0,
	0,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,

	0,0,0,0,1,1,0,0,1,0,0,1,0,0,1,0,
	1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0,

	0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,1,
	1,0,1,0,0,1,1,0,0,1,0,0,1,0,0,0,
	
};


ISR (TIMER2_OVF_vect)	//overflow interrupt function
{
    OvF++;				//adds one to overflow long
	if(OvF > 99999999) OvF = 0;	//resets if it passes a certain value
}

void mainMan()
{
	bool flag = false;			//flag used for determining whether character is live or dead
	
	guy Guy;
	wall Wall;
	int  guyx = Guy.colissionX();		//fetches x,y vals for guy and wall. y for guy is lower block, y for wall is the top
	int  wallx= Wall.colissionX();
	char guyy = Guy.colissionY();
	char wally= Wall.colissionY();
	_delay_ms(2000);
	
	while(1)
	{
		/*************************The wall*/
		Wall.move(OvF);
		
		
		/*************************jumping*/
		if(!(PINB & BUT) && (keepJumping == 0))
		{
			keepJumping = Guy.jump(OvF);
			
			if(flag)
			{
				flag = false;
				clearAll(80,256);
				Wall.resetSpeed();
				Wall.seed = OvF;		//seeds rand() with timer value, 0-255
				srand(Wall.seed);
			}
		}
		else if(keepJumping > 0)	keepJumping = Guy.jump(OvF);
		
		
		
		
		/*************************collisions*/
		guyx = Guy.colissionX();
		wallx= Wall.colissionX();
		guyy = Guy.colissionY();
		wally= Wall.colissionY();
		
		
		if(guyx == wallx && guyy <= wally && flag == false)			//triggers if the guy isn't completely over the wall.
		{
			arrayPlot(false,death);	//smiley(0);
			
			Guy.done();
			keepJumping = 0;
			
			flag = true;
		}
		

	}
	
}


int main()
{	
	TCCR2B |= (1<< CS20) | (1<< CS21);  //976 OvF per sec
    TIMSK2 |= (1<<TOIE0);				//Enables overflow interrupt for time2
	sei();								//sets interrupts
    
	DDRB |= ~BUT;
	
	PORTB |= BUT;
	mainMan();
	
}
