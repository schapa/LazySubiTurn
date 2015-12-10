/*
 * process.cpp
 *
 *  Created on: Dec 10, 2015
 *      Author: shapa
 */

#include <stdint.h>
#include "api.h"

typedef struct {
	uint32_t debounce;
	bool isActive;
} state_t, *state_p;

static state_t left;
static state_t right;
static state_t hazzard;

static void rearm(state_p pState);
static void disarm(state_p pState);
// returns true when timer fires
static bool tick(state_p pState);


extern "C" void EXTI2_3_IRQHandler(void) {
	if(EXTI_GetITStatus(RIGHT_INTERRUPT) != RESET) {
		if (!right.isActive) {
//			Turn off left
			disarm(&left);
			setLeftOutputState(DISABLE);
//			Turn On right
			rearm(&right);
			setRightOutputState(ENABLE);
		}

	    EXTI_ClearITPendingBit(RIGHT_INTERRUPT);
	}
}

extern "C" void EXTI4_15_IRQHandler(void) {

	if(EXTI_GetITStatus(LEFT_INTERRUPT) != RESET) {
		if (!left.isActive) {
//			Turn off right
			disarm(&right);
			setRightOutputState(DISABLE);
//			Turn On left
			rearm(&left);
			setLeftOutputState(ENABLE);
		}
	    EXTI_ClearITPendingBit(LEFT_INTERRUPT);
	}

	if(EXTI_GetITStatus(HAZARD_INTERRUPT) != RESET) {

		if (!hazzard.isActive) {
			rearm(&hazzard);
			setHazzardOutputState(ENABLE);
		}
	    EXTI_ClearITPendingBit(HAZARD_INTERRUPT);
	}
}

extern "C" void SysTick_Handler(void) {
	if (tick(&left)) {
		disarm(&left);
		setLeftOutputState(DISABLE);
	}

	if (tick(&right)) {
		disarm(&right);
		setRightOutputState(DISABLE);
	}

	if (tick(&hazzard)) {
		disarm(&hazzard);
		setHazzardOutputState(DISABLE);
	}
}


static void rearm(state_p pState) {
	if (!pState->isActive) {
		pState->debounce = DEBOUNCE_COUNT;
		pState->isActive = true;
	}
}

static void disarm(state_p pState) {
	pState->debounce = 0;
	pState->isActive = false;
}

static bool tick(state_p pState) {

	if (pState->isActive && pState->debounce) {
		pState->debounce--;
	}
	return pState->isActive && !pState->debounce;
}
