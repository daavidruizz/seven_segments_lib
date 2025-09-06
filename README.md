```markdown
# Display 7 Segmentos Multiplexados

Librería portable para control de displays de 7 segmentos con multiplexación. Soporta cátodo común y ánodo común mediante configuración hardware-agnóstica.

## 📋 Características

- ✅ Control de 1-8 dígitos multiplexados
- ✅ Soporte para cátodo común y ánodo común
- ✅ Arquitectura portable mediante callbacks
- ✅ Patrones predefinidos para todos los dígitos (0-9)
- ✅ Optimizada para máximo rendimiento
- ✅ Fácil integración con cualquier microcontrolador

```

## 🔧 Configuración Requerida

### Estructura de Configuración

```c
typedef struct {
    // Callbacks de hardware (IMPLEMENTAR POR PLATAFORMA)
    void (*set_segment_pin)(uint8_t pin, uint8_t state);
    void (*set_digit_pin)(uint8_t pin, uint8_t state);
    void (*delay_ms)(uint32_t milliseconds);
    
    // Configuración de pines
    uint8_t segment_pins[8];  // Orden: A, B, C, D, E, F, G, DP
    uint8_t digit_pins[4];    // Pines de control de dígitos
    
    // Configuración eléctrica
    uint8_t common_cathode;   // 1 = Cátodo común, 0 = Ánodo común
    uint8_t nDigits;          // Número de dígitos (1-8)
    
} seven_segment_config_t;
```

## 🚀 Uso Rápido

### 1. Implementar Callbacks de Hardware

```c
// Ejemplo para ESP32
void esp32_set_segment_pin(uint8_t pin, uint8_t state) {
    gpio_set_level(pin, state);
}

void esp32_set_digit_pin(uint8_t pin, uint8_t state) {
    gpio_set_level(pin, state);
}

void esp32_delay_ms(uint32_t milliseconds) {
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
}
```

### 2. Inicializar Display

```c
seven_segment_config_t config = {
    .set_segment_pin = esp32_set_segment_pin,
    .set_digit_pin = esp32_set_digit_pin,
    .delay_ms = esp32_delay_ms,
    .segment_pins = {12, 14, 27, 26, 25, 33, 32, 13},
    .digit_pins = {15, 2, 18, 19},
    .common_cathode = 1,
    .nDigits = 4
};

seven_segment_init(config);
```

### 3. Sistema de Refresh (CRÍTICO)

**Opción A: Software Polling**
```c
while(1) {
    seven_segment_refresh();
    delay_ms(5);  // 200Hz refresh
}
```

**Opción B: Timer Hardware (RECOMENDADO)**
```c
// Implementar en interrupción de timer
void IRAM_ATTR timer_isr(void) {
    seven_segment_refresh();  // LLAMAR SIEMPRE
    clear_timer_interrupt();
}

// Configurar timer a 2-5ms (200-500Hz)
setup_hardware_timer(5000);  // 5ms = 200Hz
```

### 4. Mostrar Números

```c
// Número estático
seven_segment_display_number(1234);

// Contador
for(int i = 0; i <= 9999; i++) {
    seven_segment_display_number(i);
    delay_ms(1000);
}
```

## 📊 Patrones de Segmentos

Los segmentos siguen el orden: **A B C D E F G DP**

```c
static const uint8_t digit_patterns[11] = {
    0b11111100, // 0 - A,B,C,D,E,F=1; G,DP=0
    0b01100000, // 1 - B,C=1; otros=0
    0b11011010, // 2 - A,B,D,E,G=1; C,F,DP=0
    0b11110010, // 3 - A,B,C,D,G=1; E,F,DP=0
    0b01100110, // 4 - B,C,F,G=1; A,D,E,DP=0
    0b10110110, // 5 - A,C,D,F,G=1; B,E,DP=0
    0b10111110, // 6 - A,C,D,E,F,G=1; B,DP=0
    0b11100000, // 7 - A,B,C=1; D,E,F,G,DP=0
    0b11111110, // 8 - Todos los segmentos=1; DP=0
    0b11110110, // 9 - A,B,C,D,F,G=1; E,DP=0
    0b00000000  // OFF - Todos apagados
};
```

## 🎯 Funciones Principales

### `void seven_segment_init(seven_segment_config_t cfg)`
Inicializa el display y configura los pines GPIO.

### `void seven_segment_display_number(uint16_t value)`
Establece el número a mostrar (0-9999).

### `void seven_segment_refresh(void)`
**FUNCIÓN CRÍTICA**: Realiza multiplexación. Debe llamarse cada 2-10ms.

### `void seven_segment_clear(void)`
Apaga todos los segmentos y dígitos.

### `void seven_segment_display_digit(uint8_t digit, uint8_t value)`
Muestra un dígito específico en posición específica.

## ⚠️ Troubleshooting

### Display parpadea
**Solución**: Aumentar frecuencia de refresh o usar timer hardware

### Números incorrectos
**Solución**: Verificar orden de pines y configuración common_cathode

### Segmentos apagados
**Solución**: Revisar patrones y niveles lógicos

### Alto consumo de CPU
**Solución**: Implementar timer hardware instead of polling

## 📝 Notas Importantes

1. **Refresh constante**: `seven_segment_refresh()` debe llamarse periódicamente
2. **Timer hardware**: Recomendado para mejor rendimiento
3. **Niveles lógicos**: Invertidos automáticamente para ánodo común
4. **Portabilidad**: Implementar callbacks para nueva plataforma

## 🔄 Ejemplo de Flujo

```c
// 1. Configurar hardware
// 2. Implementar callbacks
// 3. Inicializar librería
// 4. Configurar timer hardware (opcional)
// 5. Llamar seven_segment_refresh() periódicamente
// 6. Usar seven_segment_display_number() para cambiar valores
```

## 📞 Soporte

Para issues y preguntas, revisar:
1. Configuración de pines correcta
2. Implementación de callbacks
3. Frecuencia de refresh adecuada
4. Patrones de segmentos correctos

## 📄 Licencia

Libre para uso comercial y personal. Atribución opcional.
```
