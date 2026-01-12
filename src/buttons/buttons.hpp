#pragma once
#include <stdint.h>

void buttons_init_irq(void);

uint8_t button0_get(void);
uint8_t button1_get(void);
uint8_t button2_get(void);
