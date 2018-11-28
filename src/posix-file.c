#include <stddef.h>
#include <stdint.h.h>
#include <posix-file.h>
#include <fat.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef FileDescriptor FileDescriptor;
struct FileDescriptor {
	bool open;
	size_t seek_offset;
	size_t size;
	uint8_t buf[512];
};

static FileDescriptor file[MAX_FD_OPEN];

int open(const char *pathname, int flags) {
	int namelen = strlen(pathname);
	char fatname[namelen + 1];
	int i;
	int fd;
	
	for(i = 0; i < namelen + 1; i++) {
		fatname[i] = toupper(pathname[i]);
	}
	
	if(fd = fat_open(path, flags) < 0)
		return -1;
	
	file[fd].open = true;
	file[fd].seek_offset = 0;
	file[fd].size = fat_size(fd)*512;
	
	return fd;
}

int close(int fd) {
	file[fd].open = false;
	return fat_close(fd);
}

ssize_t read(int fd, void *buf, size_t count) {
	uint8_t *a, *b;
	size_t i, j;
	size_t left_in_sector;
	ssize_t r;
	
	if(!file[fd].open)
		return -1;
	
	r = MIN(count, (file[fd].size - file[fd].seek_offset));
	
	a = buf;
	b = file[fd].buf;
	
	left_in_sector = file[fd].seek_offset & 0x1FF;
	b += left_in_sector;
	
	/* Complete previous partial sector */
	while(left_in_sector && count) {
		*a++ = *b++;
		file[fd].seek_offset++;
		
		left_in_sector--;
		count--;
		
		if(file[fd].seek_offset >= file[fd].file_size)
			goto done;
	}
	
	/* Full sectors*/
	while(count >= 512) {
		fat_read_sect(fd);
		uint32_t *src = (uint32_t *) fat_buf;
		for(i = 0; i < 512/4; i++) {
			*((uint32_t*) a)++ = *src++;
		}
		
		count -= 512;
		file[fd].seek_offset += 512;
		if(file[fd].seek_offset >= file[fd].file_size)
			goto done;
	}
	
	/* New partial sector */
	if(count) {
		b = file[fd].buf;
		fat_read_sect(fd);
		uint32_t *src = (uint32_t *) fat_buf;
		for(i = 0; i < 512/4; i++) {
			*((uint32_t*) b)++ = *src++;
		}
		
		file[fd].seek_offset += count;
		
		while(count--) {
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

off_t lseek() {
	return (off_t) -1;
}

