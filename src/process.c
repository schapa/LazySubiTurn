/*
 * process.cpp
 *
 *  Created on: Dec 10, 2015
 *      Author: shapa
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "api.h"
#include "config.h"

#define DEBOUNCE_COUNT (0.1*TICKS_PER_SECOND)

typedef void (*setOutputState_t)(FunctionalState state);
typedef FunctionalState (*getInputState_t)(void);

typedef struct {
	uint32_t debounce;
	uint32_t timeout;
	bool isActive;
	setOutputState_t setOutputState;
	getInputState_t getInputState;
} tapper_t, *tapper_p;

static tapper_t s_leftTapper = { 0, 0, 0, setLeftOutputState, getLeftInputState };
static tapper_t s_rightTapper = { 0, 0, 0, setRightOutputState, getRightInputState };
static tapper_t s_hazardTapper = { 0, 0, 0, setHazardOutputState, getHazardInputState };

static tapper_p s_tappers[] = {
		&s_leftTapper, &s_rightTapper, &s_hazardTapper
};
static size_t s_tappersCount = sizeof(s_tappers)/sizeof(s_tappers[0]);

static uint32_t getTimeout(tapper_p pTapper);
static void rearm(tapper_p pTapper);
static void disarm(tapper_p pTapper);
static void tick(tapper_p pTapper);


void EXTI2_3_IRQHandler(void) {
	if(EXTI_GetITStatus(RIGHT_INTERRUPT)) {
		if (isConfigureActive()) {
			if (s_rightTapper.getInputState() == ENABLE) {
				/* right interrupt on rising edge */
				configureStop(false);
			} else {
				configureTrigger();
			}
		} else {
			s_rightTapper.debounce = DEBOUNCE_COUNT;
			s_leftTapper.setOutputState(DISABLE);
		}
		EXTI_ClearITPendingBit(RIGHT_INTERRUPT);
	}
}

void EXTI4_15_IRQHandler(void) {

	if(EXTI_GetITStatus(LEFT_INTERRUPT)) {
		if (isConfigureActive()) {
			if (s_leftTapper.getInputState() == ENABLE) {
				/* left interrupt on rising edge */
				configureStop(false);
			} else {
				configureTrigger();
			}
		} else {
			s_leftTapper.debounce = DEBOUNCE_COUNT;
			s_rightTapper.setOutputState(DISABLE);
		}
		EXTI_ClearITPendingBit(LEFT_INTERRUPT);
	}

	if(EXTI_GetITStatus(HAZARD_INTERRUPT)) {
		if (isConfigureActive()) {
			if (s_hazardTapper.getInputState() == ENABLE) {
				/* hazard interrupt on rising edge */
				configureStop(true);
			} else {
				configureTrigger();
			}
		} else {
			s_hazardTapper.debounce = DEBOUNCE_COUNT;
		}
		EXTI_ClearITPendingBit(HAZARD_INTERRUPT);
	}

	if(EXTI_GetITStatus(CONFIG_INTERRUPT)) {
		configureStart();
		EXTI_ClearITPendingBit(CONFIG_INTERRUPT);
	}
}

void SysTick_Handler(void) {
	size_t i;
	for (i = 0; i < s_tappersCount; i++) {
		tick(s_tappers[i]);
	}
	configureTick();
}


static uint32_t getTimeout(tapper_p pTapper) {

	uint32_t timeout = DEBOUNCE_COUNT;
	assert_param(pTapper);

	if (pTapper == &s_leftTapper) {
		timeout = getConfig()->leftTout;
	} else if (pTapper == &s_rightTapper) {
		timeout = getConfig()->rightTout;
	} else if (pTapper == &s_hazardTapper) {
		timeout = getConfig()->hazardTout;
	}
	return timeout;
}

static void rearm(tapper_p pTapper) {

	assert_param(pTapper);
	if (!pTapper->isActive) {
		pTapper->timeout = getTimeout(pTapper);
		pTapper->debounce = 0;
		pTapper->isActive = true;
	}
}

static void disarm(tapper_p pTapper) {

	assert_param(pTapper);
	pTapper->timeout = 0;
	pTapper->debounce = 0;
	pTapper->isActive = false;
}

static void tick(tapper_p pTapper) {

	assert_param(pTapper);
	if (pTapper->isActive) {
		if (pTapper->timeout) {
			pTapper->timeout--;
		} else {
			disarm(pTapper);
			pTapper->setOutputState(DISABLE);
		}
	} else if (pTapper->debounce) {
		pTapper->debounce--;
		if (!pTapper->debounce && (pTapper->getInputState() != ENABLE)) {
			rearm(pTapper);
			pTapper->setOutputState(ENABLE);
		}
	}
}
