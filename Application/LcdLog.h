/*
 * LcdLog.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Ericson Joseph
 */

#ifndef APPLICATION_LCDLOG_H_
#define APPLICATION_LCDLOG_H_

#include "basic_gui.h"
#include <stdio.h>
#include <string.h>

#define CLEAN_LINE     "                                                            "

char LCDLog_buffer[50];

#define LCDLog_Log(...)		do{\
		 	 	 	 	 	 	 memset(LCDLog_buffer, 0, 50);\
		 	 	 	 	 	 	 snprintf(LCDLog_buffer,50, __VA_ARGS__);\
		 	 	 	 	 	 	 GUI_DisplayStringAt(0, 50,(uint8_t*)LCDLog_buffer, LEFT_MODE);\
							}while(0)

#define LCDLog_RLog(R, ...)		do{\
		 	 	 	 	 	 	 memset(LCDLog_buffer,0, 50);\
		 	 	 	 	 	 	 snprintf(LCDLog_buffer,50, __VA_ARGS__);\
		 	 	 	 	 	 	 GUI_DisplayStringAt(0, 50 + ((1 + R)*25),(uint8_t*)CLEAN_LINE, LEFT_MODE);\
		 	 	 	 	 	 	 GUI_DisplayStringAt(0, 50 + ((1 + R)*25),(uint8_t*)LCDLog_buffer, LEFT_MODE);\
							}while(0)



#endif /* APPLICATION_LCDLOG_H_ */
