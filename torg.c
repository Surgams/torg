/*
Copyright (c) 2026 rand0m 

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "torg.h"
#include "config.h"

static void display_configs (Configs configs) {
    printf("\nYou've chosen the below configs:\n\n");
    if (configs.isfilter) {
        printf("\tCopy and rename files of types %s \n", configs.filter_types);
        if (configs.iscopy)
            printf("\tand copy file types %s \n", configs.copy_types);
    } else  
        printf ("\tCopy and rename all files\n");
    printf("\tin directory %s to type %s in directory %s\n", configs.base_dir, configs.name_prefix ,configs.dest_dir);
    printf("Are you sure you want to proceed [Y/n]:");
}


int main(int argc, char * const argv[]) {
 
    /* Create configuration folder */
    char config_path[MAX_PATH_LEN] = {'\0'};
    snprintf(config_path, MAX_PATH_LEN -1, "%s/.config/cvrt", getenv("HOME"));
    
    /* Not ready yet
    if (create_dir (config_path) == -1) {
        fprintf(stderr, "[fun: %s - %s:%d] Error: unable to create config folder: (%s)\n", 
                __func__, __FILE__, __LINE__, strerror(errno));
        return 1;
    }
*/
    Configs *configs = (Configs *) malloc(sizeof(Configs));

    initialise_configs(configs);
    if (process_configs(argc, (char **)argv, configs))
        return 0;

    display_configs(*configs);
    int proceed = getchar();
    if (proceed != 'y' && proceed != 'Y')
        return 0;

    free(configs);
    return 0;
}



