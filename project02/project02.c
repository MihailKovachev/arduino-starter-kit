#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

int main(void)
{

    DDRD &= ~(1 << DDD2);
    DDRD |= 1 << DDD3;
    DDRD |= 1 << DDD4;
    DDRD |= 1 << DDD5;

    uint8_t switchState = 0;

    while(1)
    {
        switchState = PIND & (1 << PIND2);

        if (switchState == 0)
        {
            PORTD |= 1 << PORTD3;
            PORTD &= ~(1 << PORTD4);
            PORTD &= ~(1 << PORTD5);
        }
        else {
            PORTD &= ~(1 << PORTD3);
            PORTD &= ~(1 << PORTD4);
            PORTD |= 1 << PORTD5;

            _delay_ms(250);
            PORTD |= 1 << PORTD4;
            PORTD &= ~(1 << PORTD5);
            _delay_ms(250);
        }

    }
}