#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN PB6
#define NUM_LEDS 8

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Función para interpolar colores (de r1,g1,b1 a r2,g2,b2 según factor 0-1)
uint32_t lerpColor(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2, float t) {
  uint8_t r = r1 + t * (r2 - r1);
  uint8_t g = g1 + t * (g2 - g1);
  uint8_t b = b1 + t * (b2 - b1);
  return strip.Color(r, g, b);
}

void ledTask(void *pvParameters) {
  const uint32_t colors[][3] = {
    {255, 0, 0},   // rojo
    {0, 255, 0},   // verde
    {0, 0, 255}    // azul
  };
  const int numColors = sizeof(colors) / sizeof(colors[0]);

  while (true) {
    // Recorrer colores con interpolación suave
    for (int c = 0; c < numColors; c++) {
      int nextC = (c + 1) % numColors;
      // Hacer transición gradual entre colores
      for (int step = 0; step <= 100; step++) {
        float t = step / 100.0f;
        uint32_t currentColor = lerpColor(
          colors[c][0], colors[c][1], colors[c][2],
          colors[nextC][0], colors[nextC][1], colors[nextC][2],
          t
        );

        // Barrido adelante
        for (int i = 0; i < NUM_LEDS; i++) {
          strip.clear();
          strip.setPixelColor(i, currentColor);
          strip.show();
          vTaskDelay(pdMS_TO_TICKS(50));
        }
        // Barrido atrás
        for (int i = NUM_LEDS - 2; i > 0; i--) {
          strip.clear();
          strip.setPixelColor(i, currentColor);
          strip.show();
          vTaskDelay(pdMS_TO_TICKS(50));
        }
      }
    }
  }
}

void setup() {
  strip.begin();
  strip.show();

  xTaskCreate(ledTask, "LED Task", 512, NULL, 1, NULL);
}

void loop() {
  // Nada aquí, FreeRTOS maneja la multitarea
}
