/*
 * config.c
 *
 *  Created on: Jan 7, 2016
 *      Author: shapa
 */

#include "config.h"
#include "stm32f0xx_crc.h"
#include "stm32f0xx_flash.h"


#define DEFAULT_TIMEOUT (1.8*TICKS_PER_SECOND)

#define UPPER_TIME_BOUND (9*TICKS_PER_SECOND)
#define LOWER_TIME_BOUND (0.6*TICKS_PER_SECOND)

static const config_t s_defaultConfig = {
		DEFAULT_TIMEOUT,
		DEFAULT_TIMEOUT,
		DEFAULT_TIMEOUT,
		0
};
extern config_t s_userConfig; /* provided by linker. see sections.ld */

static bool s_isActive = false;
static bool s_isTimeCounting = false;
static uint32_t s_timeElapsed = 0;

static uint32_t calculateCRC(config_p);
static bool isConfigValid(config_p);
static bool isTimeValid(void);
static bool saveConfig(bool isHazard);
static bool flashConfig(config_p newConfig);

void configureStart(void) {
	if (!s_isActive) {
		reconfigureInterruptsTrigger(true);
		s_isActive = true;
		s_timeElapsed = 0;
		s_isTimeCounting = false;
	}
}

void configureTrigger(void) {
	if (s_isActive) {
		s_isTimeCounting = true;
	}
}

void configureTick(void) {
	if (s_isActive && s_isTimeCounting) {
		if (s_timeElapsed++ > UPPER_TIME_BOUND) {
			/* overflow, exit configure mode */
			reconfigureInterruptsTrigger(false);
			s_isActive = false;
			s_isTimeCounting = false;
			s_timeElapsed = 0;
		}
	}
}

void configureStop(bool isHazard) {

	if (s_isActive) {
		if (isTimeValid()) {
			saveConfig(isHazard);
		}
		reconfigureInterruptsTrigger(false);
		s_timeElapsed = 0;
		s_isActive = false;
		s_isTimeCounting = false;
	}
}

bool isConfigureActive(void) {
	return s_isActive;
}

config_p getConfig(void) {

	config_p config = &s_userConfig;
	if (!isConfigValid(config))
		config = &s_defaultConfig;
	return config;

}

static uint32_t calculateCRC(config_p newConfig) {

	assert_param(newConfig);
	uint32_t crc = 0;
	uint32_t size = ((uint32_t)&newConfig->crc - (uint32_t)newConfig)/sizeof(uint32_t);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	CRC_DeInit();
	CRC_SetInitRegister(0xffffffff);
	CRC_ReverseInputDataSelect(CRC_ReverseInputData_No);
	CRC_ReverseOutputDataCmd(DISABLE);
	CRC_ResetDR();
	crc = CRC_CalcBlockCRC((uint32_t*)newConfig, size);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, DISABLE);
	return crc;
}

static bool isConfigValid(config_p newConfig) {
	bool retval = false;
	if (newConfig && newConfig->crc)
		retval = (newConfig->crc == calculateCRC(newConfig));
	return retval;
}

static bool isTimeValid(void) {
	return (s_timeElapsed > LOWER_TIME_BOUND) && (s_timeElapsed < UPPER_TIME_BOUND);
}

static bool saveConfig(bool isHazard) {

	bool result = false;
	config_t newConfig = *getConfig();

	__disable_irq();

	if (isHazard) {
		newConfig.hazardTout = s_timeElapsed;
	} else {
		newConfig.leftTout = s_timeElapsed;
		newConfig.rightTout = s_timeElapsed;
	}
	newConfig.crc = calculateCRC(&newConfig);
	result = flashConfig(&newConfig);

	__enable_irq();
	return result;
}

static bool flashConfig(config_p newConfig) {

	uint32_t configSize = sizeof(config_t)/sizeof(uint32_t);
	uint32_t *rawData = (uint32_t*)newConfig;
	uint32_t i = 0;
	uint32_t sartAdress = (uint32_t)&s_userConfig;

	assert_param(newConfig);
	if (isConfigValid(newConfig)) {
		FLASH_SetLatency(FLASH_Latency_0);
		FLASH_PrefetchBufferCmd(ENABLE);
		FLASH_Unlock();
		assert_param(FLASH_ErasePage(sartAdress) == FLASH_COMPLETE);
		for (i = 0; i < configSize; i++) {
			assert_param(FLASH_ProgramWord(sartAdress + i*sizeof(uint32_t), rawData[i]) == FLASH_COMPLETE);
		}
		FLASH_Lock();
		FLASH_PrefetchBufferCmd(DISABLE);
	}
}
