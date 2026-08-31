#ifndef TORUS_H
#define TORUS_H

#include <stdint.h>

#define TORUS_CELL_COUNT 1000000

int generate_binary_bits(const char *filename);
int generate_ascii_paths(const char *filename, uint16_t torus_id);

#endif
