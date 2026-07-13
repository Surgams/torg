/*
Copyright (c) 2026 rand0m 

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include "exec.h"
#include "filemgm.h"

static int check_if_dir(char *path) {
    struct stat st = {0};
    if (stat(path, &st) == - 1) {
        return 0; 
    } else if (st.st_mode & S_IFDIR) {
        return 1;
    } else {
        return 0;
    }
}

void copy_files_recursively (Configs configs) {
    char *base_path = configs.base_dir;
    char *dest_path = configs.dest_dir;

    int output;
    char path[MAX_PATH_LEN] = "", new_path[MAX_PATH_LEN] = "";
    struct dirent *dp;
    char *point;
    
    DIR *dir = opendir(base_path);
    if (!dir) {
        fprintf(stderr, "Error number %d: %s: %s\n", errno, strerror(errno), base_path);
        return;
    }

    if ((output = create_dir(dest_path)) != 0) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }
    int file_index = 0;
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, "~.") != 0 && strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if ((point = strrchr(dp->d_name,'.')) != NULL) {

                if (strstr(configs.filter_types, point) != NULL) {

                    /* Preparing source and destination files */
                    char  src_tmp[MAX_PATH_LEN] = "", dest_tmp[MAX_PATH_LEN] = "";

                    snprintf(dest_tmp, sizeof(dest_tmp) - 1, "%s/%s%03d%s", dest_path, configs.name_prefix, file_index++, point);
                    snprintf(src_tmp, sizeof(src_tmp) - 1, "%s/%s", base_path, dp->d_name);
                    copy_file (src_tmp, dest_tmp);
                    /*** Missing the dictionary******/
                }
            }

            /* Construct new path from our base path */
            sprintf(path, "%s/%s", base_path, dp->d_name);
            sprintf(new_path, "%s/%s", dest_path, dp->d_name);

            if (check_if_dir(path)) {
                if ((output = create_dir(dest_path)) != 0) {
                    fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
                    return;
                }
                /*** Need to update the configuration ****/
                Configs tmp_configs;
                strcpy(tmp_configs.base_dir, path);
                strcpy(tmp_configs.dest_dir, new_path);
                strcpy(tmp_configs.filter_types, configs.filter_types);
                copy_files_recursively(tmp_configs);
            }
        }
    }
    if (dp == NULL)
    closedir(dir);
}


