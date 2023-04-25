/*
 * main.c
 *
 * Created: 3/7/2023 1:05:50 PM
 *  Author: thome
 */ 



/*#define F_CPU 16000000
#define nuservo (1<<5)
//#define servo (1<<7)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
*/
/*
unsigned long full = 4999;
unsigned long low;
unsigned long high = 249;
*/


/*
void  timer1_init()
{
	TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);
	TCCR1A |= (1 << COM1A0);
	TCNT1 = 0;
	OCR1A = 24999;
	
}

int  main( void )
{
	DDRD |= (1 << 5);
	timer1_init();
	
	while (1)
	{
	}
}*/

/*
void  timer1_init()
{
	TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);
	TCNT1 = 0;
	
	OCR1A = 4999;
	
	TIMSK1 |= (1 << OCIE1A);
	sei();
}
ISR (TIMER1_COMPA_vect)
{
	PORTA ^= servo;
	if (PORTA & servo)
	{
		OCR1A = high;
	}
	else
	{
		low = full - high;
		OCR1A = low;
	}
    TCNT1 = 0;
}
		
int  main( void )
{
	//DDRA |= servo;
    TCCR1A = (1<<COM1A0);
    DDRA|= nuservo;
	
	timer1_init();
	int servotime = 300;
	while (1)
	{   
        while (servotime < 400)
        {
            high = servotime;
            servotime += 50;
            _delay_ms(500);
        }
        while(servotime > 200)
        {
            high = servotime;
            servotime -= 10;
            _delay_ms(20);
        }
        
        
	}
}

*/
/*
 * The program waits in a standby state until the button  is pressed. 
 * When this happens, the program generates a random waiting time and enters a waiting state for this amount of time.
 *  After the random waiting time, the program enters a waiting state where it waits for the user to press the button. 
 * When the button is pressed, the program calculates the user's reaction time and outputs this to a dial on a servo motor.
 * A crude system is implemented to keep track of records, while the system is running, it resets when off.
 * If the previous record is broken, the led flashes, and the dial on the servo goes to the new record.
 * Otherwise the dial goes to the current record and the led turns on unceremoniously.
 * The program uses two timers: Timer1 and Timer2. Timer1 is used to generate a PWM signal for controlling the servo motor.
 * Timer2 is used to generate an interrupt that is used to to generate an overflow count used in place of milliseconds.
 * The program uses this value  to determine how long the user takes to press the button.
 
 
 */


#define F_CPU 16000000				//Basic stuff
#define nuservo (1<<5)
#define LED (1<<1)
#define BUT (1<<0)


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>


int rando;		//used for random number
char seed = 0;					//used for seeding rnjesus
//int num = 0;					//unused, but a good place to mention servo values, 2600 for max, 1600 mid, 400 min, corresponds with milliseconds*2, as timer is set to "sawtooth pattern" going up and down at same rate
unsigned long OvF;			//Overflow counter
char status = 0;			//standby
const int initrecord = eeprom_read_word(0x00);// crude record tracking, resets when power is cut
int record = initrecord;

void  timer1_init()
{
    
    
	TCCR1B |= (1 << WGM13)|(1 << CS11);		//sets prescaler 64 and something with COM1A1
	ICR1 = 19999;
    TCCR1A |= (1 << COM1A1);// Enables compare match with OCR1A and sets pin to switch at bottom and prescaler to 8
    //OCR1A = num;			//forgot to use this
	
    
    TCCR2B |= (1<< CS20) | (1<< CS21);  //976 OvF per sec
    TIMSK2 |= (1<<TOIE0);				//Enables overflow interrupt for time2
	sei();								//sets interrupts
    
}


ISR (TIMER2_OVF_vect)
{
    OvF++;
	if(OvF > 30000) OvF = 0;
}


