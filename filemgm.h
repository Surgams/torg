#ifndef FILEMGM_H
#define FILEMGM_H

#include <unistd.h>

int8_t create_dir (char *path);
void copy_file (char *srcfile, char *destfile);

#endif
