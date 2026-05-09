#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t elapsedSeconds = 0;
volatile uint8_t nextLEDPin = 2;

void setUpIO()
{
    // Set up inputs and outputs
    DDRB = 0;
    DDRB &= ~(1 << PB0);

    DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
}

void startTimer()
{
    // Set up Timer 1
    TCCR1A = 0;
    TCCR1B = 0;
    TIMSK1 = 0;

    // CTC mode, no PWM
    TCCR1A &= ~(1 << COM1A0);
    TCCR1A &= ~(1 << COM1A1);
    TCCR1A &= ~(1 << WGM10);
    TCCR1A &= ~(1 << WGM11);
    TCCR1B |= (1 << WGM12);
    TCCR1B &= ~(1 << WGM13);

    // Enable match interrupts
    TIMSK1 |= (1 << OCIE1A);

    // Frequency = 16MHz / 1024 = 15625 Hz
    TCCR1B |= (1 << CS10);
    TCCR1B &= ~(1 << CS11);
    TCCR1B |= (1 << CS12);

    OCR1A = 15624; // Reset timer after 15625 ticks (1 second)
}

ISR(TIMER1_COMPA_vect)
{
    elapsedSeconds++;

    // Ten minutes have passed
    if (elapsedSeconds >= 600 && nextLEDPin < 8)
    {
        PORTD |= (1 << nextLEDPin);
        nextLEDPin++;
        elapsedSeconds = 0;
    }
}

int main()
{
    sei();

    uint8_t prevSwitchState = 0;
    uint8_t currentSwitchState = 0;
    setUpIO();
    startTimer();

    while(1)
    {
        currentSwitchState = (PINB >> PB0) & 1;
    
        if (prevSwitchState != currentSwitchState)
        {
            PORTD = 0;
            nextLEDPin = 2;
            
            cli(); // Disable interrupts temporarily to avoid race conditions
            elapsedSeconds = 0;
            sei();
            
            TCNT1 = 0;
            startTimer();
        }
        
        prevSwitchState = currentSwitchState;
    }

}