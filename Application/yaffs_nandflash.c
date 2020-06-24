/*
 * yaffs_nandflash.c
 *
 *  Created on: Jun 23, 2020
 *      Author: ericson
 */

#include "yaffsfs.h"
#include "yaffs_guts.h"
#include "yaffs_tagscompat.h"
#include "stm32h747i_discovery_qspi.h"

static BSP_QSPI_Init_t bqspi;
static const char dev_name[] = "nand";
static const char backing_file_name[] = "emfile-nand";
static struct yaffs_dev devqspi;

#define SPARE_ADDR_START 		0x0 		/* 4MB*/
#define	CHUNK_ADDR_START		0x3E0000    /* 124MB */
#define CHUNK_SIZE				1024*2
#define SPARE_SIZE				64
#define SPARE_LEN				SPARE_SIZE
#define BLOCK_SIZE				1024*4

#define GET_CHUNK_ADDR(_nand_chunk_)	(CHUNK_ADDR_START + ((CHUNK_SIZE)*(_nand_chunk_)))
#define GET_SPARE_ADDR(_nand_chunk_)	(SPARE_ADDR_START + ((SPARE_SIZE)*(_nand_chunk_)))

static uint8_t norBuff[BLOCK_SIZE];

static int yaffs_nand_drv_WriteChunk(struct yaffs_dev *dev, int nand_chunk,
                                     const u8 *data, int data_len,
                                     const u8 *oob, int oob_len) {

  if (data != NULL) {
	  BSP_QSPI_Read(0, norBuff, CHUNK_ADDR_START + ((nand_chunk/2) + 1)*BLOCK_SIZE, BLOCK_SIZE);
	  BSP_QSPI_EraseBlock(0, CHUNK_ADDR_START + ((nand_chunk/2) + 1)*BLOCK_SIZE, MT25TL01G_ERASE_4K);
	  BSP_QSPI_Write(0, (uint8_t*)data, GET_CHUNK_ADDR(nand_chunk), data_len);
  }
//  if (data) programPage(nand_chunk, (u8 *)data, data_len);


  if (oob != NULL) {
	  BSP_QSPI_Write(0, (uint8_t*)oob, GET_SPARE_ADDR(nand_chunk), oob_len);
  }
//  if (oob) programSpare(nand_chunk, buf, SPARE_LEN);

  return YAFFS_OK;

}

static int yaffs_nand_drv_ReadChunk(struct yaffs_dev *dev, int nand_chunk,
				   u8 *data, int data_len,
				   u8 *oob, int oob_len,
				   enum yaffs_ecc_result *ecc_result) {


	u8 buf[SPARE_LEN];

	if (data != NULL) {
		BSP_QSPI_Read(0, data, GET_CHUNK_ADDR(nand_chunk), data_len);
	}
//	if (data) readPage (nand_chunk, data, data_len);
	if (oob != NULL) {
		BSP_QSPI_Read(0, buf, GET_SPARE_ADDR(nand_chunk), oob_len);
	}
//	if (oob) readSpare(nand_chunk, buf, SPARE_LEN);

//	if (oob_len>SPARE_SIZE) oob_len = SPARE_SIZE;
	memcpy(oob, buf, oob_len);
	return YAFFS_OK;
}

static int yaffs_nand_drv_EraseBlock (struct yaffs_dev *dev, int block_no) {
	BSP_QSPI_EraseBlock(0, CHUNK_ADDR_START + BLOCK_SIZE*block_no, MT25TL01G_ERASE_4K);
	BSP_QSPI_EraseBlock(0, SPARE_ADDR_START + BLOCK_SIZE*block_no, MT25TL01G_ERASE_4K);
	return YAFFS_OK;
}

static int yaffs_nand_drv_MarkBad(struct yaffs_dev *dev, int block_no)
{
	u8 buf[SPARE_LEN];
	memset(buf, 0xFF, SPARE_LEN);
	buf[0] = 'Y';
	buf[1] = 'Y';
	BSP_QSPI_Write(0, buf, SPARE_ADDR_START + SPARE_SIZE*2*block_no, SPARE_LEN);
//	programSpare(page, buf, SPARE_LEN);
	return YAFFS_OK;
}

static int yaffs_nand_drv_CheckBad(struct yaffs_dev *dev, int block_no)
{
	u8 buf[SPARE_LEN];

	BSP_QSPI_Read(0, buf, SPARE_ADDR_START + SPARE_SIZE*2*block_no, SPARE_LEN);
//	readSpare(page, buf, SPARE_LEN);

	if(yaffs_hweight8(buf[0]) + yaffs_hweight8(buf[1]) < 14)
		return YAFFS_FAIL;
	else
		return YAFFS_OK;
}

static int yaffs_nand_drv_Initialise(struct yaffs_dev *dev) {
  bqspi.TransferRate = MT25TL01G_STR_TRANSFER;
  bqspi.DualFlashMode = MT25TL01G_DUALFLASH_DISABLE;
  bqspi.InterfaceMode = MT25TL01G_SPI_MODE;
  BSP_QSPI_Init(0, &bqspi);
  BSP_QSPI_EraseChip(0);
  return YAFFS_OK;
}

static int yaffs_nand_drv_Deinitialise(struct yaffs_dev *dev)
{
	BSP_QSPI_DeInit(0);
	return YAFFS_OK;
}

int yaffs_start_up(void) {

	struct yaffs_dev *dev = &devqspi;

	dev->param.name = dev_name;
	dev->driver_context = NULL;
	dev->param.start_block = 0;
	dev->param.end_block = 9;
	dev->param.chunks_per_block = 2;
	dev->param.total_bytes_per_chunk = 2048;
	dev->param.spare_bytes_per_chunk = 64;
	dev->param.is_yaffs2 = 0;
	dev->param.use_nand_ecc = 0;
	dev->param.n_reserved_blocks = 5;
	dev->param.inband_tags = 0;
	dev->param.n_caches = 5;

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

