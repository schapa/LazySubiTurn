/*
 * config.h
 *
 *  Created on: Jan 7, 2016
 *      Author: shapa
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "api.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint32_t leftTout;
	uint32_t rightTout;
	uint32_t hazardTout;
	uint32_t crc;
} config_t, *config_p;


void configureTrigger(void);
void configureStart(void);
void configureTick(void);
void configureStop(bool isHazard);
bool isConfigureActive(void);

config_p getConfig(void);


#endif /* CONFIG_H_ */
