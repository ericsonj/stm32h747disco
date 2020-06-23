/*
 * SDFatFs.c
 *
 *  Created on: Feb 18, 2020
 *      Author: Ericson Joseph
 */
#include "SDFatFs.h"
#include "sd_diskio_dma.h"
#include "stm32h747i_discovery_sd.h"
#include "stm32h747i_discovery.h"
#include "LcdLog.h"
#include "cmsis_os.h"
#include "AudioDrive.h"

#define AUDIO_SIZE     960*2 // BYTES
#define ATTENUATOR 1
#define AUDIO_BUFFER_SIZE  AUDIO_SIZE*2

#define AUDIOSFILES_NUM 2

typedef struct {
	uint8_t buff[AUDIO_BUFFER_SIZE];
} AUDIO_BufferTypeDef;

FS_FileOperationsTypeDef Appli_state = APPLICATION_IDLE;
static uint8_t isInitialized = 0;
static char SDPath[4];
static FATFS SDFatFs; /* File system object for SD card logical drive */
static FIL MyFile; /* File object */
ALIGN_32BYTES(uint8_t rtext[96]);
//static uint8_t workBuffer[_MAX_SS];
ALIGN_32BYTES(int16_t audioFrame[AUDIO_SIZE]);
ALIGN_32BYTES(int16_t audioFrameHalf[AUDIO_SIZE/2]);
//static uint8_t wtext[] =
//		"[STM32H747_Disco/CORE_CM7]:This is STM32 working with FatFs + DMA\n"; /* File write buffer */

BSP_AUDIO_Init_t AudioPlayInit;

//ALIGN_32BYTES(static AUDIO_BufferTypeDef buffer_ctl);

TickType_t frameTime = 0;
uint8_t audioFlag = 0;
QueueHandle_t audioQueue = NULL;

AudioHandle_t audiohandle;

AudioRawFile audioFiles[AUDIOSFILES_NUM] = {
		{
				.name = "QU_48KHZ2CH.RAW",
				.bitsPerSample = AUDIO_RESOLUTION_16B,
				.sampleRate = AUDIO_FREQUENCY_48K
		},
		{
				.name = "GX_48KHZ2CH.RAW",
				.bitsPerSample = AUDIO_RESOLUTION_16B,
				.sampleRate = AUDIO_FREQUENCY_48K
		}
};

int8_t audioFileIdx = 0;
uint32_t volume = 70;

static void SD_Initialize(void);
static void FS_FileOperations(void);
static void SDFatFs_task(const void *arg);

static void audioCallBack(uint8_t *buff, uint32_t size);

void SDFatFs_Init() {

	BSP_JOY_Init(JOY1, JOY_MODE_GPIO, JOY_ALL);

	audioQueue = xQueueCreate(4, sizeof(uint8_t));

	osThreadDef(SDFatFs_Thread, SDFatFs_task, osPriorityHigh, 0,
			configMINIMAL_STACK_SIZE * 5);
	osThreadCreate(osThread(SDFatFs_Thread), NULL);

}

void SDFatFs_task(const void *arg) {

	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {

		SD_Initialize();

		if (BSP_SD_IsDetected(0)) {
			FRESULT res = FR_OK;
//			res = f_mkfs(SDPath, FM_FAT32, 0, workBuffer, sizeof(workBuffer));
			if (res == FR_OK) {
				Appli_state = APPLICATION_RUNNING;
			}
		}

		if (Appli_state == APPLICATION_RUNNING) {
			FS_FileOperations();
		}

	}

	for (;;) {
		osDelay(40);
		int32_t joy = BSP_JOY_GetState(JOY1, JOY_UP);
		switch (joy) {
		case JOY_UP:
			BSP_AUDIO_OUT_GetVolume(0, &volume);
			volume = (volume > AUDIODRIVE_VOL_MAX - 1) ? AUDIODRIVE_VOL_MAX : volume + 1;
			BSP_AUDIO_OUT_SetVolume(0, volume);
			break;
		case JOY_DOWN:
			BSP_AUDIO_OUT_GetVolume(0, &volume);
			volume = (volume < 1) ? AUDIODRIVE_VOL_MIN : volume - 1;
			BSP_AUDIO_OUT_SetVolume(0, volume);
			break;
		case JOY_RIGHT:
		case JOY_LEFT:
			if(joy == JOY_RIGHT){
				audioFileIdx++;
			} else {
				audioFileIdx--;
			}

			if (audioFileIdx >= AUDIOSFILES_NUM ){
				audioFileIdx = 0;
			}else if (audioFileIdx < 0){
				audioFileIdx = AUDIOSFILES_NUM - 1;
			}
			if (AudioDrive_isBusy()){
				AudioDrive_stop();
			}
			osDelay(100);
			f_close(&MyFile);
			osDelay(200);
			if (f_open(&MyFile, audioFiles[audioFileIdx].name, FA_READ) == FR_OK) {
				audiohandle.device = AUDIO_OUT_DEVICE_HEADPHONE;
				audiohandle.channelsNbr = 1;
				audiohandle.BitsPerSample = audioFiles[audioFileIdx].bitsPerSample;
				audiohandle.sampleRate = audioFiles[audioFileIdx].sampleRate;
				audiohandle.volumen = volume;
				audiohandle.frameSize = AUDIO_SIZE;
				audiohandle.fillBuffCallback = audioCallBack;
				LCDLog_RLog(4, "Play %s ...", audioFiles[audioFileIdx].name);
				AudioDrive_play(audiohandle);
			}
		default:
			break;
		}

	}

}

