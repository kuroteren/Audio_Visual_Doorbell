/*_______________________________________________________________________________

Title:
	chipTunes.h v1.0

Description:
	Play chip tunes and tones on a piezo buzzer using PWM
	
	For complete details visit
	https://www.programming-electronics-diy.xyz/2021/02/playing-music-and-tones-using-piezo.html


Author:
 	I. Liviu
	istrateliviu24@yahoo.com


NOTICE
--------------------------------------------------------------------
Free for private, non-commercial use


Copyright (c) 2021 I. Liviu, www.programming-electronics-diy.xyz
__________________________________________________________________________________*/

#ifndef CHIP_TUNES_H
#define CHIP_TUNES_H


/*************************************************************
	SYSTEM SETTINGS
**************************************************************/
#define CHIP_TUNES_TIMER1_PRESCALER		8

#define CHIP_TUNES_TIMER0 				0 // Use timer0
#define CHIP_TUNES_TIMER2 				2 // Use timer2

#define __CHIP_TUNES_START_MARKER__			5000
#define __CHIP_TUNES_END_MARKER__			5001
#define __CHIP_TUNES_GOTO_MARKER__			5002

/*************************************************************
	INCLUDES
**************************************************************/
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "pitches.h"
#include "tunes.h"


/*************************************************************
	USER SETUP SECTION
**************************************************************/
#define CHIP_TUNES_TIMER 		CHIP_TUNES_TIMER0 // Which timer to use [CHIP_TUNES_TIMER0 or CHIP_TUNES_TIMER1]

#define CHIP_TUNES_OC1A_DDR 	DDRB
#define CHIP_TUNES_OC1A_PORT 	PORTB
#define CHIP_TUNES_OC1A_PIN  	PB1
#define CHIP_TUNES_OC1B_DDR 	DDRB
#define CHIP_TUNES_OC1B_PIN  	PB2

#define CHIP_TUNES_BLINK_LED	0
#define CHIP_TUNES_LED_DDR		DDRB
#define CHIP_TUNES_LED_PORT		PORTB
#define CHIP_TUNES_LED_PIN		5


/*************************************************************
	SYSTEM SETTINGS
**************************************************************/
#if CHIP_TUNES_TIMER == CHIP_TUNES_TIMER0

	// Timer0
	#if F_CPU >= 18000000 // 18MHz - 20MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS02))
		#define CHIP_TUNES_PRESCALER 		256
		
	#elif F_CPU >= 4000000 // 4MHz - 18MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS01) | _BV(CS00))
		#define CHIP_TUNES_PRESCALER 		64
		
	#elif F_CPU >= 1000000 // 1MHz - 4MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS01))
		#define CHIP_TUNES_PRESCALER 		8
		
	#elif F_CPU >= 125000 // 125KHz - 1MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS00))
		#define CHIP_TUNES_PRESCALER 		1
		
	#else
		#error "F_CPU not supported"
	#endif

	#define CHIP_TUNES_REG_TCCRA			TCCR0A
	#define CHIP_TUNES_REG_TCCRB			TCCR0B
	#define CHIP_TUNES_REG_TIMSK			TIMSK0
	#define CHIP_TUNES_REG_OCR				OCR0A
	#define CHIP_TUNES_BIT_WGM				WGM01
	#define CHIP_TUNES_BIT_OCIE				OCIE0A
	#define CHIP_TUNES_ISR_VECT				TIMER0_COMPA_vect

	#define CHIP_TUNES_SET_TCCRA()			(CHIP_TUNES_REG_TCCRA = _BV(CHIP_TUNES_BIT_WGM))
	#define CHIP_TUNES_SET_TCCRB()			(CHIP_TUNES_REG_TCCRB = CHIP_TUNES_CLOCKSEL)

