#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdlib.h>

void initialise_configs (Configs *configs);
uint8_t process_configs (int argc, char **argv, Configs *configs);

#endif
