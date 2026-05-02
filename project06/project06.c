#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

void setupADC()
{
    ADMUX = 0;
    ADMUX = ADMUX & ~(1 << REFS1) | (1 << REFS0); // Select AVcc with external capacitor as reference

    ADCSRA = 0;
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC clock prescaler to clk/128 to get around 125kHz (between 50kHz and 200kHz)
    ADCSRA |= (1 << ADEN);                                // Enable the ADC
}

void startCalibrationTimer()
{
    TCCR1A = 0;
    TCCR1B = 0;

    // Set timer to clear timer on compare match (CTC) mode
    TCCR1A = TCCR1A & ~(1 << WGM11) & ~(1 << WGM10);
    TCCR1B = TCCR1B & ~(1 << WGM13);
    TCCR1B |= (1 << WGM12);

    // Set prescaler to 1024
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TCCR1B &= ~(1 << CS11);

    // The timer ticks F_CPU / 1024 times in a single second
    OCR1A = F_CPU / 1024 - 1;

    // Enable the timer interrupt
    TIMSK1 = 0;
    TIMSK1 |= (1 << OCIE1A);
}

volatile uint8_t elapsedCalibrationSeconds = 0;
volatile uint8_t calibrationFinished = 0;

ISR(TIMER1_COMPA_vect)
{
    if (!calibrationFinished)
        elapsedCalibrationSeconds++;

    if (elapsedCalibrationSeconds >= 5)
        calibrationFinished = 1;
}

void setupPWMTimer()
{
    TIMSK1 = 0; // Disable the interrupt used during calibration
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

void calibratePhototransistor(uint16_t *phototransistorMin, uint16_t *phototransistorMax)
{
    // Turn on calibration indicator LED
    DDRB |= (1 << PB5);
    PORTB |= (1 << PORTB5);

    *phototransistorMin = 1023;
    *phototransistorMax = 0;
    while (calibrationFinished != 1)
    {
        uint16_t phototransistorValue = readFromADC(0); // ADC 0 is Arduino Pin A0

        if (phototransistorValue > *phototransistorMax)
            *phototransistorMax = phototransistorValue;
        if (phototransistorValue < *phototransistorMin)
            *phototransistorMin = phototransistorValue;
    }

    // Turn off calibration indicator LED
    PORTB &= ~(1 << PORTB5);
}

void setSoundFrequency(uint16_t frequency)
{
    // Frequency corresponds to the PWM period
    // f_pwm = f_clk / (prescaler * (1 + ICR1)); prescaler = 8, f_clk = 16MHz
    ICR1 = 2000000 / frequency - 1;
    OCR1A = (ICR1 + 1) / 2; // Keep duty cycle at 50
}

int main()
{
    // Enable interrupts
    sei();

    const uint16_t minSoundFrequency = 50;
    const uint16_t maxSoundFrequency = 4000;

    setupADC();

    uint16_t phototransistorMin;
    uint16_t phototransistorMax;
    startCalibrationTimer();
    calibratePhototransistor(&phototransistorMin, &phototransistorMax);

    setupPWMTimer();
    while (1)
    {
        uint16_t phototransistorValue = readFromADC(0); // Arduino Pin A0 corresponds to ADC 0

        if (phototransistorValue < phototransistorMin)
            phototransistorValue = phototransistorMin;
        else if (phototransistorValue > phototransistorMax)
            phototransistorValue = phototransistorMax;

        uint16_t frequency = (uint32_t)(phototransistorValue - phototransistorMin) * (maxSoundFrequency - minSoundFrequency) / (phototransistorMax - phototransistorMin) + minSoundFrequency;
        setSoundFrequency(frequency);
        _delay_ms(20);
    }
}