#elif CHIP_TUNES_TIMER == CHIP_TUNES_TIMER2

	// Timer2

	#if F_CPU >= 9600000	// 9.6MHz - 20MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS22) | _BV(CS20))
		#define CHIP_TUNES_PRESCALER 		128
		
	#elif F_CPU >= 4800000	// 4.8MHz - 9.6MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS22))
		#define CHIP_TUNES_PRESCALER 		64
		
	#elif F_CPU >= 4000000 // 4MHz - 4.8MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS21) | _BV(CS20))
		#define CHIP_TUNES_PRESCALER 		32
		
	#elif F_CPU >= 1000000 // 1MHz - 4MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS21))
		#define CHIP_TUNES_PRESCALER 		8
		
	#elif F_CPU >= 125000 // 125KHz - 1MHz
		#define CHIP_TUNES_CLOCKSEL 		(_BV(CS20))
		#define CHIP_TUNES_PRESCALER 		1
		
	#else
		#error "F_CPU not supported"
	#endif

	#define CHIP_TUNES_REG_TCCRA			TCCR2A
	#define CHIP_TUNES_REG_TCCRB			TCCR2B
	#define CHIP_TUNES_REG_TIMSK			TIMSK2
	#define CHIP_TUNES_REG_OCR				OCR2A
	#define CHIP_TUNES_BIT_WGM				WGM21
	#define CHIP_TUNES_BIT_OCIE				OCIE2A
	#define CHIP_TUNES_ISR_VECT				TIMER2_COMPA_vect

	#define CHIP_TUNES_SET_TCCRA()			(CHIP_TUNES_REG_TCCRA = _BV(CHIP_TUNES_BIT_WGM))
	#define CHIP_TUNES_SET_TCCRB()			(CHIP_TUNES_REG_TCCRB = CHIP_TUNES_CLOCKSEL)

#else
	#error "Bad CHIP_TUNES_TIMER set"
#endif


#if CHIP_TUNES_TIMER1_PRESCALER == 8
	#define CHIP_TUNES_TIMER1_CLOCKSEL 		(_BV(CS11))
#endif


/*************************************************************
	GLOBAL VARIABLES
**************************************************************/
volatile char CHIP_TUNES_PLAYING = 0;
static char ToneOn = 0;
static int *CurrentSong;
static int *StartMarker;
static int *GoBackMarker;
static unsigned int Wholenote;
static uint16_t delay_ms = 0;


const int *chipTunes_Playlist[] = {
	star_wars_theme,
	tetris_theme,
	mario_main_theme,
	mario_underworld_melody,
	fur_elise,
	cannon_in_d_pachelbel,
	greensleeves,
	happy_birthday,
	ode_to_joy
};

const uint8_t chipTunes_Tempo[] = {
	108,	// star_wars_theme
	144, 	// tetris_theme
	120,	// mario_main_theme
	120,	// mario_underworld_melody
	80, 	// fur_elise
	100,	// cannon_in_d_pachelbel
	70,		// greensleeves
	140,	// happy_birthday
	114		// ode_to_joy
};



/*************************************************************
	FUNCTION PROTOTYPES
**************************************************************/
void chipTunes_Init(void);
void chipTunes_Tone(uint16_t tone, uint16_t duration_ms);
void chipTunes_Play(const int *pMusicNotes, uint8_t tempo);
void chipTunes_Stop(void);
char chipTunes_IsPlaying(void);

void chipTunes_alert_alarm(uint8_t vuvuzela);


