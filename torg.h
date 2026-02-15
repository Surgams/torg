#ifndef TORG_H
#define TORG_H

#include <stdbool.h>

#define MAX_PATH_LEN 1500
#define FILE_TYPE_LEN 50
#define PREFIX_LEN 100

typedef struct Configs_ {
    bool isfilter;
    char filter_types[FILE_TYPE_LEN];
    bool iscopy;
    char copy_types[FILE_TYPE_LEN];
    char base_dir[MAX_PATH_LEN];
    char dest_dir[MAX_PATH_LEN];
    char name_prefix[PREFIX_LEN];
    char config_file_path[MAX_PATH_LEN];
} Configs;

#endif
