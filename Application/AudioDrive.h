/*
 * AudioDrive.h
 *
 *  Created on: Feb 29, 2020
 *      Author: Ericson Joseph
 */

#ifndef APPLICATION_AUDIODRIVE_H_
#define APPLICATION_AUDIODRIVE_H_

#include <stdint.h>
#include <stdbool.h>


#define AUDIODRIVE_VOL_MAX	100
#define	AUDIODRIVE_VOL_MIN	0


typedef struct _AudioHandle AudioHandle_t;

struct _AudioHandle {
	uint32_t device;
	uint32_t channelsNbr;
	uint32_t sampleRate;
	uint32_t volumen;
	uint32_t BitsPerSample;
	uint32_t frameSize;
	void (*fillBuffCallback)(uint8_t *buff, uint32_t size);
};

typedef struct _AudioCallBack AudioCallBack_t;

struct _AudioCallBack {
	uint32_t frameSize;
	void (*fillBuffCallback)(uint8_t *buff, uint32_t size);
};

enum {
	aERROR = -1,
	aOK,
	aBUSY
};

int8_t AudioDrive_init(void);

bool AudioDrive_isBusy(void);

int8_t AudioDrive_play(AudioHandle_t handle);

void AudioDrive_stop(void);

#endif /* APPLICATION_AUDIODRIVE_H_ */
