/*
 * yaffs_osglue.c
 *
 *  Created on: Jun 23, 2020
 *      Author: Ericson Joseph
 */
#include "yaffsfs.h"
#include "yaffs_osglue.h"
#include "FreeRTOS.h"
#include "task.h"

static int yaffsfs_lastError;

void yaffsfs_Lock(void) {

}

void yaffsfs_Unlock(void) {

}


u32 yaffsfs_CurrentTime(void) {
	return 0;
}

void yaffsfs_SetError(int err) {
	yaffsfs_lastError = err;
}

void *yaffsfs_malloc(size_t size) {
	return pvPortMalloc(xSize);
}

void yaffsfs_free(void *ptr) {
	vPortFree(pv);
}

int yaffsfs_CheckMemRegion(const void *addr, size_t size, int write_request) {
	if(!addr)
		return -1;
	return 0;
}

void yaffsfs_OSInitialisation(void) {

}


