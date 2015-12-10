/*
 * gpio.cpp
 *
 *  Created on: Dec 10, 2015
 *      Author: shapa
 */

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_exti.h"

#include "api.h"


#define LEFT_INPUT GPIO_Pin_10
#define RIGHT_INPUT GPIO_Pin_2
#define HAZARD_INPUT GPIO_Pin_4

#define LEFT_INTERRUPT EXTI_Line10
#define RIGHT_INTERRUPT EXTI_Line2
#define HAZARD_INTERRUPT EXTI_Line4

#define LEFT_OUTPUT GPIO_Pin_9
#define RIGHT_OUTPUT GPIO_Pin_1
#define HAZZARD_OUTPUT GPIO_Pin_3


void initGpio(void) {

	GPIO_InitTypeDef initStructure;
	initStructure.GPIO_Mode = GPIO_Mode_IN;
	initStructure.GPIO_OType = GPIO_OType_PP;
	initStructure.GPIO_Pin = LEFT_INPUT | RIGHT_INPUT | HAZARD_INPUT;
	initStructure.GPIO_PuPd = GPIO_PuPd_UP;
	initStructure.GPIO_Speed = GPIO_Speed_Level_1;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_Init(GPIOA, &initStructure);

	initStructure.GPIO_Pin = LEFT_OUTPUT | RIGHT_OUTPUT | HAZZARD_OUTPUT;
	initStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &initStructure);
}

void initInterrupts(void) {
	EXTI_InitTypeDef initStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	initStructure.EXTI_Line = LEFT_INTERRUPT | RIGHT_INTERRUPT | HAZARD_INTERRUPT;
	initStructure.EXTI_LineCmd = ENABLE;
	initStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	initStructure.EXTI_Trigger = EXTI_Trigger_Falling;

	EXTI_Init(&initStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_EnableIRQ(EXTI2_3_IRQn);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void setLeftOutputState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, LEFT_OUTPUT, val);
}

void setRightOutputState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, RIGHT_OUTPUT, val);
}

void setHazzardOutputState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, HAZZARD_OUTPUT, val);
}

extern "C" void EXTI2_3_IRQHandler(void) {
	if(EXTI_GetITStatus(RIGHT_INTERRUPT) != RESET) {

	    EXTI_ClearITPendingBit(RIGHT_INTERRUPT);
	}

}

extern "C" void EXTI4_15_IRQHandler(void) {

	if(EXTI_GetITStatus(LEFT_INTERRUPT) != RESET) {

	    EXTI_ClearITPendingBit(LEFT_INTERRUPT);
	}

	if(EXTI_GetITStatus(HAZARD_INTERRUPT) != RESET) {

	    EXTI_ClearITPendingBit(HAZARD_INTERRUPT);
	}

}
