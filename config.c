/*
Copyright (c) 2026 rand0m 

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include<string.h>
#include <stdio.h>
#include <ctype.h>
#include "arguments.h"
#include "ini.h"
#include "config.h"


static char * home_path_cat(const char *path, const char *home, char *output) {
    memset (output, 0, MAX_PATH_LEN);
    size_t path_len = strlen(path);
    size_t home_len = strlen(home);

    if (path_len + home_len > MAX_PATH_LEN -1)
        return NULL;

    for (size_t i = 0; i < home_len; i++) {
        output[i] = home[i];
    }
    if (output[home_len] == '/') { 
        output[home_len] = '\0';
        home_len--;
    }

    /* skipping ~ */
    for (size_t i = 1; i < path_len; i++) {
        output[home_len + i -1] = path[i];
    }
    output[MAX_PATH_LEN - 1] = 0;
    return output;
}

/* Case insensitive string compare */
static int strcmpci(const char *a, const char *b) {
    for (;;) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a) {
            return d;
        }
        a++, b++;
    }
}

static void trim_l (char *str) {
    int index =0, i, j ;

    /*  the last index of leading space character */
    while (str[index] == ' ' || str[index] == '\t' || str[index] == '\n') {
        index++;
    }

    if (index > 0) {
        for (i = index, j = 0; str[i] != '\0'; i++, j++) {
            str[j] = str[i];
        }
        str[j] = '\0'; 
    }
}

void initialise_configs (Configs *configs) {
    configs->isfilter = false;
    memset (configs->filter_types, '\0', FILE_TYPE_LEN);
    memset (configs->base_dir, '\0', MAX_PATH_LEN);
    memset (configs->dest_dir, '\0', MAX_PATH_LEN);
    memset (configs->config_file_path, '\0', MAX_PATH_LEN);
    memset (configs->name_prefix, '\0', PREFIX_LEN);
    configs->do_generate_dictionary = false;
}

uint8_t process_configs(int argc, char **argv, Configs *configs) {

    if (get_arguments(argc, argv, configs))
        return 1;

    char default_config[FILE_TYPE_LEN]= {'\0'};
    snprintf(default_config, FILE_TYPE_LEN -1, "%s/.config/torg/config.ini", getenv("HOME"));
   
    char *config_file = strnlen(configs->config_file_path, MAX_PATH_LEN) == 0 ? default_config : configs->config_file_path;

    /* config file handler */
    ini_t *config = NULL;

    char *file_types = NULL, *base_dir = NULL, *dest_dir = NULL, *name_prefix = NULL;

    if ((config = ini_load(config_file)) == NULL){
        /* create the folder first */
        fprintf(stderr, "%d %s: Error: configuration file %s is missing\n", ((__LINE__)-6),__func__, config_file);
        display_help();
        return 1;
    } 

    /* config file loaded */
    if ((strcmpci(ini_get(config, "filter", "enabled"), "true") == 0) ||
            (strcmpci(ini_get(config, "filter", "enabled"), "yes") == 0)) {  
        configs->isfilter = true;
        if ((file_types = (char *)ini_get(config, "filter", "file_types")) == NULL) {
            fprintf(stderr, "%d %s: Error: 'file_types' configuration is missing\n", ((__LINE__)-6),__func__);
            ini_free(config);
            return 1;
        }
        strncpy(configs->filter_types, file_types, FILE_TYPE_LEN - 1);
        configs->filter_types[FILE_TYPE_LEN - 1] = '\0';
    }

    /* To hold the home path in case ~ was used */
    char *home_path_output = (char *) malloc(MAX_PATH_LEN);

    base_dir = (char *)ini_get(config, "core", "base_dir");
    dest_dir = (char *)ini_get(config, "core", "dest_dir");

    if (strnlen(configs->base_dir, MAX_PATH_LEN) == 0) {
        if (base_dir == NULL) {
            fprintf(stderr, "%d %s: Error: 'base_dir' configuration is missing\n", ((__LINE__)-6),__func__);
            ini_free(config);
            return 1;
        } else {
            size_t baselen = strnlen(base_dir, MAX_PATH_LEN);
            if (baselen < MAX_PATH_LEN) {
                if (base_dir[baselen - 1] == '/')
                    base_dir[baselen - 1] = '\0';
                if (base_dir[0] == '~') {
                    char *home = getenv("HOME");
                    if (baselen + strlen(home) < MAX_PATH_LEN -1) {
                        snprintf(configs->base_dir, MAX_PATH_LEN, "%s", home_path_cat(base_dir, home, home_path_output));
                    } else {
                        fprintf(stderr, "%d %s: Base directory path is too long", ((__LINE__)-6),__func__);
                        ini_free(config);
                        return 1;
                    }
                }
                else {
                    strncpy(configs->base_dir, base_dir, MAX_PATH_LEN - 1);
                }
                configs->base_dir[MAX_PATH_LEN - 1] = 0;
            } else {
                fprintf(stderr, "%d %s: Base directory path is too long", ((__LINE__)-6),__func__);
                ini_free(config);
                return 1;
            }
        }
    }

    if (strnlen(configs->dest_dir, MAX_PATH_LEN) == 0) {
        if (dest_dir == NULL) {
            fprintf(stderr, "%d %s:Error: 'dest_dir' configuration is missing\n",((__LINE__)-6),__func__);
            return 1;
        } else {
            size_t destlen = strnlen(dest_dir, MAX_PATH_LEN);
            if (destlen < MAX_PATH_LEN) {
                if (dest_dir[destlen - 1] == '/')
                    dest_dir[destlen - 1] = '\0';
                if (dest_dir[0] == '~') {
                    char *home = getenv("HOME");
                    if (destlen + strlen(home) < MAX_PATH_LEN - 1) {
                        snprintf(configs->dest_dir, MAX_PATH_LEN, "%s", home_path_cat(dest_dir, home, home_path_output));
                    } else {
                        fprintf(stderr, "%d %s: Destination directory path is too long",((__LINE__)-6),__func__);
                        ini_free(config);
                        return 1;
                    }
                }
                else {
                    strncpy(configs->dest_dir, dest_dir, MAX_PATH_LEN - 1);
                }
                configs->dest_dir[MAX_PATH_LEN - 1] = 0;
            } else {
                fprintf(stderr, "%d %s: Destination directory path is too long",((__LINE__)-6),__func__);
                ini_free(config);
                return 1;
            }
        }
    }

    if ((name_prefix = (char *)ini_get(config, "core", "name_prefix")) == NULL) {
        fprintf(stderr, "%d %s: Error: 'name_prefix' configuration is missing\n", ((__LINE__)-6),__func__);
        ini_free(config);
        return 1;
    } else {
        name_prefix[PREFIX_LEN -1] = 0;
        trim_l(name_prefix);
        strncpy(configs->name_prefix, name_prefix, PREFIX_LEN - 1);
        configs->name_prefix[PREFIX_LEN - 1] = 0;
    }

    if ((strcmpci(ini_get(config, "core", "generate_dict"), "true") == 0) ||
            (strcmpci(ini_get(config, "core", "generate_dict"), "yes") == 0)) {  
        configs->do_generate_dictionary = true;
    } else {
        configs->do_generate_dictionary = false;
    }
    
    free(home_path_output);
    home_path_output = NULL;
    ini_free(config);
    return 0;
}
