#include <stdlib.h>
#include <string.h>
#include <fat.h>
#include <dirent.h>

typedef struct Dir Dir;
struct Dir {
	char dirname[NAME_MAX + 1];
	struct dirent d;
	struct FATDirList fatdir;
	int index;
};

DIR *opendir(const char *name) {
	Dir *d;
	
	if(!(d = malloc(sizeof(Dir))))
		return NULL;
	d->dirname[NAME_MAX] = 0;
	
	strncpy(d->dirname, name, NAME_MAX);
	
	d->index = 0;
	d->d.d_type = DT_UNKNOWN;
	d->d.d_reclen = sizeof(struct dirent);
	
	return d;
}

int closedir(DIR *dirp) {
	free(dirp);
	
	return 0;
}

struct dirent *readdir(DIR *dirp) {
	Dir *dir = dirp;
	
	for(;;) {
		if(!fat_dirlist(dir->dirname, &dir->fatdir, 1, dir->index))
			return NULL;
		
		if(dir->fatdir.attrib & 0x8) {
			dir->index++;
			continue;
		} else if(dir->fatdir.attrib & 0x10) {
			dir->d.d_type = DT_DIR;
		} else {
			dir->d.d_type = DT_REG;
		}
		
		dir->d.d_off = dir->index;
		strcpy(dir->d.d_name, dir->fatdir.filename);
		dir->index++;
		break;
	}
	
	return &dir->d;
}
