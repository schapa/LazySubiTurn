/*
 * api.h
 *
 *  Created on: Dec 10, 2015
 *      Author: shapa
 */

#ifndef API_H_
#define API_H_


#include "stm32f0xx_gpio.h"
#include <stdbool.h>

#define PORT GPIOA

#define LEFT_INPUT GPIO_Pin_10
#define RIGHT_INPUT GPIO_Pin_2
#define HAZARD_INPUT GPIO_Pin_4
#define CONFIG_INPUT GPIO_Pin_5

#define LEFT_INTERRUPT EXTI_Line10
#define RIGHT_INTERRUPT EXTI_Line2
#define HAZARD_INTERRUPT EXTI_Line4
#define CONFIG_INTERRUPT EXTI_Line5

#define LEFT_OUTPUT GPIO_Pin_9
#define RIGHT_OUTPUT GPIO_Pin_1
#define HAZARD_OUTPUT GPIO_Pin_3


#define TICKS_PER_SECOND 100


void initGpio(void);
void initInterrupts(void);
void reconfigureInterruptsTrigger(bool isConfigure);

void setLeftOutputState(FunctionalState state);
void setRightOutputState(FunctionalState state);
void setHazardOutputState(FunctionalState state);
FunctionalState getLeftInputState(void);
FunctionalState getRightInputState(void);
FunctionalState getHazardInputState(void);

#endif /* API_H_ */
