/*
    Прошивка к простому RGB контроллеру,
    управляемому 2мя аналоговыми потенциометрами.

    МК - ATtiny85(45)
    Clock - 4 MHz (Internal)
*/
#include <util/delay.h>

/* Указываем пины ШИМ и АЦП */
#define R_PWM_PIN   1
#define G_PWM_PIN   0
#define B_PWM_PIN   4
#define COLOR_PIN   A3
#define BRIGHT_PIN  A1

uint8_t r_pwm = 0, g_pwm = 0, b_pwm = 0;          // Компоненты RGB

void setup() {
  pinMode(R_PWM_PIN, OUTPUT);                     // Настраиваем пины с ШИМ на выход
  pinMode(G_PWM_PIN, OUTPUT);
  pinMode(B_PWM_PIN, OUTPUT);

  TCCR0B = (TCCR0B & 0b11111000) | 1;             // ШИМ 16 кгц для таймера 0
  TCCR1 = (TCCR1 & 0b11110000) | 1;               // ШИМ 16 кгц для таймера 1
}

void loop() {
  /* Получаем аналоговые значения с потенциометров */
  uint8_t bright = analogRead(BRIGHT_PIN) >> 2;   // Получаем 8-ми битные значения
  uint8_t color = analogRead(COLOR_PIN) >> 2;     // Для этого удаляем младшие 2 бита

  /* Получаем RGB, корректируем и применяем яркость */
  makeColor(color);                               // Преобразуем оттенок в RGB компоненты
  bright = getCRT(bright);                        // Пропускаем яркость через CRT
  makeBright(bright);                             // Применяем яркость на компоненты RGB

  /* Выводим RGB на LED драйвер в виде ШИМ сигнала */
  analogWrite(R_PWM_PIN, r_pwm);                  // Генерируем ШИМ сигналы
  analogWrite(G_PWM_PIN, g_pwm);
  analogWrite(B_PWM_PIN, b_pwm);

  _delay_ms(20);                                  // Софт-delay, независимый от таймера 0
}

void makeColor(uint8_t color) {                   // 8-ми битное цветовое колесо
  uint8_t shift = 0;                              // Цветовой сдвиг
  if (color > 170) {                              // Синий - фиолетовый - красный
    shift = (color - 170) * 3;                    // Получаем цветовой сдвиг 0 - 255
    r_pwm = shift, g_pwm = 0, b_pwm = ~shift;     // Получаем компоненты RGB
  } else if (color > 85) {                        // Зеленый - голубой - синий
    shift = (color - 85) * 3;                     // Получаем цветовой сдвиг 0 - 255
    r_pwm = 0, g_pwm = ~shift, b_pwm = shift;     // Получаем компоненты RGB
  } else {                                        // Красный - оранжевый - зеленый
    shift = (color - 0) * 3;                      // Получаем цветовой сдвиг 0 - 255
    r_pwm = ~shift, g_pwm = shift, b_pwm = 0;     // Получаем компоненты RGB
  }
}

void makeBright(uint8_t brig) {                   // Применяем яркость на компоненты RGB
  r_pwm = ((uint16_t)r_pwm  * (brig + 1)) >> 8;   // Применяем яркость
  g_pwm = ((uint16_t)g_pwm  * (brig + 1)) >> 8;   // Применяем яркость
  b_pwm = ((uint16_t)b_pwm  * (brig + 1)) >> 8;   // Применяем яркость
}

uint8_t getCRT(uint8_t val) {                     // Кубическая CRT гамма
  if (!val) return 0;                             // Принудительный 0
  return ((long)val * val * val + 130305) >> 16;  // Рассчитываем, используя полином
}
