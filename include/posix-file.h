#ifndef POSIX_FILE_H_
#define POSIX_FILE_H_

#define	MAX_FD_OPEN		4

#define	O_WRONLY		2
#define	O_RDONLY		1
#define	O_RDWR			(O_RDONLY | O_WRONLY)

enum {
	SEEK_SET,
	SEEK_CUR,
	SEEK_END,
};

int fsinit();

int open(const char *pathname, int flags);
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
off_t lseek();

#endif
