#ifndef UNISTD_H_
#define UNISTD_H_

enum {
	SEEK_SET,
	SEEK_CUR,
	SEEK_END,
};

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, void *buf, size_t count);
off_t lseek(int fd, off_t offset, int whence);

int close(int fd);

int getpagesize(void);

#endif

