/*
Copyright (c) 2026 rand0m 

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include "arguments.h"
#include "versdts.h"


void display_help(void) {
    printf("Usage: torg configs" \
            "\n\t[-b --base]\t<BASE_FOLDER>" \
            "\n\t[-d --dest]\t<DESTINATION_FOLDER>" \
            "\n\t[-f --conf]\t<CONF_FILE>" \
            "\n\t[-h --help]\n" \
            "\n\t[-v --version]\n" \
            "\nDefault location:~/.config/cvrt/config.ini\n" \
            "\nSample config.ini" \
            "\n------------------" \
            "\n\t[filter]" \
            "\n\t; enable filtering by file type, if not enabled" \
            "\n\t; then all the files will be copied and renamed" \
            "\n\tenabled = true" \
            "\n\t; set the required file types, if filter is enabled" \
            "\n\t; then this field is mandatory" \
            "\n\tfile_types = .wav .flac" \
            "\n\t[copy]" \
            "\n\t; if filter is enabled, then other files types" \
            "\n\t; will be copied to the new location" \
            "\n\tenabled = true" \
            "\n\t; if copy is enabled, will copy the file types below" \
            "\n\t; * or blank for all files other than" \
            "\n\t; the above filtered ones" \
            "\n\tfile_types =" \
            "\n" \
            "\n\t[core]" \
            "\n\t; base folder, destination folder, and new name prefix" \
            "\n\tbase_dir = Music" \
            "\n\tdest_dir = Music2" \
            "\n\tname_prefix = file_\n\n"); 
}


uint8_t get_arguments (int argc, char **argv, Configs *configs) {
    int choice;

    while (1) {
        static struct option long_configs[] = {
            /* This configs set the secure flag. */
            {"base",   required_argument, 0,'b'},
            {"dest",   required_argument, 0,'d'},
            {"conf",   required_argument, 0,'f'},
            {"help",   no_argument,       0, 'h'},
            {"version", no_argument,      0, 'v'},
            {0,        0,                 0, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        choice = getopt_long (argc, argv, "vhb:d:f:", long_configs, 
                &option_index);

        size_t optarg_len = 0;

        /* Detect the end of the configs. */
        if (choice == -1)
            break;

        switch (choice) {
            case 0:
                if (strncasecmp(long_configs[option_index].name, "help", 5) == 0) {
                    display_help();
                    return 1;
                }
                break;

            case 'b':
                optarg_len = strnlen(optarg, MAX_PATH_LEN);
                if(optarg_len <= MAX_PATH_LEN - 1) {
                    strcpy(configs->base_dir, optarg);
                    configs->base_dir[optarg_len] = '\0';
                    if (configs->base_dir[optarg_len -1] == '/')
                        configs->base_dir[optarg_len-1] ='\0';
                } else {
                    fprintf(stderr, "Base directory path is too long");
                    return 1;
                }
                break;
            case 'd':
                optarg_len = strnlen(optarg, MAX_PATH_LEN);
                if(optarg_len <= MAX_PATH_LEN - 1) {
                    strcpy(configs->dest_dir, optarg);
                    configs->dest_dir[optarg_len] = '\0';
                    if (configs->dest_dir[optarg_len -1] == '/')
                        configs->dest_dir[optarg_len-1] ='\0';
                } else {
                    fprintf(stderr, "Destination directory path is too long");
                    return 1;
                }
                break;
            case 'f':
                optarg_len = strnlen(optarg, MAX_PATH_LEN);
                if(optarg_len <= MAX_PATH_LEN - 1) {
                    strcpy(configs->config_file_path, optarg);
                    configs->config_file_path[optarg_len] = '\0';
                    if (configs->config_file_path[optarg_len -1] == '/')
                        configs->config_file_path[optarg_len-1] ='\0';
                } else {
                    fprintf(stderr, "Conf file path is too long");
                    return 1;
                }
                break;
            case 'h':
                display_help();
                return 1;
            case 'v':
                display_system_details(); 
                display_current_version();
                return 1;
            case '?':
                /* getopt_long already printed an error message. */
                break;
            case ':':   /* missing option argument */
                fprintf(stderr, "%s: option `-%c' requires an argument\n", argv[0], optopt);
                display_help();
                break;
            default:
                return 1;
        }
    }
    /* Print any remaining command line arguments (not configs). */
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
        return -1;
    }
    return 0;
}

