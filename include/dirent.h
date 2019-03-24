#ifndef DIRENT_H_
#define DIRENT_H_

#include <stdint.h>

#define NAME_MAX 255

#define DT_UNKNOWN 0

typedef uint64_t ino_t;
typedef int64_t off_t;

struct dirent {
	ino_t d_ino;
	off_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[NAME_MAX + 1];
};

typedef void DIR;

DIR *opendir(const char *name);
int closedir(DIR *dirp);
struct dirent *readdir(DIR *dirp);

#endif
