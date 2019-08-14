#ifndef __CLKCONFIG_H
#define	__CLKCONFIG_H

#include "stm32h7xx.h"


void SystemClock_Config(void);
void SystemClockHSI_Config(void);
void SystemClockHSE_Config(void);
void SystemClockCSI_Config(void);
void SwitchSystemClock(void);

#endif /* __CLKCONFIG_H */
