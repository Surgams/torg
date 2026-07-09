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
#include "exec.h"
#include "filemgm.h"

#define DELIMETER " "
#define CMD_ARG 200
char **ftypes;

static int cmd_arg_split (char *cmd, char **cmd_array, const char *delimiter) {
    char *token;
    int counter = 0;

    while ((token = strsep(&cmd, delimiter)) != NULL) {
        bool isempty = true;
        while (*token != '\0') {
            if (!isspace(*token)) {
                isempty = false;
                break;
            }
        }
        if (isempty)
            continue;

        cmd_array[counter] = (char *) malloc((strlen(token) + 1) * sizeof(char));
        sprintf(cmd_array[counter], "%s", token);

        if (counter++ >= CMD_ARG - 1)
            return 1;
    }
    cmd_array[counter] = NULL;
    return 0;
}

void convert_files_recursively (Configs configs) {
    char *base_path = configs.base_dir;
    char *dest_path = configs.dest_dir;

    int output, pid;
    char path[MAX_PATH_LEN], new_path[MAX_PATH_LEN];
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

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if ((point = strrchr(dp->d_name,'.')) != NULL) {

                if (strstr(configs.filter_types, point) != NULL) {

                    /* Preparing input and output files */
                    char  i_tmp[MAX_PATH_LEN] = {}, o_tmp[MAX_PATH_LEN] = {};
                    
                    /* output temp file format string */
                    char format[20] = "%s/%.*s";
                    strcat(format, configs.cvrt_type);

                    snprintf(o_tmp, sizeof(o_tmp) - 1, format, dest_path, (int)(strlen(dp->d_name) - strlen(point)), dp->d_name);
                    snprintf(i_tmp, sizeof(i_tmp) - 1, "%s/%s", base_path, dp->d_name);

                    char *fullcmd = strdup(configs.full_cmd);

                    /* Preparing the arguments array */
                    char *args[CMD_ARG] = {0};
                    cmd_arg_split(fullcmd, args, DELIMETER); 

                    /* Replacing the placeholders '?' with input and output filenames */
                    bool is_inputfile = true;
                    for (int i = 0; args[i] != NULL && i < CMD_ARG; i++) {
                        if (strcmp(args[i], "?") == 0) {
                            if (is_inputfile) {
                                args[i] = (char *)realloc (args[i], strlen(i_tmp) + 1);
                                strcpy(args[i], i_tmp);
                                args[i][strlen(i_tmp)] = '\0';
                                is_inputfile = false;
                            } else {
                                args[i] = (char *)realloc (args[i], strlen(o_tmp) + 1);
                                strcpy(args[i], o_tmp);
                                args[i][strlen(o_tmp)] = '\0';
                            }
                        }
                    }
                    if((pid = fork()) == 0) {
                        execvp(args[0], (char *const *)args);
                        _exit(0);
                    } else if (pid > 0) {
                        wait(NULL);
                        /* Freeing the args array */
                        for(int i = 0; args[i] != NULL && i < CMD_ARG; i++) {
                            free(args[i]);
                            args[i] = NULL;
                        }
                        if (fullcmd != NULL) {
                            free(fullcmd);
                            fullcmd = NULL;
                        }
                    }
                } else if (configs.iscopy) {
                    if ((strcmp(configs.copy_types, "*") == 0) || (strstr(configs.copy_types, point) != NULL)) {

                        /* Preparing source and destination files */
                        char  src_tmp[MAX_PATH_LEN] = '\0', dest_tmp[MAX_PATH_LEN] = '\0';

                        snprintf(dest_tmp, sizeof(dest_tmp) - 1, "%s/%s", dest_path, dp->d_name);
                        snprintf(src_tmp, sizeof(src_tmp) - 1, "%s/%s", base_path, dp->d_name);
                        copy_file (src_tmp, dest_tmp);
                    }
                }
            }

            /* Construct new path from our base path */
            sprintf(path, "%s/%s", base_path, dp->d_name);
            sprintf(new_path, "%s/%s", dest_path, dp->d_name);

            if ((output = create_dir(dest_path)) != 0) {
                fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
                return;
            }
            Configs tmp_configs;
            strcpy(tmp_configs.base_dir, path);
            strcpy(tmp_configs.dest_dir, new_path);
            strcpy(tmp_configs.full_cmd, configs.full_cmd);
            strcpy(tmp_configs.cvrt_type, configs.cvrt_type);
            strcpy(tmp_configs.filter_types, configs.filter_types);
            tmp_configs.iscopy = configs.iscopy;
            strcpy(tmp_configs.copy_types, configs.copy_types);
            convert_files_recursively(tmp_configs);
        }
    }
    if (dp == NULL)
    closedir(dir);
}

void free_cmd () {
    for(int i = 0; ftypes != NULL && ftypes[i] != NULL && i < FILE_TYPE_LEN; i++) {
        free(ftypes[i]);
        ftypes[i] = NULL;
    }
    if(ftypes != NULL) {
        free(ftypes);
        ftypes = NULL;
    }
}