/*************************************************************
	FUNCTIONS
**************************************************************/
void chipTunes_Init(void){
	CHIP_TUNES_OC1A_DDR |= (1 << CHIP_TUNES_OC1A_PIN);
	CHIP_TUNES_OC1B_DDR |= (1 << CHIP_TUNES_OC1B_PIN);
	
	#if CHIP_TUNES_BLINK_LED == 1
		CHIP_TUNES_LED_DDR |= (1 << CHIP_TUNES_LED_PIN);
		CHIP_TUNES_LED_PORT &= ~(1 << CHIP_TUNES_LED_PIN);
	#endif
	
	// Enable timer 1 Compare Output in toggle mode
	// OC1B - Same as OC1A but inverted
	TCCR1A |= (1 << COM1A1) | (1 << COM1B0) | (1 << COM1B1);
	
	// Mode 8, PWM, Phase and Frequency Correct (TOP value is ICR1)
	TCCR1B |= (1 << WGM13);
	
	// Timer 0 or 2 settings
	CHIP_TUNES_SET_TCCRA();
	CHIP_TUNES_REG_TIMSK = _BV(CHIP_TUNES_BIT_OCIE);
	CHIP_TUNES_REG_OCR = ((F_CPU / CHIP_TUNES_PRESCALER) / 1000) - 1; // 1000 is the frequency for 1ms interrupt
	sei();
}


void chipTunes_Tone(uint16_t tone, uint16_t duration_ms){
	CHIP_TUNES_PLAYING = 1;
	
	// Calculate PWM top value to set the frequency
	uint16_t pwm_top_val = F_CPU / (2 * CHIP_TUNES_TIMER1_PRESCALER * (uint32_t)tone); // Mode 8, PWM, Phase and Frequency Correct
	
	// Set duty cycle
	OCR1A = OCR1B = pwm_top_val / 2;
	
	// Set frequency
	ICR1H = (pwm_top_val >> 8);
	ICR1L = pwm_top_val;
	
	// Start timer 1
	TCCR1B |= CHIP_TUNES_TIMER1_CLOCKSEL;
	
	if(duration_ms){
		delay_ms = duration_ms;
		ToneOn = 1;
		
		// Start timer 0 or 2
		// Set prescaler and start the timer
		CHIP_TUNES_SET_TCCRB();
	}
}


void chipTunes_Play(const int *pMusicNotes, uint8_t tempo){
	if(CHIP_TUNES_PLAYING == 0){
		CHIP_TUNES_PLAYING = 1;
		
		// Save current song and tempo
		CurrentSong = (int *)pMusicNotes;
		
		// Calculate the duration of a whole note in ms (60s/tempo)*4 beats
		Wholenote = ((uint32_t)60000 * 4) / tempo;
		
		// Start timer 1
		TCCR1B |= CHIP_TUNES_TIMER1_CLOCKSEL;
		
		// Start timer 0 or 2
		// Set prescaler and start the timer
		CHIP_TUNES_SET_TCCRB();
	}
}


void chipTunes_Stop(void){
	// Stop timer 1
	TCCR1B &= ~CHIP_TUNES_TIMER1_CLOCKSEL;
	OCR1A = 0;
	OCR1B = 0xFFFF;
	TCNT1 = 0;
	CHIP_TUNES_OC1A_PORT &= ~(1 << CHIP_TUNES_OC1A_PIN);
	
	// Stop timer 0 or 2
	CHIP_TUNES_REG_TCCRB = 0;
	
	CHIP_TUNES_PLAYING = 0;
	
	#if CHIP_TUNES_BLINK_LED == 1
		CHIP_TUNES_LED_PORT &= ~(1 << CHIP_TUNES_LED_PIN);	
	#endif
}


char chipTunes_IsPlaying(void){
	return CHIP_TUNES_PLAYING;
}



void chipTunes_alert_alarm(uint8_t vuvuzela){
	uint16_t f;
	
	for(f = 100; f < 1000; f++){
		_delay_ms(1);
		
		// This prevents stuttering
		if(vuvuzela == 0) while(TCNT1 > 0);
		
		chipTunes_Tone(f, 0);
		
		// Reset timer
		if(vuvuzela) TCNT1 = 0;
	}
	
	chipTunes_Stop();
	_delay_ms(200);
}



/*************************************************************
	ISR Handlers
**************************************************************/

