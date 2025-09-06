#include "seven_segments.h"
#include <stdlib.h>

static seven_segment_config_t config;
uint8_t current_digit = 0;
uint16_t current_value = 0;     //Numero a refrescar en memoria

//PATRONES A B C D E F G DP
static const uint8_t digit_patterns[11] = {
    0b11111100, //0
    0b01100000, //1
    0b11011010, //2
    0b11110010, //3
    0b01100110, //4
    0b10110110, //5
    0b10111110, //6
    0b11100000, //7
    0b11111110, //8
    0b11110110, //9
    0b00000000  //OFF
};

void seven_segment_init(seven_segment_config_t cfg, uint8_t nDigits) {
        
    uint8_t i;
    
    if (nDigits <= 0 || nDigits > 8)
        return;
    
    config = cfg;
    config.nDigits = nDigits;

    //Inicializamos digitos
    for(i = 0; i < nDigits; i++) {
        config.set_digit_pin(config.digit_pins[i], config.common_cathode ? 1 : 0); 
    }
    //Apagamos segmentos
    for (i = 0; i < SEGMENTS; i++) {
        config.set_segment_pin(config.segment_pins[i], LOW);
    }
}

void seven_segment_clear(void) {
    uint8_t i;

    for (i = 0; i < config.nDigits; i++) {
        config.set_digit_pin(config.digit_pins[i], config.common_cathode ? 1 : 0);
    }
    
    for (i = 0; i < SEGMENTS; i++) {
        config.set_segment_pin(config.segment_pins[i], LOW);
    }
}

void seven_segment_display_digit(uint8_t digit, uint8_t value) {
    if (digit >= config.nDigits || value > 9) {
            return;
    }

    for (int i = 0; i < config.nDigits; i++) {
        config.set_digit_pin(config.digit_pins[i], config.common_cathode ? 1 : 0);
    }
    
    uint8_t pattern = digit_patterns[value];
    for (int i = 0; i < SEGMENTS; i++) {
        uint8_t state = (pattern >> (7-i)) & 0x01;
        if (!config.common_cathode) {
            state = !state; // Invertir para ánodo común
        }
        config.set_segment_pin(config.segment_pins[i], state);
    }

    config.set_digit_pin(config.digit_pins[digit], config.common_cathode ? 0 : 1);
}

void seven_segment_display_number(uint16_t value) {
    current_value = value;
}

void seven_segment_refresh(void) {

    uint8_t digits[config.nDigits];
    uint16_t temp = current_value;  
    
    for (int i = 0; i < config.nDigits; i++) {
        digits[i] = 0;
    }
   
    for (int pos = config.nDigits - 1; pos >= 0 && temp > 0; pos--) {
        digits[pos] = temp % 10;
        temp /= 10;
    }

    for (int i = 0; i < config.nDigits; i++) {
        config.set_digit_pin(config.digit_pins[i], config.common_cathode ? 1 : 0);
    }

    uint8_t value = digits[current_digit];
    seven_segment_display_digit(current_digit, value);

    current_digit++;
    if (current_digit >= config.nDigits) {
        current_digit = 0;
    }
}