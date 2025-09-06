#ifndef SEVEN_SEGMENTS_H
#define SEVEN_SEGMENT_H

#include <stdint.h>

#define HIGH        1
#define LOW         0
#define SEGMENTS    8

typedef struct 
{
    // Punteros a funciones abstractas para control de pines
    void (*set_segment_pin)(uint8_t pin, uint8_t state);
    void (*set_digit_pin)(uint8_t pin, uint8_t state);
    void (*delay_ms)(uint32_t milliseconds);

    // Configuración de pines
    uint8_t segment_pins[8];    // Pines para segmentos a-g + dp
    uint8_t nDigits;            // Hasta 8 digitos
    uint8_t digit_pins[8];// Pines para selección de dígitos
    uint8_t common_cathode;     // 1 si es cátodo común, 0 si es ánodo común

} seven_segment_config_t;


void seven_segment_init(seven_segment_config_t config, uint8_t nDigits);
void seven_segment_display_number(uint16_t value);
void seven_segment_display_digit(uint8_t digit, uint8_t value);
void seven_segment_clear(void);
void seven_segment_set_brightness(uint8_t brightness);
void seven_segment_refresh(void);

#endif