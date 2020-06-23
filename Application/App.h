/*
 * App.h
 *
 *  Created on: Jan 16, 2020
 *      Author: ericson
 */

#ifndef APPLICATION_APP_H_
#define APPLICATION_APP_H_


#include "stm32h7xx_hal.h"
#include "stm32h747i_discovery.h"


void APP_init();

/*<test>*/
#include <stdint.h>
int32_t APP_sum(int32_t a, int32_t b);

int32_t APP_multi(int32_t a, int32_t b);
/*</test>*/

#endif /* APPLICATION_APP_H_ */
