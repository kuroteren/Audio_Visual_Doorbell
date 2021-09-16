/*
 * main.c
 *
 * Created: 8/19/2021 10:01:01 PM
 *  Author: Alexander
 */ 

#include <xc.h>
#include <avr/io.h>
#include <avr/delay.h>

// Borrowed from Alien Transducer
// https://www.programming-electronics-diy.xyz/2021/02/playing-music-and-tones-using-piezo.html
#include "chipTunes.h"
#include "tunes.h"
#include "pitches.h"

int main(void)
{
	// SET PINB0 to Input
	DDRB &= ~(1 << PIND0);
	
	// Set led pins
	DDRB |= (1 << PINB3); //Blue
	DDRB |= (1 << PINB4); //Green
	DDRB |= (1 << PINB5); //Yellow
	DDRB |= (1 << PINB2); //Red
	
	// Set up buzzer
	chipTunes_Init();
		
    while(1)
    {
		uint8_t button = PIND & (1 << PIND0);
		
        if(button)
		{
			chipTunes_Play(fur_elise, 1);
			
			for(int i = 0; i < 10; i++)
			{
				PORTB ^= (1 << PINB3);
				_delay_ms(3000);
				PORTB ^= (1 << PINB3);
				PORTB ^= (1 << PINB4);
				_delay_ms(3000);
				PORTB ^= (1 << PINB4);
				PORTB ^= (1 << PINB5);
				_delay_ms(3000);
				PORTB ^= (1 << PINB5);
				PORTB ^= (1 << PINB2);
				_delay_ms(3000);
				PORTB ^= (1 << PINB6);
				PORTB ^= (1 << PINB3);
				_delay_ms(3000);
				PORTB ^= (1 << PINB3);
				PORTB ^= (1 << PINB4);
				_delay_ms(3000);
				PORTB ^= (1 << PINB4);
				PORTB ^= (1 << PINB5);
				_delay_ms(3000);
				PORTB ^= (1 << PINB5);
				PORTB ^= (1 << PINB2);
				_delay_ms(3000);
				PORTB ^= (1 << PINB2);
				_delay_ms(3000);
				// Turn all lights off
				PORTB &= ~(1 << PINB2);
				PORTB &= ~(1 << PINB3);
				PORTB &= ~(1 << PINB4);
				PORTB &= ~(1 << PINB5);
						
			}
		}
		
		chipTunes_Stop();
				
    }
}