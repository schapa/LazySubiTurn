/*
 * api.h
 *
 *  Created on: Dec 10, 2015
 *      Author: shapa
 */

#ifndef API_H_
#define API_H_


#include "stm32f0xx_gpio.h"

void initGpio(void);
void initInterrupts(void);

void setLeftOutputState(FunctionalState state);
void setRightOutputState(FunctionalState state);
void setHazzardOutputState(FunctionalState state);


#endif /* API_H_ */
