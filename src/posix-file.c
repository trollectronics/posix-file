#include <stddef.h>
#include <stdint.h>
#include <posix-file.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <mem.h>
#include <fat.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct FileDescriptor FileDescriptor;
struct FileDescriptor {
	bool open;
	size_t seek_offset;
	size_t size;
	uint8_t buf[512];
};

static FileDescriptor file[MAX_FD_OPEN];

extern uint8_t fat_buf[512];

int open(const char *pathname, int flags) {
	int namelen = strlen(pathname);
	char fatname[namelen + 1];
	int i;
	int fd;
	
	for(i = 0; i < namelen + 1; i++) {
		fatname[i] = toupper(pathname[i]);
	}
	
	if((fd = fat_open(fatname, flags)) < 0)
		return -1;
	
	file[fd].open = true;
	file[fd].seek_offset = 0;
	file[fd].size = fat_fsize(fd);
	
	return fd;
}

int close(int fd) {
	file[fd].open = false;
	fat_close(fd);
	return 0;
}

ssize_t read(int fd, void *buf, size_t count) {
	uint8_t *a, *b;
	size_t i;
	size_t left_in_sector;
	ssize_t r;
	
	if(!file[fd].open)
		return -1;
	
	if(file[fd].seek_offset >= file[fd].size)
		return 0;
	
	r = MIN(count, (file[fd].size - file[fd].seek_offset));
	
	a = buf;
	b = file[fd].buf;
	
	left_in_sector = (512 - (file[fd].seek_offset & 0x1FF)) & 0x1FF;
	b += file[fd].seek_offset & 0x1FF;
	
	/* Complete previous partial sector */
	while(left_in_sector && count) {
		*a++ = *b++;
		file[fd].seek_offset++;
		
		left_in_sector--;
		count--;
		
		if(file[fd].seek_offset >= file[fd].size)
			goto done;
	}
	
	/* Full sectors*/
	while(count >= 512) {
		fat_read_sect(fd);
		uint32_t *src = (uint32_t *) fat_buf;
		for(i = 0; i < 512/4; i++) {
			*((uint32_t *) a) = *src++;
			a += 4;
		}
		
		count -= 512;
		file[fd].seek_offset += 512;
		if(file[fd].seek_offset >= file[fd].size)
			goto done;
	}
	
	/* New partial sector */
	if(count) {
		b = file[fd].buf;
		fat_read_sect(fd);
		uint32_t *src = (uint32_t *) fat_buf;
		for(i = 0; i < 512/4; i++) {
			*((uint32_t *) b) = *src++;
			b += 4;
		}
		
		file[fd].seek_offset += count;
		b = file[fd].buf;
		while(count) {
			*a++ = *b++;
			count--;
		}
	}
	
	done:
	return r;
}

ssize_t write(int fd, const void *buf, size_t count) {
	return -1;
}

off_t lseek(int fd, off_t offset, int whence) {
	if(!file[fd].open)
		return -1;

	switch (whence) {
		case SEEK_SET:
			file[fd].seek_offset = offset;
			break;
		case SEEK_CUR:
			file[fd].seek_offset += offset;
			break;
		case SEEK_END:
			file[fd].seek_offset = file[fd].size + offset;
			break;
	}

	if (file[fd].seek_offset < file[fd].size) {
		fat_seek(fd, file[fd].seek_offset & ~0x1FF);
		uint8_t *b = file[fd].buf;
		fat_read_sect(fd);
		uint32_t *src = (uint32_t *) fat_buf;
		for(i = 0; i < 512/4; i++) {
			*((uint32_t *) b) = *src++;
			b += 4;
		}
	}

	return (off_t) file[fd].seek_offset;
}

