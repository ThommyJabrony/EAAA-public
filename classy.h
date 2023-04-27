/* 
 * File:   guy.cpp
 * Author: tj
 * 
 * Created on April 13, 2023, 1:51 PM
 */


class guy
{	
private:									//inits default values
	int x;
	char y;
	char headR; char headG; char headB;
	char bodyR; char bodyG; char bodyB;
	char jumpState;
	unsigned long prevOvF;
	
public:
	
	guy(void)								//defines defaults
	{
		x = 4;	y = 0;							//defining position
		headR = 140; headG = 70; headB = 10;	//defining colours
		bodyR = 20; bodyG = 50; bodyB = 120;
		jumpState = 0;
		prevOvF = 0;
		
		generate();
	}
	
	void generate()									//generates the guy buy turning on leds where he's supposed to be
	{	
		singleColour( x, y-1, 0,	 0,		0);		//terminates guy in previous position
		singleColour( x, y+2, 0,	 0,		0);

		singleColour( x, y  , bodyR, bodyG, bodyB); //creates guy in new position
		singleColour( x, y+1, headR, headG, headB);
		SafeSetLeds(256);							// calls function that sets led's with regard to not overusing power, to protect leds from overheating and power supply from over drawign.
	}
	
	void vert (bool direc)							// simply moves y coord
	{
		if(direc) 	y += 1;
		else		y -= 1;
		generate();
	}
	
	
	
	
	char jump(unsigned long OvF)					// a long long state machine. Could be rewritten to simulate gravity, but it doesn't make a lot of sense with such a small display.
	{
		switch(jumpState)
		{
			case 0:
				vert(true);
				jumpState = 1;
				prevOvF = OvF;
				break;
			case 1:
				if(OvF > prevOvF + 30)
				{
					vert(true);
					jumpState = 2;
					prevOvF = OvF;
				}
				break;
			case 2:
				if(OvF > prevOvF + 40)
				{
					vert(true);
					jumpState = 3;
					prevOvF = OvF;
				}
				break;
			case 3:
				if(OvF > prevOvF + 60)
				{
					vert(true);
					jumpState = 4;
					prevOvF = OvF;
				}
				break;
			case 4:
				if(OvF > prevOvF + 70)
				{
					vert(true);
					jumpState = 5;
					prevOvF = OvF;
				}
				break;
			case 5:
				if(OvF > prevOvF + 100)
				{
					vert(true);
					jumpState = 6;
					prevOvF = OvF;
				}
				break;
			case 6:
				if(OvF > prevOvF + 120)
				{
					vert(true);
					jumpState = 7;
					prevOvF = OvF;
				}
				break;
			case 7:
				if(OvF > prevOvF + 160)
				{
					vert(true);
					jumpState = 8;
					prevOvF = OvF;
				}
				break;
			case 8:
				if(OvF > prevOvF + 160)
				{
					vert(false);
					jumpState = 9;
					prevOvF = OvF;
				}
				break;
			case 9:
				if(OvF > prevOvF + 120)
				{
					vert(false);
					jumpState = 10;
					prevOvF = OvF;
				}
				break;
			case 10:
				if(OvF > prevOvF + 100)
				{
					vert(false);
					jumpState = 11;
					prevOvF = OvF;
				}
				break;
			case 11:
				if(OvF > prevOvF + 70)
				{
					vert(false);
					jumpState = 12;
					prevOvF = OvF;
				}
				break;
			case 12:
				if(OvF > prevOvF + 60)
				{
					vert(false);
					jumpState = 13;
					prevOvF = OvF;
				}
				break;
			case 13:
				if(OvF > prevOvF + 40)
				{
					vert(false);
					jumpState = 14;
					prevOvF = OvF;
				}
				break;
			case 14:
				if(OvF > prevOvF + 30)
				{
					vert(false);
					jumpState = 15;
					prevOvF = OvF;
				}
				break;
			case 15:
				if(OvF > prevOvF + 20)
				{
					vert(false);
					jumpState = 0;
				}
				break;
		}
		if( OvF + 100000 < prevOvF) prevOvF = OvF+200;
		
		return jumpState;		
		
	}
	
	char colissionX()
	{
		return x;
	}
	char colissionY()
	{
		return y;
	}
	void done()										//turns guy off
	{	singleColour( x, y-1, 0,	 0,		0);
		singleColour( x, y+2, 0,	 0,		0);
		singleColour( x, y,	  0,	 0,		0);
		singleColour( x, y+1, 0,	 0,		0);
	}
	
	
};








class wall
{
	
private:
	int x; // init standard values, x is int to allow it to be -1
	char y;
	char R; char G; char B;
	
	char jumpState;
	unsigned long prevOvF;
	char speed;
	
public:
	char seed;
	
	wall(void)
	{
		x = 15;	y = 0;				//defining standard values
		R = 40; G = 140; B = 140;
		jumpState = 0;
		prevOvF = 0;
		speed = 200;				// speed is set to five blocks per second
				
		seed = TCNT2;		//seeds rand() with timer value, 0-255
		srand(seed);
		
		generate(true);
	}
	
	
	void generate(bool restart)		//function used to generate new wall and delete old
	{	
		for(char i = 0; i <= y; i++)		// iterates over x+1 and everything under  y and plots wall as 0's
		{
			singleColour(x+1, i, 0, 0, 0);
		}
		
		
		if (restart)		//resets x, decolours wall in pos 1, generates new wall height, and generates new wall colour
		{
			x = 15;
			for(char i = 0; i < y; i++) singleColour(0, i, 0, 0, 0);
			y = rand() % 6+1;
			
			if (speed > 125) speed -= 2;	//speed increases faster in the beginning
			else			 speed -= 1;
			R = rand() % 150 + 5;
			G = rand() % 150 + 5;
			B = rand() % 150 + 5;
		}
		
			
		for(char i = 0; i <= y; i++)			//  iterates over x+1 and everything under  y and plots wall as generated colour
		{
			singleColour(x, i, R, G, B);
		}
		
		SafeSetLeds(256);
	}
	
	void resetSpeed() {speed = 200;}		//does what the name implies
	
	void move(unsigned long OvF)			
	{
		if (OvF > prevOvF + speed)			// moves only when intended delay has been  reached. See arduino's non blocking delay for reference
		{	
			x = x - 1;
			if (x < 0)	generate(true);
			else		generate(false);
			prevOvF = OvF;
		}
		if( OvF + 100000 < prevOvF) prevOvF = OvF+200;
	}
	
	char colissionX()
	{
		return x;
	}
	char colissionY()
	{
		return y;
	}
};