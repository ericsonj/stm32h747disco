/*
 * AudioDrive.c
 *
 *  Created on: Feb 29, 2020
 *      Author: Ericson Joseph
 */

#include "AudioDrive.h"
#include "stm32h747i_discovery_audio.h"
#include "cmsis_os.h"
#include <string.h>

#define AUDIOBUFFER_SIZE 4096

ALIGN_32BYTES(uint8_t audiobuffer[AUDIOBUFFER_SIZE]);
//static TaskHandle_t audioTaskHandle;
static SemaphoreHandle_t audioDriveUsed;
static QueueHandle_t audioQueue = NULL;

static BSP_AUDIO_Init_t AudioPlayInit;
static AudioCallBack_t callback;

static bool isAudioDriveBusy;

static void AudioDrive_task(const void *arg);


int8_t AudioDrive_init(void) {

	audioQueue = xQueueCreate(2, sizeof(uint8_t*));

	audioDriveUsed = xSemaphoreCreateMutex();
	if (audioDriveUsed == NULL) {
		return aERROR;
	}
	xSemaphoreGive(audioDriveUsed);

	osThreadDef(AudioDrive_Thread, AudioDrive_task, osPriorityHigh, 0,
			configMINIMAL_STACK_SIZE * 5);
	osThreadCreate(osThread(AudioDrive_Thread), NULL);

	isAudioDriveBusy = false;

	return aOK;
}


void AudioDrive_task(const void *arg) {
	uint8_t *buffertoPlay;
	for (;;) {
		xQueueReceive(audioQueue, &buffertoPlay, portMAX_DELAY);
		if (callback.fillBuffCallback != NULL) {
			callback.fillBuffCallback(buffertoPlay, callback.frameSize);
		}
	}
}

bool AudioDrive_isBusy(void) {
	return isAudioDriveBusy;
}


int8_t AudioDrive_play(AudioHandle_t handle) {
	if (xQueueSemaphoreTake(audioDriveUsed, 0) == pdFALSE) {
		return aBUSY;
	}

	if ((handle.frameSize * 2) > AUDIOBUFFER_SIZE) {
		return aERROR;
	}

//	BSP_AUDIO_OUT_DeInit(0);

	AudioPlayInit.Device = handle.device;
	AudioPlayInit.ChannelsNbr = handle.channelsNbr;
	AudioPlayInit.SampleRate = handle.sampleRate;
	AudioPlayInit.BitsPerSample = handle.BitsPerSample;
	AudioPlayInit.Volume = handle.volumen;
	callback.fillBuffCallback = handle.fillBuffCallback;
	callback.frameSize = handle.frameSize;

	memset(audiobuffer,0, AUDIOBUFFER_SIZE);

	if (BSP_AUDIO_OUT_Init(0, &AudioPlayInit) == 0) {
		BSP_AUDIO_OUT_Play(0, (uint8_t*) &audiobuffer[0], handle.frameSize * 2);
	}

	isAudioDriveBusy = true;

	return aOK;

}


void AudioDrive_stop(void) {
	BSP_AUDIO_OUT_Stop(0);
	xSemaphoreGive(audioDriveUsed);
	isAudioDriveBusy = false;
}


void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance) {
	uint8_t *prtBuff = &audiobuffer[callback.frameSize];
	BaseType_t xTaskWokenByReceive = pdFALSE;
	xQueueSendFromISR(audioQueue, &prtBuff, &xTaskWokenByReceive);
}


void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance) {
	uint8_t *prtBuff = &audiobuffer[0];
	BaseType_t xTaskWokenByReceive = pdFALSE;
	xQueueSendFromISR(audioQueue, &prtBuff, &xTaskWokenByReceive);
}
