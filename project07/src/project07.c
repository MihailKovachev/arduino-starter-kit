#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void setupADC()
{
    ADMUX = 0;
    ADMUX = ADMUX & ~(1 << REFS1) | (1 << REFS0); // Select AVcc with external capacitor as reference

    ADCSRA = 0;
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC clock prescaler to clk/128 to get around 125kHz (between 50kHz and 200kHz)
    ADCSRA |= (1 << ADEN);                                // Enable the ADC
}

void setupPWMTimer()
{
    TCCR1A = 0;
    TCCR1B = 0;

    DDRB |= (1 << DDB1); // Set Arduino Pin 9 (PB1) as output

    // Select Fast PWM mode with ICR1 as source for TOP
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    TCCR1A = (TCCR1A & ~(1 << WGM10)) | (1 << WGM11);

    TCCR1A = TCCR1A & ~(1 << COM1A0) | (1 << COM1A1);              // Set compare output mode to clear on match
    TCCR1B = (TCCR1B & ~(1 << CS12) & ~(1 << CS10)) | (1 << CS11); // Set prescaler to 8
}

uint16_t readFromADC(uint8_t channel)
{
    channel &= 0b00001111;                  // The 4 least significants bits determine MUX0, MUX1, MUX2 and MUX3 and thus the input channel
    ADMUX = (ADMUX & 0b11110000) | channel; // Wipe the 4 least significant bits and set them to channel
    ADCSRA |= (1 << ADSC);                  // Begin conversion
    while (ADCSRA & (1 << ADSC))
        ; // Wait for conversion to finish
    return ADC;
}

void playSound(uint16_t frequency)
{

    if (frequency != 0)
    {
        DDRB |= (1 << DDB1);

        // Frequency corresponds to the PWM period
        // f_pwm = f_clk / (prescaler * (1 + ICR1)); prescaler = 8, f_clk = 16MHz
        ICR1 = 2000000 / frequency - 1;
        OCR1A = (ICR1 + 1) / 2; // Keep duty cycle at 50
    }
    else
    {
        DDRB &= ~(1 << DDB1);
    }
}

void disablePWNTimer()
{
}

int main()
{
    // Enable interrupts
    sei();

    int notes[] = {262, 294, 330, 349};

    setupADC();

    setupPWMTimer();
    while (1)
    {
        uint16_t pinValue = readFromADC(0); // Arduino Pin A0 corresponds to ADC 0

        if (pinValue == 1023)
        {
            playSound(notes[0]);
        }
        else if (pinValue >= 990 && pinValue <= 1010)
        {
            playSound(notes[1]);
        }
        else if (pinValue >= 505 && pinValue <= 515)
        {
            playSound(notes[2]);
        }
        else if (pinValue >= 5 && pinValue <= 10)
        {
            playSound(notes[3]);
        }
        else
        {
            playSound(0);
        }

        _delay_ms(20);
    }
}