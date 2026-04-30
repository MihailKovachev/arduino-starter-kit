#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

void setupADC()
{
    ADMUX = 0;
    ADMUX = ADMUX & ~(1 << REFS1) | (1 << REFS0); // Select AVcc with external capacitor as reference

    ADCSRA = 0;
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC clock prescaler to clk/128 to get around 125kHz (between 50kHz and 200kHz)
    ADCSRA |= (1 << ADEN); // Enable the ADC

}

// Set up timer1 for PWM
void setupTimer1()
{
    DDRB |= (1 << DDB1); // Set Arduino Pin 9 (PB1) as output

    // Select Fast PWM mode with ICR1 as source for TOP
    TCCR1B |= (1 << WGM13) | (1 << WGM12); 
    TCCR1A = (TCCR1A & ~(1 << WGM10)) | (1 << WGM11);


    TCCR1A = TCCR1A & ~(1 << COM1A0) | (1 << COM1A1); // Set compare output mode to clear on match
    TCCR1B = (TCCR1B & ~(1 << CS12) & ~(1 << CS10)) | (1 << CS11); // Set prescaler to 8

    // f_pwm = f_clk / (prescaler * (1 + TOP))
    // 50 Hz = 16 000 000 Hz / (8 * (1 + TOP)) => TOP = 39 999
    // Set TOP to 39 999
    ICR1 = 39999;
}

const uint16_t minOnPulseDuration = 600; // 600us for 0 degrees
const uint16_t maxOnPulseDuration = 2400; // 2400us for 180 degrees

void setServoPosition(uint8_t angle)
{
    // map angle to pulse duration
    uint16_t pulseOnDuration = minOnPulseDuration + ((uint32_t)angle * (maxOnPulseDuration - minOnPulseDuration)) / 179;
    uint16_t ticks = pulseOnDuration * 2; // 16Mhz with Prescaler of clk/8 results in a tick duration of 0.5us, so pulseOnDuration / tickDuration

    OCR1A = ticks; // Set the output to low after the desired number of ticks.
}

uint16_t readFromADC(uint8_t channel)
{
    channel &= 0b00001111; // The 4 least significants bits determine MUX0, MUX1, MUX2 and MUX3 and thus the input channel
    ADMUX = (ADMUX & 0b11110000) | channel; // Wipe the 4 least significant bits and set them to channel
    ADCSRA |= (1 << ADSC); // Begin conversion
    while(ADCSRA & (1 << ADSC)); // Wait for conversion to finish
    return ADC;
}

int main()
{

    setupADC();
    setupTimer1();

    while(1)
    {
        uint16_t potentiometerValue = readFromADC(0); // Arduino pin A0 corresponds to ADC0
        uint8_t angle = ((uint32_t)potentiometerValue * 179) / 1023;
        
        setServoPosition(angle);
        _delay_ms(15);
        
    }

}