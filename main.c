#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "seven_segments.h"
#include "driver/timer.h"

static const char *TAG = "Display7Seg";

// Configuración de pines
#define SEGMENT_A_GPIO   GPIO_NUM_12
#define SEGMENT_B_GPIO   GPIO_NUM_14
#define SEGMENT_C_GPIO   GPIO_NUM_27
#define SEGMENT_D_GPIO   GPIO_NUM_26
#define SEGMENT_E_GPIO   GPIO_NUM_25
#define SEGMENT_F_GPIO   GPIO_NUM_33
#define SEGMENT_G_GPIO   GPIO_NUM_32  
#define SEGMENT_DP_GPIO  GPIO_NUM_13

#define DIGIT_1_GPIO     GPIO_NUM_15
#define DIGIT_2_GPIO     GPIO_NUM_2
#define DIGIT_3_GPIO     GPIO_NUM_18
#define DIGIT_4_GPIO     GPIO_NUM_19

#define COMMON_CATHODE   1  

// Array de pines de segmentos
const gpio_num_t segment_pins[] = {
    SEGMENT_A_GPIO, SEGMENT_B_GPIO, SEGMENT_C_GPIO, SEGMENT_D_GPIO,
    SEGMENT_E_GPIO, SEGMENT_F_GPIO, SEGMENT_G_GPIO, SEGMENT_DP_GPIO
};

// Array de pines de dígitos
const gpio_num_t digit_pins[] = {
    DIGIT_1_GPIO, DIGIT_2_GPIO, DIGIT_3_GPIO, DIGIT_4_GPIO
};

// Función para inicializar GPIO
void init_hardware_gpio(void) {
    // Configurar todos los pines como salidas
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = 0,
        .pull_up_en = 0
    };

    // Configurar pines de segmentos
    uint64_t segment_mask = (1ULL << SEGMENT_A_GPIO) | (1ULL << SEGMENT_B_GPIO) |
                           (1ULL << SEGMENT_C_GPIO) | (1ULL << SEGMENT_D_GPIO) |
                           (1ULL << SEGMENT_E_GPIO) | (1ULL << SEGMENT_F_GPIO) |
                           (1ULL << SEGMENT_G_GPIO) | (1ULL << SEGMENT_DP_GPIO);
    
    io_conf.pin_bit_mask = segment_mask;
    gpio_config(&io_conf);

    // Configurar pines de dígitos
    uint64_t digit_mask = (1ULL << DIGIT_1_GPIO) | (1ULL << DIGIT_2_GPIO) |
                         (1ULL << DIGIT_3_GPIO) | (1ULL << DIGIT_4_GPIO);
    
    io_conf.pin_bit_mask = digit_mask;
    gpio_config(&io_conf);

    ESP_LOGI(TAG, "Hardware GPIO inicializado");
}

// Implementaciones específicas para ESP32
void esp32_set_segment_pin(uint8_t pin, uint8_t state) {
    gpio_set_level(pin, state);
}

void esp32_set_digit_pin(uint8_t pin, uint8_t state) {
    gpio_set_level(pin, state);
}

void esp32_delay_ms(uint32_t milliseconds) {
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
}

// Variable global para la configuración
seven_segment_config_t display_config;

void setup_display_library(void) {
    // Configurar los pines hardware
    uint8_t segment_pins[8] = {
        SEGMENT_A_GPIO, SEGMENT_B_GPIO, SEGMENT_C_GPIO, SEGMENT_D_GPIO,
        SEGMENT_E_GPIO, SEGMENT_F_GPIO, SEGMENT_G_GPIO, SEGMENT_DP_GPIO
    };
    
    uint8_t digit_pins[4] = {
        DIGIT_1_GPIO, DIGIT_2_GPIO, DIGIT_3_GPIO, DIGIT_4_GPIO
    };

    // Llenar la estructura de configuración
    display_config.set_segment_pin = esp32_set_segment_pin;
    display_config.set_digit_pin = esp32_set_digit_pin;
    display_config.delay_ms = esp32_delay_ms;
    display_config.common_cathode = 1;  
    display_config.nDigits = 2;         

    // Copiar los arrays de pines
    for (int i = 0; i < 8; i++) {
        display_config.segment_pins[i] = segment_pins[i];
    }
    for (int i = 0; i < 4; i++) {
        display_config.digit_pins[i] = digit_pins[i];
    }

    // Inicializar la librería
    seven_segment_init(display_config, 4);
    ESP_LOGI(TAG, "Librería de display inicializada");
}


void IRAM_ATTR timer_isr(void *arg) {
    seven_segment_refresh(); 
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
}

void setup_hardware_timer(void) {
    timer_config_t config = {
        .divider = 80,           // 1MHz clock
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 2000); // 2ms = 500Hz
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, 0, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
}

void app_main(void) {
    ESP_LOGI(TAG, "Iniciando aplicación con número fijo");
    
    // 1. Inicializar hardware
    init_hardware_gpio();
    
    // 2. Configurar librería
    setup_display_library();
    setup_hardware_timer();

    int counter = 0;
    while (1) {
        seven_segment_display_number(counter);
        ESP_LOGI(TAG, "Mostrando: %d", counter);
        
        counter++;
        if (counter > 99) counter = 0;
        
        vTaskDelay(pdMS_TO_TICKS(500));  // Cambiar número cada 1 segundo
    }
}