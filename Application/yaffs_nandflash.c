/*
 * yaffs_nandflash.c
 *
 *  Created on: Jun 23, 2020
 *      Author: ericson
 */

#include "yaffsfs.h"
#include "stm32h747i_discovery_qspi.h"

static BSP_QSPI_Init_t bqspi;
static const char dev_name[] = "nand";
static const char backing_file_name[] = "emfile-nand";
static struct yaffs_dev dev;


int yaffs_start_up(void) {

//	bqspi.TransferRate = MT25TL01G_STR_TRANSFER;
//	bqspi.DualFlashMode = MT25TL01G_DUALFLASH_DISABLE;
//	bqspi.InterfaceMode = MT25TL01G_SPI_MODE;
//	BSP_QSPI_Init(0, &bqspi);

	dev->param.name = dev_name;
	dev->driver_context = NULL;
	dev->param.start_block = 0;
	dev->param.end_block = 1023;
	dev->param.chunks_per_block = 32;
	dev->param.total_bytes_per_chunk = 512;
	dev->param.spare_bytes_per_chunk = 16;
	dev->param.is_yaffs2 = 0;
	dev->param.use_nand_ecc = 0;
	dev->param.n_reserved_blocks = 5;
	dev->param.inband_tags = 0;
	dev->param.n_caches = 10;

	dev->tagger.write_chunk_tags_fn = NULL;
	dev->tagger.read_chunk_tags_fn = NULL;
	dev->tagger.query_block_fn = NULL;
	dev->tagger.mark_bad_fn = NULL;
	yaffs_tags_compat_install(dev);

	dev->drv.drv_write_chunk_fn = yaffs_nand_drv_WriteChunk;
	dev->drv.drv_read_chunk_fn = yaffs_nand_drv_ReadChunk;
	dev->drv.drv_erase_fn = yaffs_nand_drv_EraseBlock;
	dev->drv.drv_mark_bad_fn = yaffs_nand_drv_MarkBad;
	dev->drv.drv_check_bad_fn = yaffs_nand_drv_CheckBad;
	dev->drv.drv_initialise_fn = yaffs_nand_drv_Initialise;
	dev->drv.drv_deinitialise_fn = yaffs_nand_drv_Deinitialise;

	/* The yaffs device has been configured, install it into yaffs */
	yaffs_add_device(dev);

	return YAFFS_OK;


}