ISR(CHIP_TUNES_ISR_VECT){
	
	static char repeat_fragment = 0;
	static int16_t duration = 0;
	uint16_t note = 0;
	uint16_t pwm_top_val = 0;
	
	#if CHIP_TUNES_BLINK_LED == 1
		static uint16_t led_delay_ms = 0;
	#endif
	
	if(delay_ms == 0){
		// 150us @ 8MHz with led
		// 118us @ 8MHz no led
		
		if(ToneOn){
			ToneOn = 0;
			chipTunes_Stop();
			return;
		}
		
		// Check if the array pointer is not at the end
		if(pgm_read_word(CurrentSong)){
			note = pgm_read_word(CurrentSong);
			
			
			// Save the start address where the fragment starts
			if(note == __CHIP_TUNES_START_MARKER__){
				
				// This is not a note - move pointer to a note
				CurrentSong++;
				note = pgm_read_word(CurrentSong);
				StartMarker = CurrentSong;
				
			// Save the end address where the fragment ends	
			}else if(note == __CHIP_TUNES_END_MARKER__){
				
				if(repeat_fragment == 0){
					// This is not a note - move pointer to a note
					CurrentSong++;
					note = pgm_read_word(CurrentSong);
				}else{
					repeat_fragment = 0;
					
					// Go back to where it was before repeating the fragment
					if(pgm_read_word(GoBackMarker)){
						CurrentSong = GoBackMarker;
						note = pgm_read_word(CurrentSong);
					}else{
						chipTunes_Stop();
						return;
					}
				}
			}
			
			// Repeat the fragment
			if(note == __CHIP_TUNES_GOTO_MARKER__){
			
				// Save the current position
				CurrentSong++;
				GoBackMarker = CurrentSong;
				repeat_fragment = 1;
				
				// Move pointer to the start of the fragment
				CurrentSong = StartMarker;
				note = pgm_read_word(CurrentSong);
				
			}
			
			
			// Move to the next array element to find duration
			CurrentSong++;
			
			// Get note duration
			// If is greater than 1 (not pause) and less than 33 (not a note) then is duration
			if((int16_t)pgm_read_word(CurrentSong) < 33 && (int16_t)pgm_read_word(CurrentSong) != REST && (int16_t)pgm_read_word(CurrentSong) != 0){
				duration = (int16_t)pgm_read_word(CurrentSong);
				
				if(duration > 0){
					duration = Wholenote / duration;
				}else{
					// Dotted notes are represented with negative durations
					duration = Wholenote / (0 - duration); // 0 - duration: convert duration from negative to positive
					
					// Increases the duration with one half for dotted notes
					duration = duration + (duration / 2);
				}
				
				// Go to next note for next time
				CurrentSong++;
			}
			
			
			if(note == REST){
				// Pause, do not generate any sound
				OCR1A = 0;
				OCR1B = 0xFFFF;
				
			}else{
				
				// Calculate PWM top value to set the frequency
				pwm_top_val = F_CPU / (2 * CHIP_TUNES_TIMER1_PRESCALER * (uint32_t)note); // Mode 8, PWM, Phase and Frequency Correct
				
				// Set duty cycle (volume)
				OCR1A = OCR1B = pwm_top_val / 2; // 50% duty cycle
				
				// Set frequency
				ICR1H = (pwm_top_val >> 8);
				ICR1L = pwm_top_val;
				
				// Reset timer
				TCNT1 = 0;
			}
			
			
			// Set note or rest delay
			delay_ms = duration;
			
			#if CHIP_TUNES_BLINK_LED == 1
				if(note != REST){
					CHIP_TUNES_LED_PORT |= (1 << CHIP_TUNES_LED_PIN);
					led_delay_ms = ((uint32_t)duration * 90) / 100;
				}
			#endif
			
		}else{
		
			chipTunes_Stop();
		}
		
	}else{
	
		delay_ms--;
		
		#if CHIP_TUNES_BLINK_LED == 1
			if(led_delay_ms > 0){
				led_delay_ms--;
			}else{
				CHIP_TUNES_LED_PORT &= ~(1 << CHIP_TUNES_LED_PIN);
			}
		
		#endif
	}
	
}

#endif