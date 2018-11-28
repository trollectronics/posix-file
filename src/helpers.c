static int _fat_read_sd(uint32_t sector, uint8_t *data) {
	SDStreamStatus status;
	
	status = SD_STREAM_STATUS_BEGIN;
	
	sd_stream_read_block(&status, sector);
	if(status == SD_STREAM_STATUS_FAILED) {
		printf("sd_stream_read_block failed at SD_STREAM_STATUS_BEGIN\n");
		return -1;
	}
	
	while(status >= 1)
		*data++ = sd_stream_read_block(&status);
	
	if(status == SD_STREAM_STATUS_FAILED) {
		printf("sd_stream_read_block failed\n");
		return -1;
	}
	
	return 0;
}

static int _fat_write_sd(uint32_t sector, uint8_t *data) {
	SDStreamStatus status;
	
	status = SD_STREAM_STATUS_BEGIN;
	
	sd_stream_write_block(&status, sector);
	if(status == SD_STREAM_STATUS_FAILED)
		return -1;
	
	while(status >= 1)
		sd_stream_write_block(&status, *data++);
	
	if(status == SD_STREAM_STATUS_FAILED)
		return -1;
	
	return 0;
}

int fsinit() {
	/* Init filesystem functionality */
	
	if(fat_init(_fat_read_sd, _fat_write_sd, fat_buf) < 0) {
		return -1;
	}
	
	return 0;
}