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

#define TRY_FUNC \
    do

#define END_FUNC \
    while (0)    \
        ;

#define EXIT_FUNC break

#define QSPI_FLASH_BLOCK_LEN (1024 * 4)
#define WRITE_READ_ADDR ((uint32_t)0x0)

static void       QSPIFlash_task(const void* args);
static osThreadId task;
#if 0
static BSP_QSPI_Init_t bqspi;
static uint8_t         txblock[QSPI_FLASH_BLOCK_LEN];
static uint8_t         rxblock[QSPI_FLASH_BLOCK_LEN];
static BSP_QSPI_Info_t pQSPI_Info;
#endif

static uint8_t data[16];

void QSPIFlash_init(void) {

    osThreadDef(QSPIFlash_thread, QSPIFlash_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 8);
    task = osThreadCreate(osThread(QSPIFlash_thread), NULL);
}

static void QSPIFlash_task(const void* args) {

    yaffs_trace_mask = YAFFS_TRACE_ERROR;
    yaffs_start_up();
    int f = 0;
    TRY_FUNC {

        if (yaffs_mount("nand") != 0) {
            EXIT_FUNC;
        }


        f = yaffs_open("nand/file0.txt", O_CREAT | O_WRONLY | O_APPEND, (S_IREAD | S_IWRITE));
        if (f != -1) {
            yaffs_write(f, "HOLA\n", 6);
            yaffs_flush(f);
            yaffs_close(f);
        }

        f = yaffs_open("nand/file0.txt", O_CREAT | O_RDONLY, (S_IREAD | S_IWRITE));
        if (f != -1) {
        	yaffs_read(f, data, 16);
            yaffs_close(f);
        }
    }
    END_FUNC;

#if 0
    bqspi.TransferRate  = MT25TL01G_DTR_TRANSFER;
    bqspi.DualFlashMode = MT25TL01G_DUALFLASH_ENABLE;
    bqspi.InterfaceMode = MT25TL01G_QPI_MODE;

    TRY_FUNC {

        if (BSP_QSPI_Init(0, &bqspi) != BSP_ERROR_NONE) {
            EXIT_FUNC;
        }

        BSP_QSPI_GetInfo(0, &pQSPI_Info);

        if ((pQSPI_Info.FlashSize != 0x8000000) ||
            (pQSPI_Info.EraseSectorSize != 0x2000) ||
            (pQSPI_Info.ProgPageSize != 0x100) ||
            (pQSPI_Info.EraseSectorsNumber != 0x4000) ||
            (pQSPI_Info.ProgPagesNumber != 0x80000)) {
            EXIT_FUNC;
        }

        if (BSP_QSPI_EraseBlock(0, WRITE_READ_ADDR, MT25TL01G_ERASE_4K) != BSP_ERROR_NONE) {
            EXIT_FUNC;
        }

        memset(txblock, 0xAA, QSPI_FLASH_BLOCK_LEN);

        if (BSP_QSPI_Write(0, txblock, WRITE_READ_ADDR, QSPI_FLASH_BLOCK_LEN) != BSP_ERROR_NONE) {
            EXIT_FUNC;
        }

        if (BSP_QSPI_Read(0, rxblock, WRITE_READ_ADDR, QSPI_FLASH_BLOCK_LEN) != BSP_ERROR_NONE) {
            EXIT_FUNC;
        }

        memset(txblock, 0x80, QSPI_FLASH_BLOCK_LEN);

        if (BSP_QSPI_Write(0, txblock, WRITE_READ_ADDR + 0x1000, QSPI_FLASH_BLOCK_LEN) != BSP_ERROR_NONE) {
            EXIT_FUNC;
        }

        if (BSP_QSPI_Read(0, rxblock, WRITE_READ_ADDR + 0x1000, QSPI_FLASH_BLOCK_LEN) != BSP_ERROR_NONE) {
            EXIT_FUNC;
        }
    }
    END_FUNC
#endif

    osThreadTerminate(task);
}
