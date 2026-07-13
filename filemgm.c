/*
Copyright (c) 2026 rand0m 

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include "filemgm.h"

int8_t create_dir (const char *path) {
    struct stat st = {0};

    if (stat(path, &st) == - 1) {
        return mkdir(path, 0700);
    }
    return 0;
}


void copy_file (char *srcfile, char *destfile) {
    int sfd, dfd;
    char *src, *dest;
    size_t filesize;

    sfd = open(srcfile, O_RDONLY);
    
    if (sfd == -1) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    filesize = lseek(sfd, 0, SEEK_END);

    src = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, sfd, 0);

    if (src == MAP_FAILED) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    dfd = open(destfile, O_RDWR | O_CREAT, 0700);

    if (dfd == -1) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    if (ftruncate(dfd, filesize) != 0) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    dest = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dfd, 0);

    if (dest == MAP_FAILED) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    memcpy(dest, src, filesize);
    
    /* unmap */
    munmap(src, filesize);
    munmap(dest, filesize);

    /* close handlers */
    close(sfd);
    close(dfd);

}