int main()  //assignment
{
    
    DDRD |= nuservo;	//data direction for servo
    DDRB |= LED | ~BUT; // data direction for led and button
    PORTB |= BUT;		// pullup for button
	DDRA = 3;
	

   
    
    
    
    timer1_init();
    TCNT2 = 0;
    TCNT1 = 0;
    OvF = 0;
    
	
	int reac = 0;			 //reaction time
    PORTB |= LED;
	OCR1A = 2600;			//resets servo
    _delay_ms(200);
	OCR1A = 0;
    while(1)
    {   
        PORTA = status;		//debugging to check state
        switch(status)
        {
            case 0: //standby
                
                if (!(PINB & BUT))		//bitwise if check to see if button is pressed
                {   
                   
                    _delay_ms(100);        
                    status = 1;
                    if (seed == 0)
					{
						seed = TCNT2;		//seeds rand() with timer value, 0-255
						srand(seed);
					}
					rando = rand() % 4000 + 2000;	//creates random number
                    PORTB &= ~LED;					//led off
                    OvF = 0;						//overflow and timers to zero
                    TCNT2 = 0;
                    
                }
                break;
                
            case 1: //Random waiting time
                if (OvF > rando)					//rnjesus take the wheel
                {
                    status = 2;
                    PORTB |= LED;					//led on
                    OvF = 0;						//overflow and timers to zero
                    TCNT2 = 0;
                }
                break;
                
            case 2: //waiting for press
				if (!(PINB & BUT))					
				{
					status = 3;
					reac = OvF * 2.254; //overflows are converted to ticks for servo and truncated
					PORTB &= ~LED;		//led off
				}
				if (OvF > 976)			// if one second is exceeded, it skips to the next phase
				{
					status =3;
					reac = 2200;
				}
                
                break;
            /*
			case 3:
				OCR1A = 2600 - reac; //sets pwm for servo
				_delay_ms(5000);
				OCR1A = 2600;		// resets servo to position 0
				_delay_ms(500);
				PORTB &= ~LED;		//led off
				if (reac < record)
				{
					record = reac; // changes record, if broken
					for (int i = 0; i < 20; i++)
					{
						PORTB ^= LED;			//led blinks
						_delay_ms(100);
					}	
				}
							//led goes back on
				OCR1A = 2600 - record;	//displaying record
				_delay_ms(4000);
				OCR1A = 2600;			//resetting
				_delay_ms(500);
				
				OCR1A = 0;
				reac = 0; //resets reaction time
				PORTB |= LED;
				status = 0;
				
				
                break;
			*/
			case 3:
				OCR1A = 2600 - reac; //sets pwm for servo
				_delay_ms(5000);
				OCR1A = 2600;		// resets servo to position 0
				_delay_ms(500);
				PORTB &= ~LED;		//led off
				if (reac < record)
				{
					record = reac; // changes record, if broken
					eeprom_update_word(0x00,record);
					for (int i = 0; i < 20; i++)
					{
						PORTB ^= LED;			//led blinks
						_delay_ms(100);
					}	
				}
							//led goes back on
				OCR1A = 2600 - record;	//displaying record
				_delay_ms(4000);
				OCR1A = 2600;			//resetting
				_delay_ms(500);
				
				OCR1A = 0;
				reac = 0; //resets reaction time
				PORTB |= LED;
				status = 0;
				
				
                break;
			
                
        }
        
    }
    return 0;
}
        
        
        
        
        
        
        
        
        
        
        
        
        
        /*
void but_init()						// leftover from attempt to use interrupt pin
{
    SREG |= (1<<I);
    EIMSK |= (1<<INT2);
    EICRA |= (0<<ISC20) | (0<<ISC21); //Low level
    //EICRA |= (1<<ISC20) | (0<<ISC21); //rising edge
    //EICRA |= (1<<ISC20) | (1<<ISC21); //falling edge
       
}


ISR (INT2_vect)          //External interrupt_zero ISR
{
	status++;
    
    if (status > 5) status = 0; //resets status, change 5 to fit
}
*/
        
        
        
        
 /*
                    _delay_ms(50);
                    PORTB |= LED;
                    _delay_ms(50);
                    PORTB &= ~LED;
                    _delay_ms(50);
                    PORTB |= LED;
                    _delay_ms(50);
                    PORTB &= ~LED;
                    _delay_ms(50);
                    PORTB |= LED;
                    _delay_ms(50);
                    PORTB &= ~LED;
                    _delay_ms(50);
                    PORTB ^= LED;
                    _delay_ms(500);
                    PORTB ^= LED;
                    _delay_ms(500);
                    */        
        
        
        
        
 /*       
        if (state == 'a')
        {
            OCR1A = num;
            num += 10;
            if (num > 2600) state = 'b';
            _delay_ms(20);
        }
        else if(state == 'b')
        {
            OCR1A = num;
            num -= 10;
            if (num < 400) state = 'c';
            _delay_ms(20);
        }
        else
        {
            
            num = 1600;
            OCR1A = num;
            _delay_ms(5000);
            state = 'a';
            _delay_ms(20);
        }
    num = 1600;
    OCR1A = num;
    _delay_ms(5000);
    
    _delay_ms(20);
    num = 2600;
    OCR1A = num;
    _delay_ms(5000);
    
    _delay_ms(20);
    num = 400;
    OCR1A = num;
    _delay_ms(5000);
    
    _delay_ms(20);
        
    }
    return 0;
}*/