/*
 * SDFatFs.h
 *
 *  Created on: Feb 18, 2020
 *      Author: Ericson Joseph
 */

#ifndef APPLICATION_SDFATFS_H_
#define APPLICATION_SDFATFS_H_

#include "stm32h747i_discovery_audio.h"

typedef enum {
	APPLICATION_IDLE = 0,
	APPLICATION_RUNNING,
	APPLICATION_SD_UNPLUGGED,
	APPLICATION_STATUS_CHANGED,
} FS_FileOperationsTypeDef;

typedef struct _AudioRawFile AudioRawFile;

struct _AudioRawFile {
	char name[50];
	uint32_t bitsPerSample;
	uint32_t sampleRate;
};


void SDFatFs_Init();


#endif /* APPLICATION_SDFATFS_H_ */
