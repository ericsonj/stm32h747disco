/*
 * QSPI_Flash.c
 *
 *  Created on: Jun 25, 2020
 *      Author: Ericson
 */
#include <string.h>
#include "stm32h747i_discovery_qspi.h"
#include "cmsis_os.h"
#include "yaffsfs.h"
#include "yaffs_trace.h"

#define BSP_QSPI		0
#define YAFFS2_TEST		1

#define pTRY \
    do

#define pCATCH(error) 	\
    while (0);	 		\
    if(error)			\


#define pERROR(error, _value_)	\
	error = _value_;			\
	break;						\


#define QSPI_FLASH_BLOCK_LEN (1024 * 4)
#define WRITE_READ_ADDR ((uint32_t)0x0)

static void QSPIFlash_task(const void *args);
static osThreadId task;
#if BSP_QSPI
static BSP_QSPI_Init_t bqspi;
static uint8_t txblock[QSPI_FLASH_BLOCK_LEN];
static uint8_t rxblock[QSPI_FLASH_BLOCK_LEN];
static BSP_QSPI_Info_t pQSPI_Info;
#endif

#if YAFFS2_TEST
static uint8_t data[16];
#endif

void QSPIFlash_init(void) {
	osThreadDef(QSPIFlash_thread, QSPIFlash_task, osPriorityNormal, 0,
			configMINIMAL_STACK_SIZE * 8);
	task = osThreadCreate(osThread(QSPIFlash_thread), NULL);
}

static void QSPIFlash_task(const void *args) {

	int error = 0;

#if YAFFS2_TEST
	yaffs_trace_mask = YAFFS_TRACE_ERROR;
	yaffs_start_up();
	int f = 0;

	pTRY {

		if (yaffs_mount("nand") != 0) {
			pERROR(error, 1);
		}

		f = yaffs_open("nand/file0.txt", O_CREAT | O_EXCL | O_RDWR, (S_IREAD | S_IWRITE));
		if (f != -1) {
			/*If File not exist*/
			yaffs_write(f, "HOLA\n", 6);
			yaffs_flush(f);
			yaffs_close(f);
		} else {
			/* If file exist */
			f =  yaffs_open("nand/file0.txt",  O_APPEND | O_WRONLY, (S_IREAD | S_IWRITE));
			if (f != -1) {
				yaffs_write(f, "HOLA\n", 6);
				yaffs_flush(f);
				yaffs_close(f);
			}
		}

		f = yaffs_open("nand/file0.txt",  O_RDONLY, (S_IREAD | S_IWRITE));
		if (f != -1) {
			yaffs_read(f, data, 16);
			yaffs_close(f);
		}

	}pCATCH(error) {}
#endif

#if BSP_QSPI

	bqspi.TransferRate = MT25TL01G_DTR_TRANSFER;
	bqspi.DualFlashMode = MT25TL01G_DUALFLASH_ENABLE;
	bqspi.InterfaceMode = MT25TL01G_QPI_MODE;

	pTRY {

		if (BSP_QSPI_Init(0, &bqspi) != BSP_ERROR_NONE) {
			pERROR(error, 1);
		}

		BSP_QSPI_GetInfo(0, &pQSPI_Info);

		if ((pQSPI_Info.FlashSize != 0x8000000)
				|| (pQSPI_Info.EraseSectorSize != 0x2000)
				|| (pQSPI_Info.ProgPageSize != 0x100)
				|| (pQSPI_Info.EraseSectorsNumber != 0x4000)
				|| (pQSPI_Info.ProgPagesNumber != 0x80000)) {
			pERROR(error, 1);
		}
		uint16_t i = 0;
		for (i = 0; i < 2048; ++i) {
			if (BSP_QSPI_EraseBlock(0, WRITE_READ_ADDR + ((0x10000)*(i)),
					MT25TL01G_ERASE_64K) != BSP_ERROR_NONE) {
				pERROR(error, 1);
			}
		}

//		memset(txblock, 0xAA, QSPI_FLASH_BLOCK_LEN);
//
//		if (BSP_QSPI_Write(0, txblock, WRITE_READ_ADDR,
//				16) != BSP_ERROR_NONE) {
//			pERROR(error, 1);
//		}
//
//		if (BSP_QSPI_Read(0, rxblock, WRITE_READ_ADDR,
//				16) != BSP_ERROR_NONE) {
//			pERROR(error, 1);
//		}
//
//		memset(txblock, 0x80, QSPI_FLASH_BLOCK_LEN);
//
//		if (BSP_QSPI_Write(0, txblock, WRITE_READ_ADDR + 16,
//				16) != BSP_ERROR_NONE) {
//			pERROR(error, 1);
//		}
//
//		if (BSP_QSPI_Read(0, rxblock, WRITE_READ_ADDR + 16,
//				16) != BSP_ERROR_NONE) {
//			pERROR(error, 1);
//		}
//
//		memset(rxblock, 0x00, QSPI_FLASH_BLOCK_LEN);
//
//		if (BSP_QSPI_Read(0, rxblock, WRITE_READ_ADDR, 32)) {
//			pERROR(error, 1);
//		}

	}pCATCH(error) {
	}

#endif

	osThreadTerminate(task);
}
