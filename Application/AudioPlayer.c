/*
 * Audio.c
 *
 *  Created on: Feb 6, 2020
 *      Author: Ericson Joseph
 */

#include "AudioPlayer.h"
#include "stm32h747i_discovery_audio.h"
#include "LcdLog.h"
#include "cmsis_os.h"

#define AUDIO_SIZE	160
#define ATTENUATOR 1
#define AUDIO_BUFFER_SIZE  40*2


BSP_AUDIO_Init_t AudioPlayInit;


typedef struct {
  uint8_t buff[AUDIO_BUFFER_SIZE];
} AUDIO_BufferTypeDef;


ALIGN_32BYTES (static AUDIO_BufferTypeDef  buffer_ctl);

static int16_t ToneArray_8000[] = {
		(int16_t)(0.0 * ATTENUATOR),
		(int16_t)(0.0 * ATTENUATOR),
		(int16_t)(7725.0 * ATTENUATOR),
		(int16_t)(7725.0 * ATTENUATOR),
		(int16_t)(14694.0 * ATTENUATOR),
		(int16_t)(14694.0 * ATTENUATOR),
		(int16_t)(20225.0 * ATTENUATOR),
		(int16_t)(20225.0 * ATTENUATOR),
		(int16_t)(23776.0 * ATTENUATOR),
		(int16_t)(23776.0 * ATTENUATOR),
		(int16_t)(25000.0 * ATTENUATOR),
		(int16_t)(25000.0 * ATTENUATOR),
		(int16_t)(23776.0 * ATTENUATOR),
		(int16_t)(23776.0 * ATTENUATOR),
		(int16_t)(20225.0 * ATTENUATOR),
		(int16_t)(20225.0 * ATTENUATOR),
		(int16_t)(14694.0 * ATTENUATOR),
		(int16_t)(14694.0 * ATTENUATOR),
		(int16_t)(7725.0 * ATTENUATOR),
		(int16_t)(7725.0 * ATTENUATOR),
		(int16_t)(0.0 * ATTENUATOR),
		(int16_t)(0.0 * ATTENUATOR),
		(int16_t)(-7725.0 * ATTENUATOR),
		(int16_t)(-7725.0 * ATTENUATOR),
		(int16_t)(-14694.0 * ATTENUATOR),
		(int16_t)(-14694.0 * ATTENUATOR),
		(int16_t)(-20225.0 * ATTENUATOR),
		(int16_t)(-20225.0 * ATTENUATOR),
		(int16_t)(-23776.0 * ATTENUATOR),
		(int16_t)(-23776.0 * ATTENUATOR),
		(int16_t)(-25000.0 * ATTENUATOR),
		(int16_t)(-25000.0 * ATTENUATOR),
		(int16_t)(-23776.0 * ATTENUATOR),
		(int16_t)(-23776.0 * ATTENUATOR),
		(int16_t)(-20225.0 * ATTENUATOR),
		(int16_t)(-20225.0 * ATTENUATOR),
		(int16_t)(-14694.0 * ATTENUATOR),
		(int16_t)(-14694.0 * ATTENUATOR),
		(int16_t)(-7725.0 * ATTENUATOR),
		(int16_t)(-7725.0 * ATTENUATOR)
};

void AudioPlayer_Init() {


	memcpy(buffer_ctl.buff, ToneArray_8000, AUDIO_BUFFER_SIZE);

	AudioPlayInit.Device = AUDIO_OUT_DEVICE_HEADPHONE;
	AudioPlayInit.ChannelsNbr = 1;
	AudioPlayInit.SampleRate = AUDIO_FREQUENCY_8K;
	AudioPlayInit.BitsPerSample = AUDIO_RESOLUTION_16B;
	AudioPlayInit.Volume = 90;

	if (BSP_AUDIO_OUT_Init(0, &AudioPlayInit) == 0) {
		LCDLog_RLog(2, "Audio Initialize OK");
		BSP_AUDIO_OUT_Play(0,(uint8_t *)&buffer_ctl.buff[0], AUDIO_BUFFER_SIZE);
	}

}

void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance){

}