static void SD_Initialize(void) {
	if (isInitialized == 0) {
		BSP_SD_Init(0);
		BSP_SD_DetectITConfig(0);

		if (BSP_SD_IsDetected(0)) {
			isInitialized = 1;
		}
	}
}

#if 0
static uint8_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2,
		uint32_t BufferLength) {
	while (BufferLength--) {
		if (*pBuffer1 != *pBuffer2) {
			return 1;
		}
		pBuffer1++;
		pBuffer2++;
	}
	return 0;
}
#endif

static void FS_FileOperations(void) {
	FRESULT res; /* FatFs function common result code */
	uint32_t bytesread; /* File write/read counts */

	/* Register the file system object to the FatFs module */
	if (f_mount(&SDFatFs, (TCHAR const*) SDPath, 0) == FR_OK) {
#if 0
		/* Create and Open a new text file object with write access */
		if (f_open(&MyFile, "STM32.TXT", FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
			/* Write data to the text file */
			res = f_write(&MyFile, wtext, (sizeof(wtext) - 1),
					(void*) &byteswritten);

			if ((byteswritten > 0) && (res == FR_OK)) {
				/* Close the open text file */
				f_close(&MyFile);

				/* Open the text file object with read access */
				if (f_open(&MyFile, "STM32.TXT", FA_READ) == FR_OK) {
					/* Read data from the text file */
					res = f_read(&MyFile, rtext, sizeof(rtext),
							(void*) &bytesread);

					if ((bytesread > 0) && (res == FR_OK)) {
						/* Close the open text file */
						f_close(&MyFile);

						/* Compare read data with the expected data */
						if (Buffercmp(rtext, wtext, byteswritten) == 0) {
							/* Success of the demo: no error occurrence */
//							BSP_LED_On(LED_GREEN);
							LCDLog_RLog(3, "Test SD OK");
						}
					}
				}
			}
		}
#endif

		AudioDrive_init();

		if (f_open(&MyFile, "QU_48KHZ2CH.RAW", FA_READ) == FR_OK) {
			res = f_read(&MyFile, audioFrame, sizeof(audioFrame),
					(void*) &bytesread);
			if (res == FR_OK) {

				audiohandle.device = AUDIO_OUT_DEVICE_HEADPHONE;
				audiohandle.channelsNbr = 1;
				audiohandle.BitsPerSample = AUDIO_RESOLUTION_16B;
				audiohandle.sampleRate = AUDIO_FREQUENCY_48K;
				audiohandle.volumen = volume;
				audiohandle.frameSize = AUDIO_SIZE;
				audiohandle.fillBuffCallback = audioCallBack;

				AudioDrive_play(audiohandle);

#if 0
				memcpy(buffer_ctl.buff, audioFrame, sizeof(audioFrame));
				BSP_AUDIO_OUT_Stop(0);
				AudioPlayInit.Device = AUDIO_OUT_DEVICE_HEADPHONE;
				AudioPlayInit.ChannelsNbr = 1;
				AudioPlayInit.SampleRate = AUDIO_FREQUENCY_48K;
				AudioPlayInit.BitsPerSample = AUDIO_RESOLUTION_16B;
				AudioPlayInit.Volume = 69;

				if (BSP_AUDIO_OUT_Init(0, &AudioPlayInit) == 0) {
					LCDLog_RLog(2,
							"Play Bohemian Rhapsody PCM 16bits 96Khz ...");
					BSP_AUDIO_OUT_Play(0, (uint8_t*) &buffer_ctl.buff[0],
					AUDIO_BUFFER_SIZE);
				}
#endif

			}

#if 0

			volatile uint32_t ticks = osKernelSysTick();

			for (;;) {
				xQueueReceive(audioQueue, &audioFlag, portMAX_DELAY);
				if (audioFlag == 1) {
					volatile uint32_t ctick = osKernelSysTick();
					ticks = ctick - ticks;
					ticks = ctick;
					audioFlag = 0;
					res = f_read(&MyFile, audioFrameHalf,
							sizeof(audioFrameHalf), (void*) &bytesread);
					if (bytesread == sizeof(audioFrameHalf) && res == FR_OK) {
						memcpy(&buffer_ctl.buff[0], audioFrameHalf,
								sizeof(audioFrameHalf));
					} else {
						break;
					}
				} else if (audioFlag == 2) {
					audioFlag = 0;
					res = f_read(&MyFile, audioFrameHalf,
							sizeof(audioFrameHalf), (void*) &bytesread);
					if (bytesread == sizeof(audioFrameHalf) && res == FR_OK) {
						memcpy(&buffer_ctl.buff[AUDIO_BUFFER_SIZE / 2],
								audioFrameHalf, sizeof(audioFrameHalf));
					} else {
						break;
					}
				}
			}
#endif

		}
	}
}

void audioCallBack(uint8_t *buff, uint32_t size) {
	FRESULT res;
	uint32_t bytesread;
	res = f_read(&MyFile, audioFrame, size, (void*) &bytesread);
	if (res == FR_OK && bytesread == size) {
		memcpy(buff, audioFrame, size);
	} else {
		memset(buff, 0, size);
	}
}

#if 0
void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance) {
	uint8_t auxaudioFlag = 2;
	BaseType_t xTaskWokenByReceive = pdFALSE;
	xQueueSendFromISR(audioQueue, &auxaudioFlag, &xTaskWokenByReceive);
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance) {
	uint8_t auxaudioFlag = 1;
	BaseType_t xTaskWokenByReceive = pdFALSE;
	xQueueSendFromISR(audioQueue, &auxaudioFlag, &xTaskWokenByReceive);
}
#endif
