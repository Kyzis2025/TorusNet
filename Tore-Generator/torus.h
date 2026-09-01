#ifndef TORUS_H
#define TORUS_H

#include <stdint.h>

#define TORUS_CELL_COUNT 1000000
#define CASE_ID_COUNT    32768

int generate_case_ids(uint16_t *case_ids, uint16_t *unused_ids, uint16_t *torus_id);
int generate_binary_bits(const char *filename);
int generate_ascii_paths(const char *filename, uint16_t *case_ids, uint16_t torus_id);
int save_unused_ids(const char *filename, uint16_t *unused_ids);

#endif
