#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h>

void setupADC()
{
    ADMUX = 0;
    ADMUX |= (1 << REFS0);

    ADCSRA = 0;
    ADCSRA |= 1 << ADEN;
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t readFromADC(uint8_t channel)
{
    channel &= 0b00001111;
    ADMUX = (ADMUX & ~0b00001111) | channel;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int main(void)
{
    float baselineTemp = 30.0;

    DDRD = (1 << DDD2) | (1 << DDD3) | (1 << DDD4);

    PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4));

    setupADC();

    while(1)
    {
        uint16_t sensorVal = readFromADC(0);

        float voltage = (sensorVal / 1024.0f) * 5.0f;
        float temperature = (voltage - 0.5f) * 100.0f;

        if (temperature < baselineTemp + 2) {
            PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4));
        } 
        else if (temperature >= baselineTemp + 2 && temperature < baselineTemp + 4) {
            PORTD |= (1 << PORTD2);
            PORTD &= ~((1 << PORTD3) | (1 << PORTD4));
        } 
        else if (temperature >= baselineTemp + 4 && temperature < baselineTemp + 6) {
            PORTD |= ((1 << PORTD2) | (1 << PORTD3));
            PORTD &= ~(1 << PORTD4);
        } 
        else if (temperature >= baselineTemp + 6) {
            PORTD |= ((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4));
        }

        _delay_ms(1);
    }
}